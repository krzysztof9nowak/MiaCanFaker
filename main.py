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

class MetaScheduler(type):
    @classmethod
    def __prepare__(meta, name, bases):
        d = dict()
        d['periodics'] = []
        d['handlers'] = {}

        def periodic(t):
            def periodic_decorator(func):
                d['periodics'].append({'func': func, 'period': t, 'last': 0})
                return func

            return periodic_decorator

        def handle(cls):
            def handle_decorator(func):
                d['handlers'][cls] = func
                return func

            return handle_decorator

        d['periodic'] = periodic
        d['handle'] = handle
        return d

class Scheduler(metaclass=MetaScheduler):
    def __init__(self, can):
        print(self.periodics)
        self.can = can

        self.tx_queue = queue.Queue()
        self.bms_state = BMS_STATE.READY

        self.rx_thread = threading.Thread(target=self.receiver)
        self.tx_thread = threading.Thread(target=self.transmitter)


    def run(self):
        self.rx_thread.start()
        self.tx_thread.start()
        while True:
            for periodic in self.periodics:
                t = time.time()
                dt = t - periodic['last']
                if dt > periodic['period']:
                    periodic['func'](self)
                    periodic['last'] = t
                time.sleep(0.01)

    def receiver(self):
        while True:
            can_frame = self.can.frame_recv()

            if can_frame.can_id in mia_frames:
                cls = mia_frames[can_frame.can_id]
                try:
                    frame = cls.from_buffer_copy(bytes(can_frame.data))
                except ValueError as e:
                    print(cls, can_frame.data)
                    raise
                except TypeError as e:
                    print(can_frame.data)
                    raise
            else:
                frame = can_frame

            # ignored = [0x631]
            # vfd = [0x181, 0x281, 0x481, 0x201, 0x301, 0x701, 0x081, 0x663, 0x263, 0x80]
            if frame.can_id in [0x630]:
                print(frame, flush=True)

            if frame.__class__ in self.handlers:
                self.handlers[frame.__class__](self, frame)
            elif frame.can_id in self.handlers:
                self.handlers[frame.can_id](self, frame)

    def transmitter(self):
        while True:
            frame = self.tx_queue.get()
            self.can.data_send(frame.can_id, bytes(frame))



    @periodic(0.1)
    def send_bms_status(self):
        bms_sync = BMS_Sync_EGV()
        bms_sync.voltage = 75 * 100
        bms_sync.current = 0
        bms_sync.temperature = 10
        bms_sync.soc = 80
        bms_sync.soh = 80
        bms_sync.status = self.bms_state  # TODO: change state some time after start
        bms_sync.emergency = 0
        bms_sync.regen = 1

        self.send(bms_sync)

    @periodic(0.1)
    def send_bms_imax(self):
        bms_imax = BMS_Imax_EGV()
        bms_imax.discharge = -300
        bms_imax.charge = 100
        self.send(bms_imax)

    @handle(EGV_Cmd_BMS)
    def cmd_bms(self, cmd: EGV_Cmd_BMS):
        print(cmd)
        target_state = cmd.state
        print("bms cmd", target_state)
        if target_state == 0:
            self.bms_state = BMS_STATE.RUN
        elif target_state == 1:
            self.bms_state = BMS_STATE.CHARGER
        elif target_state == 2:
            self.bms_state = BMS_STATE.READY
        elif target_state == 3:
            self.bms_state = BMS_STATE.READY  # should be shutdown
        elif target_state == 4:
            self.bms_state = BMS_STATE.STOP
        else:
            raise

        ack = EGV_Ack_BMS(0x630)
        self.send(ack)
        self.send_bms_status()
        self.send_charger()

    @periodic(0.1)
    def send_default_bms(self):
        defi = BMS_Default_EGV()
        self.send(defi)

    @periodic(1)
    def send_charger(self):
        chg = BMS_Regul_CHA(10 * 10, 90, 0, 0)
        if self.bms_state == BMS_STATE.CHARGER:
            chg.charge = 1
            chg.contactor = 1

        print(chg)
        self.send(chg)


    def send(self, frame: CanFrame):
        self.tx_queue.put(frame)


if __name__ == "__main__":
    can = CAN('/dev/ttyUSB0')
    sch = Scheduler(can)
    sch.run()
