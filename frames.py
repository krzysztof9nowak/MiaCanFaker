import ctypes
import sys
import inspect

c_uint8 = ctypes.c_uint8
c_int8 = ctypes.c_int8
c_uint16 = ctypes.c_uint16
c_int16 = ctypes.c_int16
struct = ctypes.LittleEndianStructure


class CanFrame(ctypes.LittleEndianStructure):
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


class BMS_Sync_EGV(CanFrame):
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


class EGV_Ack_BMS(CanFrame):
    can_id = 0x631
    _fields_ = [
        ("can_id", c_uint16)
    ]


class EGV_Cmd_BMS(CanFrame):
    can_id = 0x630
    _fields_ = [
        ("_", c_uint8, 1),
        ("state", c_uint8, 3)
    ]


class BMS_Imax_EGV(CanFrame):
    can_id = 0x623
    _fields_ = [
        ("discharge", c_int16),
        ("charge", c_uint8),
    ]


class BMS_Default_EGV(CanFrame):
    can_id = 0x621
    _fields_ = [
        ("delta", c_uint8, 1),
        ("_", c_uint8, 7),
        ("others", c_uint8)
    ]


def find_all_frame_classes():
    frames = {}
    for name, obj in inspect.getmembers(sys.modules[__name__]):
        if inspect.isclass(obj) and issubclass(obj, CanFrame) and obj != CanFrame:
            if not hasattr(obj, 'can_id'):
                raise AttributeError(f'{obj.__name__} has no can_id')
            frames[obj.can_id] = obj

    return frames


mia_frames = find_all_frame_classes()

if __name__ == '__main__':
    imax = BMS_Imax_EGV()
    imax.discharge = 500
    print(imax)
    print(mia_frames)
