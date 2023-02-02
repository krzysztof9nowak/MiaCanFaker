
import ctypes

c_uint8 = ctypes.c_uint8
c_int8 = ctypes.c_int8
c_uint16 = ctypes.c_uint16
c_int16 = ctypes.c_int16
struct = ctypes.LittleEndianStructure

class BMS_Sync_EGV(struct):
    _fields_ = [
            ("current", c_int16),
            ("voltage", c_uint16),
            ("temperature", c_int8),
            ("soc", c_uint8),
            ("state",c_uint8,6),
            ("emergency", c_uint8,1),
            ("regen", c_uint8,1),
            ("soh", c_uint8),
        ]

class EGV_Ack_BMS(struct):
    _fields_ = [
            ("can_id", c_uint16)
        ]

class EGV_Cmd_BMS(struct):
    _fields_ = [
            ("_", c_uint8,1),
            ("state", c_uint8, 3)
            ]


class BMS_Imax_EGV(struct):
    _fields_ = [
            ("discharge", c_int16),
            ("charge", c_uint8),
        ]

class BMS_Default_EGV(struct):
    _fields_ = [
            ("delta", c_uint8, 1),
            ("_", c_uint8, 7),
            ("others", c_uint8)
            ]
