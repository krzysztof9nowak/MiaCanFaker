import can
import canopen
from waveshare_bus import *
import pygame



pygame.init()
screen = pygame.display.set_mode((720, 480))
clock = pygame.time.Clock()

BLACK = (0, 0, 0)
WHITE = (255, 255, 255)

bus = WaveshareSerialBus('/dev/ttyUSB0')
network = canopen.Network(bus)
network.notifier = can.Notifier(network.bus, network.listeners, 1) # bodge, normally it's called when calling network.connect(), but is skiped when constructing Network with bus 

network.send_periodic(0x80, b'\xff', 0.1)

node = network.add_node(1, 'DCF0925.dcf')
node.nmt.wait_for_heartbeat()
node.nmt.send_command(0x1)
node.nmt.wait_for_heartbeat()

print(node.nmt.state)

# product_code = node.sdo['Identity object']['Product code']
# while product_code.raw == 0:
#     pass

# print(hex(product_code.raw))

trying_pdo = True
while trying_pdo:
    try:
        print("Trying PDO")
        node.tpdo.read()
        node.rpdo.read()
        trying_pdo = False
    except:
        time.sleep(0.1)
        pass

EGV_Cmd_VAR = node.rpdo[2]
EGV_Cmd_VAR[0].phys = 200
EGV_Cmd_VAR[1].phys = -20
EGV_Cmd_VAR[2].phys = 1000
EGV_Cmd_VAR[3].raw = 0

EGV_Accel_VAR = node.rpdo[1]
EGV_Accel_VAR[0].raw = 0
EGV_Accel_VAR[1].raw = 0
EGV_Accel_VAR[2].raw = 0 # fwd
EGV_Accel_VAR[3].raw = 0 # rev
EGV_Accel_VAR[4].raw = 0 # brake
EGV_Accel_VAR[5].raw = 0 # ds1
EGV_Accel_VAR[6].raw = 0 # fs1
EGV_Accel_VAR[7].raw = 0 # ds2

EGV_Cmd_VAR.start(0.1)
EGV_Accel_VAR.start(0.1)

node.nmt.state = 'OPERATIONAL'
node.nmt.wait_for_heartbeat()
print(node.nmt.state)

run = True
thr = 0
while run:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            run = False
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_d:
                print('drive')
                EGV_Accel_VAR[2].raw = 1 # fwd
                EGV_Accel_VAR[3].raw = 0 # rev
                EGV_Accel_VAR[6].raw = 1 # fs1
            elif event.key == pygame.K_n:
                print('neutral')
                EGV_Accel_VAR[2].raw = 0 # fwd
                EGV_Accel_VAR[3].raw = 0 # rev
                EGV_Accel_VAR[6].raw = 0 # fs1
            elif event.key == pygame.K_r:
                print('reverse')
                EGV_Accel_VAR[2].raw = 0 # fwd
                EGV_Accel_VAR[3].raw = 1 # rev
                EGV_Accel_VAR[6].raw = 1 # fs1
            elif event.key == pygame.K_0:
                thr = 0
            elif event.key == pygame.K_1:
                thr = 90
            elif event.key == pygame.K_2:
                thr = 150
            elif event.key == pygame.K_3:
                thr = 250

    status = node.tpdo[1][2].raw
    print(f"status {hex(status)}")

    if status == 0x440:
        print('err')
        fault_count = node.sdo[0x5300][1]
        fault_id = node.sdo[0x5300][2]
        fault_text = node.sdo[0x5300][3]

        fault_id_str = node.sdo[0x5610][1]
        fault_str = node.sdo[0x5610][2]

        cnt = fault_count.raw
        print(f'cnt {cnt}')
        for i in range(cnt):
            fault_id.raw = i
            fault_id_str = i
            print(f'{i} {hex(fault_text.raw)} {fault_str.data}')

    keys=pygame.key.get_pressed()
    if keys[pygame.K_UP]:
        EGV_Accel_VAR[0].raw = thr
    else:
        EGV_Accel_VAR[0].raw = 0

    clock.tick(60)

network.disconnect()
bus.close()
quit()

