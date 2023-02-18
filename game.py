
pygame.init()
screen = pygame.display.set_mode((720, 480))  # Notice the tuple! It's not 2 arguments.
clock = pygame.time.Clock()

BLACK = (0, 0, 0)
WHITE = (255, 255, 255)


run = True
while run:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            run = False

    move_ticker = 0
    keys=pygame.key.get_pressed()
    if keys[pygame.K_UP]:
        print("UP")

    clock.tick(60)

quit()
