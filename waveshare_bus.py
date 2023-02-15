"""
A text based interface. For example use over serial ports like
"/dev/ttyS1" or "/dev/ttyUSB0" on Linux machines or "COM1" on Windows.
The interface is a simple implementation that has been used for
recording CAN traces.

See the interface documentation for the format being used.
"""

import io
import logging
import struct
from typing import Any, List, Tuple, Optional
import time

from can import BusABC, Message
from can import (
    CanInterfaceNotImplementedError,
    CanInitializationError,
    CanOperationError,
    CanTimeoutError,
)
from can.typechecking import AutoDetectedConfig

logger = logging.getLogger("can.serial")

try:
    import serial
except ImportError:
    logger.warning(
        "You won't be able to use the serial can backend without "
        "the serial module installed!"
    )
    serial = None

try:
    from serial.tools.list_ports import comports as list_comports
except ImportError:
    # If unavailable on some platform, just return nothing
    def list_comports() -> List[Any]:
        return []


from enum import IntEnum


class CAN_SPEED(IntEnum):
    SPEED_1000000 = 0x01
    SPEED_500000 = 0x03


class CAN_MODE(IntEnum):
    NORMAL = 0x00


class CAN_FRAME(IntEnum):
    STANDARD = 0x01
    EXTENDED = 0x02


class WaveshareSerialBus(BusABC):
    """
    Enable basic can communication over a serial device.

    .. note:: See :meth:`~_recv_internal` for some special semantics.

    """

    def __init__(
        self,
        channel: str,
        serial_baudrate: int = 2000000,
        timeout: float = 0.1,
        rtscts: bool = False,
        *args,
        **kwargs,
    ) -> None:
        """
        :param channel:
            The serial device to open. For example "/dev/ttyS1" or
            "/dev/ttyUSB0" on Linux or "COM1" on Windows systems.

        :param baudrate:
            Baud rate of the serial device in bit/s (default 2000000).

            .. warning::
                Some serial port implementations don't care about the baudrate.

        :param timeout:
            Timeout for the serial device in seconds (default 0.1).

        :param rtscts:
            turn hardware handshake (RTS/CTS) on and off

        :raises ~can.exceptions.CanInitializationError:
            If the given parameters are invalid.
        :raises ~can.exceptions.CanInterfaceNotImplementedError:
            If the serial module is not installed.
        """

        if not serial:
            raise CanInterfaceNotImplementedError("the serial module is not installed")

        if not channel:
            raise TypeError("Must specify a serial port.")

        self.channel_info = f"Serial interface: {channel}"

        try:
            self._ser = serial.serial_for_url(
                channel, baudrate=serial_baudrate, timeout=timeout, rtscts=rtscts
            )
        except ValueError as error:
            raise CanInitializationError(
                "could not create the serial device"
            ) from error

        self.configure(CAN_SPEED.SPEED_500000, CAN_MODE.NORMAL, CAN_FRAME.STANDARD)

        super().__init__(channel, *args, **kwargs)

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
        self._ser.write(bytes(cmd))


    def shutdown(self) -> None:
        """
        Close the serial interface.
        """
        super().shutdown()
        self._ser.close()

    def send(self, msg: Message, timeout: Optional[float] = None) -> None:
        """
        Send a message over the serial device.

        :param msg:
            Message to send.

            .. note:: Flags like ``extended_id``, ``is_remote_frame`` and
                      ``is_error_frame`` will be ignored.

            .. note:: If the timestamp is a float value it will be converted
                      to an integer.

        :param timeout:
            This parameter will be ignored. The timeout value of the channel is
            used instead.

        """

        if msg.arbitration_id > 0xffff:
            raise ValueError("Arbitration ID is out of range")

        if msg.dlc > 8:
            raise ValueError("Data too long")

        frame = [
            0xaa,
            0xC0 | msg.dlc,
            msg.arbitration_id & 0xff,
            msg.arbitration_id >> 8,
        ]

        frame += msg.data
        frame.append(0x55)

        # Write to serial device
        try:
            self._ser.write(bytes(frame))
        except serial.PortNotOpenError as error:
            raise CanOperationError("writing to closed port") from error
        except serial.SerialTimeoutException as error:
            raise CanTimeoutError() from error


    def _recv_internal(
        self, timeout: Optional[float]
    ) -> Tuple[Optional[Message], bool]:
        """
        Read a message from the serial device.

        :param timeout:

            .. warning::
                This parameter will be ignored. The timeout value of the channel is used.

        :returns:
            Received message and :obj:`False` (because no filtering as taken place).

            .. warning::
                Flags like ``is_extended_id``, ``is_remote_frame`` and ``is_error_frame``
                will not be set over this function, the flags in the return
                message are the default values.
        """
        try:
            rx_byte = self._ser.read()
            if rx_byte and ord(rx_byte) == 0xAA:
                rx_byte = self._ser.read()

                if ord(rx_byte) == 0x55:
                    raise ValueError("Received command frame")
                if (ord(rx_byte) >> 4) == 0x0c:
                    dlc = ord(rx_byte) & 0x0f

                    s = self._ser.read(2)

                    arbitration_id = s[0] + 256 * s[1]

                    data = self._ser.read(dlc)

                    # received message data okay
                    msg = Message(
                        timestamp=time.time(),
                        arbitration_id=arbitration_id,
                        dlc=dlc,
                        data=data,
                    )
                    return msg, False


                else:
                    raise CanOperationError("Received unknown frame type")

            else:
                return None, False

        except serial.SerialException as error:
            raise CanOperationError("could not read from serial") from error


    def fileno(self) -> int:
        try:
            return self._ser.fileno()
        except io.UnsupportedOperation:
            raise NotImplementedError(
                "fileno is not implemented using current CAN bus on this platform"
            )
        except Exception as exception:
            raise CanOperationError("Cannot fetch fileno") from exception

    @staticmethod
    def _detect_available_configs() -> List[AutoDetectedConfig]:
        return [
            {"interface": "serial", "channel": port.device} for port in list_comports()
        ]