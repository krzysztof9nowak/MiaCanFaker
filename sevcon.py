import can
import canopen
from waveshare_bus import *
import time


with WaveshareSerialBus('/dev/ttyUSB0') as bus:
    network = canopen.Network(bus)
    network.notifier = can.Notifier(network.bus, network.listeners, 1) # bodge, normally it's called when calling network.connect(), but is skiped when constructing Network with bus 

    network.send_periodic(0x80, b'\xff', 0.05)

    node = network.add_node(1, 'DCF0925.dcf')
    # node.nmt.send_command(0x1)
    node.nmt.wait_for_heartbeat()
    print(node.nmt.state)

    # product_code = node.sdo['Identity object']['Product code']
    # status_word = node.sdo['statusword']
    # vel = node.sdo['Velocity']
    # preop = node.sdo[0x2800]

    # print(hex(product_code.raw))
    # print(hex(status_word.raw))


    node.tpdo.read()
    node.rpdo.read()

    EGV_Cmd_VAR = node.rpdo[2]
    EGV_Accel_VAR = node.rpdo[1]

    EGV_Cmd_VAR.start(0.1)
    EGV_Accel_VAR.start(0.1)





    for var in node.tpdo[1].map:
        print(var.od.name)


    node.nmt.state = 'OPERATIONAL'
    node.nmt.wait_for_heartbeat()
    print(node.nmt.state)

    # while True:
    #     node.tpdo[1].wait_for_reception()
    #     print(f"status {hex(node.tpdo[1][2].raw)}")

    fault_count = node.sdo[0x5300][1]
    fault_id = node.sdo[0x5300][2]
    fault_text = node.sdo[0x5300][3]

    cnt = fault_count.raw
    print(f'cnt {cnt}')
    for i in range(cnt):
        fault_id.raw = i
        print(f'{i} {hex(fault_text.raw)}')


    network.disconnect()

