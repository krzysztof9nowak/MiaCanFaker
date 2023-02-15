import can
import canopen
from waveshare_bus import *
import time


with WaveshareSerialBus('/dev/ttyUSB0') as bus:
    network = canopen.Network(bus)
    network.notifier = can.Notifier(network.bus, network.listeners, 1) # bodge, normally it's called when calling network.connect(), but is skiped when constructing Network with bus 

    node = network.add_node(1, 'DCF0925.dcf')
    node.nmt.send_command(0x1)
    node.nmt.wait_for_heartbeat()
    print(node.nmt.state)

    product_code = node.sdo['Identity object']['Product code']
    status_word = node.sdo['statusword']
    vel = node.sdo['Velocity']
    preop = node.sdo[0x2800]

    print(hex(product_code.raw))
    print(hex(status_word.raw))
    print(vel.phys)
    print(preop.raw)

    node.tpdo.read()

    node.tpdo[1].clear()
    node.tpdo[1].add_variable('Velocity')
    node.tpdo[1].trans_type = 254


    node.nmt.state = 'OPERATIONAL'
    node.nmt.wait_for_heartbeat()
    print(node.nmt.state)
    print(node.tpdo[1])


    network.disconnect()

