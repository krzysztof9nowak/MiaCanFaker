import ctypes
import sys
import inspect

c_uint8 = ctypes.c_uint8
c_int8 = ctypes.c_int8
c_uint16 = ctypes.c_uint16
c_int16 = ctypes.c_int16


class CanFrameBig(ctypes.BigEndianStructure):
   # _pack_ = 1
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

    def __str__(self):
        s = self.__class__.__name__ + '('
        for field in self._fields_:
            name = field[0]
            value = getattr(self, name)
            s += f'{name}={value}, '
        s += ')'

        return s


class CanFrameLittle(ctypes.LittleEndianStructure):
   # _pack_ = 1
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

    def __str__(self):
        s = self.__class__.__name__ + '('
        for field in self._fields_:
            name = field[0]
            value = getattr(self, name)
            s += f'{name}={value}, '
        s += ')'

        return s


class BMS_Sync_EGV(CanFrameBig):
    can_id = 0x620
    _fields_ = [
        ("current", c_int16),
        ("voltage", c_uint16),
        ("temperature", c_int8),
        ("soc", c_uint8),
        ("state", c_uint8, 6),
        ("emergency", c_uint8, 1),
        ("regen", c_uint8, 1),
        ("soh", c_uint8),
    ]


class EGV_Ack_BMS(CanFrameBig):
    can_id = 0x631
    _fields_ = [
        ("can_id", c_uint16)
    ]


class EGV_Cmd_BMS(CanFrameBig):
    can_id = 0x630
    _fields_ = [
        ("_", c_uint8, 1),
        ("state", c_uint8, 3)
    ]


class BMS_Imax_EGV(CanFrameBig):
    can_id = 0x623
    _fields_ = [
        ("discharge", c_int16),
        ("charge", c_uint8),
    ]


class BMS_Default_EGV(CanFrameBig):
    can_id = 0x621
    _fields_ = [
        ("delta", c_uint8, 1),
        ("_", c_uint8, 7),
        ("others", c_uint8)
    ]

class BMS_Regul_CHA(CanFrameBig):
    can_id = 0x622
    _fields_ = [
        ("current", c_int16),
        ("voltage", c_uint16),
        ("charge", c_uint8, 1),
        ("contactor", c_uint8, 1),
        ("_", c_uint8, 6),
    ]

class EGV_SYNC_ALL(CanFrameBig):
    can_id = 0x080
    _fields_ = [
        ("bms", c_uint8, 1),
        ("var", c_uint8, 1),
        ("abs", c_uint8, 1),
        ("immo", c_uint8, 1),
        ("charger", c_uint8, 1),
        ("bvs", c_uint8, 1),
        ("_", c_uint8, 1),
        ("diag", c_uint8, 1),
    ]

class VAR_Stat1_EGV(CanFrameLittle):
    can_id = 0x181
    _fields_ = [
        ("rpm", ctypes.c_int32),
        ("torque", c_int16),
        ("status", c_uint16)
    ]


class VAR_Stat2_EGV(CanFrameLittle):
    can_id = 0x281
    _fields_ = [
        ("voltage", c_uint16), # 1/16V
        ("motor_temp", c_int16),
        ("controler_temp", c_int8),
        ("change_state", c_uint8),
       # ("cmd_feedback", c_uint8),
    ]

class VAR_Current_EGV(CanFrameLittle):
    can_id = 0x481
    _fields_ = [
        ("controller_current", c_int16), # 1/16A
        ("motor_current", c_int16), # 1A
        ("curr_limit", c_int16), # 1A
        ("regen_limt", c_int16), # 1A
    ]


class VAR_hbeat_EGV(CanFrameLittle):
    can_id = 0x701
    _fields_ = [
            ("_", c_uint8),
        ]


class CHA_status_EGV(CanFrameBig):
    can_id = 0x560
    _fields_ = [
        ("status", c_uint8),
        ("plug", c_uint8,1),
        ("selector", c_uint8,1),
        ("_", c_uint8,6),
        ("temp", c_uint8,1),
        ("oc", c_uint8,1),
        ("voltage", c_uint8,1),
        ("electronic", c_uint8,1),
        ("cycle", c_uint8,1),
        ("setpoint", c_uint8,1),
    ]

class CHA_Ack_BMS(CanFrameBig):
    can_id = 0x561
    _fields_ = [
        ("can_id", c_uint16),
        ("status", c_uint8, 2),
        ("power", c_uint8, 2),
        ("contactor", c_uint8, 1),
    ]


class EGV_Cmd_VAR(CanFrameLittle):
    can_id = 0x301
    _fields_ = [
        ("curr_limit", c_int16),
        ("regen_limit", c_int16),
        ("max_torque", c_uint16),
        ("fan", c_uint16)

    ]

# class BVS_Sync_EGV(CanFrameBig):
#     can_id = 0x590
#     _fields_ = [
#         ("right_closed", c_uint8, 1),
#         ("left_locked", c_uint8, 1),
#         ("right_locked", c_uint8, 1),
#         ("trunk_locked", c_uint8, 1),
#         ("plug_locked", c_uint8, 1),
#         ("preheat", c_uint8, 1),
#         ("preheat_request", c_uint8, 1),
#         ("left_closed", c_uint8, 1),
#         ("odometer", ctypes.c_uint32)
#     ]


class Diag_req_EGV(CanFrameLittle):
    can_id = 0x260
    _fields_ = [
            ("type", c_uint8, 4),
            ("size", c_uint8, 4),
            ("sid", c_uint8),
            ("did", c_uint8),
            ("a0", ctypes.c_uint8),
            ("a1", ctypes.c_uint8),
            ("a2", ctypes.c_uint8),
            ("a3", ctypes.c_uint8),
            ("a4", ctypes.c_uint8),
        ]

    def __str__(self):
        s = self.__class__.__name__ + '('
        for field in self._fields_:
            name = field[0]
            value = getattr(self, name)
            s += f'{name}={hex(value)}, '
        s += ')'

        return s

class Diag_res_EGV(CanFrameLittle):
    can_id = 0x660
    _fields_ = [
            ("type", c_uint8, 4),
            ("size", c_uint8, 4),
            ("sid", c_uint8),
            ("a0", ctypes.c_uint8),
            ("a1", ctypes.c_uint8),
            ("a2", ctypes.c_uint8),
            ("a3", ctypes.c_uint8),
            ("a4", ctypes.c_uint8),
        ]

    def __str__(self):
        s = self.__class__.__name__ + '('
        for field in self._fields_:
            name = field[0]
            value = getattr(self, name)
            s += f'{name}={hex(value)}, '
        s += ')'

        return s





def find_all_frame_classes():
    frames = {}
    for name, obj in inspect.getmembers(sys.modules[__name__]):
        if inspect.isclass(obj) and (issubclass(obj, CanFrameBig) or issubclass(obj, CanFrameLittle)) and (obj != CanFrameLittle and obj != CanFrameBig) :
            if not hasattr(obj, 'can_id'):
                raise AttributeError(f'{obj.__name__} has no can_id')
            frames[obj.can_id] = obj

    return frames


mia_frames = find_all_frame_classes()
