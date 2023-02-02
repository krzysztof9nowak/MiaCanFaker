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
        self.recv_thread = threading.Thread(target=self.recv_func)
        self.periodics = []
        self.register_periodic(self.send_bms_status, 0.1)
        self.register_periodic(self.send_bms_imax, 0.1)
        self.handlers = {}
        self.register_handler(0x630, self.cmd_bms)
        self.tx_queue = queue.Queue()
        self.bms_state = BMS_STATE.INIT

    def run(self):
        self.recv_thread.start()
        while True:
            while not self.tx_queue.empty():
                can_id, frame = self.tx_queue.get()
                self.can.data_send(can_id, bytes(frame))
                #print(f'queue {hex(can_id)}')
            
            for periodic in self.periodics:
                t = time.time()
                dt = t - periodic['last']
                if dt > periodic['period']:
                    periodic['func']()
                    periodic['last'] = t
            time.sleep(0.01)

    def recv_func(self):
        while True:
            frame = self.can.frame_recv()

            if frame.frame_id & 0xf00 == 0x600 and frame.frame_id != 0x631:
                print(frame, flush=True)

            if frame.frame_id in self.handlers:
                self.handlers[frame.frame_id](frame)


    def register_periodic(self, func, period):
        self.periodics.append({'func':func, 'period':period, 'last':0})

    def register_handler(self, can_id, func):
        self.handlers[can_id] = func

    def send_bms_status(self):
        print(f"sending {self.bms_state}")
        bms_sync = BMS_Sync_EGV()
        bms_sync.voltage = 75 * 100
        bms_sync.current = 0
        bms_sync.temperature = 25
        bms_sync.soc = 80
        bms_sync.soh = 80
        bms_sync.status = 0
        bms_sync.emergency = 0
        bms_sync.regen = 1
        bms_sync.soc = int(5 * time.time()) % 70 + 10

        self.tx_queue.put((0x620, bms_sync))

    def send_bms_imax(self):
        print("sending imax")
        bms_imax = BMS_Imax_EGV()
        bms_imax.discharge = -300
        bms_imax.charge = 100
        self.tx_queue.put((0x623, bms_imax))

    def cmd_bms(self, frame):
        bms_cmd = EGV_Cmd_BMS.from_buffer_copy(frame.data)
        target_state = bms_cmd.state
        print("bms cmd", target_state)
        if target_state == 0:
            self.bms_state = BMS_STATE.RUN
        elif target_state == 3:
            self.bms_state = BMS_STATE.READY
        else:
            raise

        ack = EGV_Ack_BMS(0x630)
        self.tx_queue.put((0x625, ack))



if __name__ == "__main__":
    can = CAN('/dev/ttyUSB0')
    sch = Scheduler(can)
    sch.run()


