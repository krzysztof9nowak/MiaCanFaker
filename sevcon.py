import can
import canopen
from waveshare_bus import *
import time


with WaveshareSerialBus('/dev/ttyUSB0') as bus:
    network = canopen.Network(bus)
    network.notifier = can.Notifier(network.bus, network.listeners, 1)
    node = network.add_node(0, 'DCF0925.dcf')

    product_code = node.sdo['Identity object']['Product code']
    print(hex(product_code.raw))

    network.disconnect()

