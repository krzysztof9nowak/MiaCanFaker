from frames import *
from waveshare import CAN
import time
import threading
from collections import namedtuple
import queue
from enum import IntEnum


class BMS_STATE(IntEnum):
    READY = 0
    RUN = 1
    DOWNGRADED = 2
    CHARGER = 3
    ERROR = 4
    INIT = 5
    STOP = 6


class Scheduler(object):
    def __init__(self, can):
        self.can = can
        self.rx_thread = threading.Thread(target=self.receiver)
        self.tx_thread = threading.Thread(target=self.transmitter())

        self.periodics = []
        self.register_periodic(self.send_bms_status, 0.1)
        self.register_periodic(self.send_bms_imax, 0.1)
        self.register_periodic(self.send_default_bms, 0.1)
        self.handlers = {}
        self.register_handler(0x630, self.cmd_bms)
        self.tx_queue = queue.Queue()
        self.bms_state = BMS_STATE.READY

    def run(self):
        self.rx_thread.start()
        self.tx_thread.start()

        for periodic in self.periodics:
            t = time.time()
            dt = t - periodic['last']
            if dt > periodic['period']:
                periodic['func']()
                periodic['last'] = t
            time.sleep(0.01)

    def receiver(self):
        while True:
            can_frame = self.can.frame_recv()

            if can_frame.can_id in mia_frames:
                cls = mia_frames[can_frame.can_id]
                frame = cls.from_buffer_copy(can_frame.data)
            else:
                frame = can_frame

            # ignored = [0x631]
            # vfd = [0x181, 0x281, 0x481, 0x201, 0x301, 0x701, 0x081, 0x663, 0x263, 0x80]
            # if frame.frame_id in vfd:
            print(frame, flush=True)

            if frame.can_id in self.handlers:
                self.handlers[frame.can_id](frame)

    def transmitter(self):
        while True:
            frame = self.tx_queue.get()
            self.can.data_send(frame.can_id, bytes(frame))

    def register_periodic(self, func, period):
        self.periodics.append({'func': func, 'period': period, 'last': 0})

    def register_handler(self, can_id, func):
        self.handlers[can_id] = func

    def send_bms_status(self):
        bms_sync = BMS_Sync_EGV()
        bms_sync.voltage = 75 * 100
        bms_sync.current = 0
        bms_sync.temperature = 25
        bms_sync.soc = 80
        bms_sync.soh = 80
        bms_sync.status = self.bms_state  # TODO: change state some time after start
        bms_sync.emergency = 0
        bms_sync.regen = 1

        self.send(bms_sync)

    def send_bms_imax(self):
        bms_imax = BMS_Imax_EGV()
        bms_imax.discharge = -300
        bms_imax.charge = 100
        self.send(bms_imax)

    def cmd_bms(self, frame):
        bms_cmd = EGV_Cmd_BMS.from_buffer_copy(frame.data)
        target_state = bms_cmd.state
        print("bms cmd", target_state)
        if target_state == 0:
            self.bms_state = BMS_STATE.RUN
        elif target_state == 3:
            self.bms_state = BMS_STATE.READY
        elif target_state == 4:
            self.bms_state = BMS_STATE.STOP
        elif target_state == 1:
            self.bms_state = BMS_STATE.CHARGER
        else:
            raise

        ack = EGV_Ack_BMS(0x630)
        self.send(ack)

    def send_default_bms(self):
        defi = BMS_Default_EGV()
        self.send(defi)

    def send(self, frame: CanFrame):
        self.tx_queue.put(frame)


if __name__ == "__main__":
    can = CAN('/dev/ttyUSB0')
    sch = Scheduler(can)
    sch.run()
