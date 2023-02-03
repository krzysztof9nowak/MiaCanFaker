import serial
from enum import IntEnum


class CAN_SPEED(IntEnum):
    SPEED_1000000 = 0x01
    SPEED_500000 = 0x03


class CAN_MODE(IntEnum):
    NORMAL = 0x00


class CAN_FRAME(IntEnum):
    STANDARD = 0x01
    EXTENDED = 0x02


class DataFrame:
    def __init__(self, frame):
        self.frame = frame
        if len(self.frame) >= 6 and frame[0] == 0xaa and frame[1] >> 4 == 0xc:
            self.can_id = frame[2] + 256 * frame[3]
            self.data = frame[4:len(frame) - 1]
            self.dlc = frame[1] & 0xf

    def __str__(self):
        return 'Frame ID ' + '{:#04x}'.format(self.can_id) + ' data: ' + self.hex_string(self.data) + " DLC: " + str(
            self.dlc)

    def hex_string(self, data):
        s = ""
        for i in data:
            s += hex(i)[2:] + ' '
        return s


class CAN:
    def __init__(self, port):
        tty_baud = 2000000
        self.ser = serial.Serial(port, tty_baud)
        self.frame_type = CAN_FRAME.STANDARD
        self.configure(CAN_SPEED.SPEED_500000, CAN_MODE.NORMAL, self.frame_type)

    def checksum(self, cmd):
        return sum(cmd) % 256

    def configure(self, can_speed, mode, frame_type):
        cmd = [
            0xaa,
            0x55,
            0x12,
            can_speed,
            frame_type,
            0,  # filter id
            0,
            0,
            0,
            0,  # mask id
            0,
            0,
            0,
            mode,
            0x01,
            0,
            0,
            0,
            0
        ]
        cmd.append(self.checksum(cmd[2:]))

        self.frame_send(cmd)

    def data_send(self, can_id, data):
        assert (len(data) <= 8)

        byte1 = 0xC0 | len(data)
        if self.frame_type == CAN_FRAME.STANDARD:
            pass
        elif self.frame_type == CAN_FRAME.EXTENDED:
            byte1 = byte1 | 0x20
        else:
            raise ValueError

        frame = [
            0xaa,
            byte1,
            can_id & 0xff,
            can_id >> 8,
        ]

        frame += data
        frame.append(0x55)

        self.frame_send(frame)

    def frame_send(self, cmd):
        data = bytes(cmd)
        self.ser.write(data)

    def frame_recv(self):
        frame = b''
        while True:
            byte = self.ser.read()
            frame += byte

            if frame[0] != 0xaa:
                print("skip")
                frame = []  # skip
                continue
            elif len(frame) < 2:
                continue
            elif frame[1] == 0x55:
                # command frame
                if len(frame) >= 20:
                    print('Command frame ', frame)
                    raise Exception
                else:
                    continue
            elif (frame[1] >> 4) == 0xc:
                # data frame
                if len(frame) >= (frame[1] & 0xf) + 5:
                    return DataFrame(frame)
                else:
                    continue

            print("kaszana")
            frame = []  # jakaś kaszana przyszła
