#ifdef __x86_64__

#include <SDL.h>

#include <miagl.h>
#include <miagl-driver.h>
#include <miaui.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void process_event(SDL_Event* event);
void lib_task();
void draw_screen(SDL_Renderer* renderer, int pixelSize);

uint8_t screen[64][128];
uint8_t PALETTE[16][3] = {
    {0, 0, 0},
    {1 * 255 / 15, 1 * 200 / 15, 0},
    {2 * 255 / 15, 2 * 200 / 15, 0},
    {3 * 255 / 15, 3 * 200 / 15, 0},
    {4 * 255 / 15, 4 * 200 / 15, 0},
    {5 * 255 / 15, 5 * 200 / 15, 0},
    {6 * 255 / 15, 6 * 200 / 15, 0},
    {7 * 255 / 15, 7 * 200 / 15, 0},
    {8 * 255 / 15, 8 * 200 / 15, 0},
    {9 * 255 / 15, 9 * 200 / 15, 0},
    {10 * 255 / 15, 10 * 200 / 15, 0},
    {11 * 255 / 15, 11 * 200 / 15, 0},
    {12 * 255 / 15, 12 * 200 / 15, 0},
    {13 * 255 / 15, 13 * 200 / 15, 0},
    {14 * 255 / 15, 14 * 200 / 15, 0},
    {15 * 255 / 15, 15 * 200 / 15, 0},
};

void driver_init_driver()
{
    printf("Init driver has been called!\n");
}

bool driver_is_buffer_ready()
{
    return true;
}

void driver_flush_full_screen(void* buffer, uint16_t size)
{
    if (size != sizeof(screen)) {
        fprintf(stderr, 
            "Error: invalid buffer size passed to flush_full_screen. Got %d, expected %d!\n", 
            (int)size, (int)sizeof(screen));
    }

    uint8_t* buf = buffer;
    memcpy(screen, buf, size);
}

void driver_flush_part_screen(void* buffer, uint16_t size, 
                             uint8_t start_x, uint8_t end_x, uint8_t start_y,
                             uint8_t end_y)
{
    // printf("flush_part_screen: start_x = %d end_x = %d start_y = %d end_y = %d\n", (int)start_x, (int)end_x, (int)start_y, (int)end_y);
    uint8_t* buf = buffer;
    int expectedSize = (end_x - start_x + 1) * (end_y - start_y + 1) / 2;
    if (size != expectedSize) {
        fprintf(stderr, 
            "Error: invalid buffer size passed to flush_part_screen. Got %d, expected %d!\n", 
            (int)size, expectedSize);
    }

    int index = 0;
    for (int y = start_y; y <= end_y; y++) {
        for (int x = start_x; x <= end_x; x += 2) {
            screen[y][x / 2] = buf[index++];
        }
    }
}

#define DISPLAY_WIDTH 256
#define DISPLAY_HEIGHT 64

DEFINE_MIAGL_STRUCT(DISPLAY_WIDTH, DISPLAY_HEIGHT);

miagl_t gl;
miagl_driver_t driver = {
    .fn_init_driver = driver_init_driver,
    .fn_is_buffer_ready = driver_is_buffer_ready,
    .fn_flush_full_screen = driver_flush_full_screen,
    .fn_flush_part_screen = driver_flush_part_screen
};
mui_state_t ui_state = {
    .debug_text = NULL,
    .capacitor_voltage = 800,
    .cell_count = 22,
    .motor_current = 0,
    .gear = MUI_NEUTRAL,
    .vehicle_speed = 0,
    .light_status = 0,
    .battery_level = 72,
    .estimated_range = 0,
    .motor_temp = 60,
    .inverter_temp = 50,
    .odometer = 41853,
    .trip_meter = 0,
};

static uint64_t get_diff(struct timespec t1, struct timespec t2)
{
    return (t2.tv_sec - t1.tv_sec) * 1000000000ULL + (t2.tv_nsec - t1.tv_nsec);
}

int main(int argc, char **argv) 
{
    int scale = 7;
    if (argc == 2) {
        scale = atoi(argv[1]);

        if (scale < 1) scale = 1;
        if (scale > 10) scale = 10;
    }

    const int SCREEN_WIDTH = DISPLAY_WIDTH * scale;
    const int SCREEN_HEIGHT = DISPLAY_HEIGHT * scale;

    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "sdl2 initalization failed: %s", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("x86-miagl-test", 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN);

    if (!window) {
        fprintf(stderr, "sdl2 window creation failed: %s", SDL_GetError());
        return 2;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        fprintf(stderr, "sdl2 renderer creation failed: %s", SDL_GetError());
        return 3;
    }

    printf("Size of the static buffer is %d.\n", (int)sizeof(gl));

    printf("Initializing gfx library...\n");
    mgl_InitLibrary(&gl, DISPLAY_WIDTH, DISPLAY_HEIGHT, &driver);

    printf("Entering main loop...\n");
    bool running = true;
    uint64_t perf[300];
    int perf_idx = 0;

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                break;
            }

            process_event(&event);
        }

        struct timespec time1, time2;
        clock_gettime(CLOCK_THREAD_CPUTIME_ID, &time1);
        lib_task();
        clock_gettime(CLOCK_THREAD_CPUTIME_ID, &time2);
        draw_screen(renderer, scale);

        perf[perf_idx++] = get_diff(time1, time2);

        if (perf_idx == 300) {
            perf_idx = 0;
            uint64_t min = -1ULL;
            uint64_t max = 0;
            uint64_t sum = 0;

            for (size_t i = 0; i < 300; i++) {
                if (perf[i] < min) min = perf[i];
                if (perf[i] > max) max = perf[i];
                sum += perf[i];
            }

            sum /= 300;

            printf("[Performance]: best=%" PRIu64 "ns worst=%" PRIu64 "ns avg=%" PRIu64 "ns.\n", min, max, sum);
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    printf("Exiting...\n");
    return 0;
}

void process_event(SDL_Event* event)
{
    if (event->type == SDL_KEYDOWN) {
        switch(event->key.keysym.sym) {
        case SDLK_q: ++ui_state.capacitor_voltage; break;
        case SDLK_a: --ui_state.capacitor_voltage; break;
        case SDLK_w: ui_state.motor_current += 5; break;
        case SDLK_s: ui_state.motor_current -= 5; break;
        case SDLK_UP: if (ui_state.gear > MUI_DRIVE) --ui_state.gear; break;
        case SDLK_DOWN: if (ui_state.gear < MUI_REVERSE) ++ui_state.gear; break;
        case SDLK_r: ++ui_state.vehicle_speed; break;
        case SDLK_f: --ui_state.vehicle_speed; break;
        case SDLK_LEFT: ui_state.light_status ^= MUI_STATUS_LEFT_BLINKER_ON; break;
        case SDLK_RIGHT: ui_state.light_status ^= MUI_STATUS_RIGHT_BLINKER_ON; break;
        case SDLK_LALT: ui_state.light_status ^= MUI_STATUS_LEFT_DOOR_OPEN; break;
        case SDLK_RALT: ui_state.light_status ^= MUI_STATUS_RIGHT_DOOR_OPEN; break;
        case SDLK_SPACE: ui_state.light_status ^= MUI_STATUS_MOTOR_FAN_RUNNING; break;
        case SDLK_e: ++ui_state.battery_level; break;
        case SDLK_d: --ui_state.battery_level; break;
        case SDLK_t: ++ui_state.estimated_range; break;
        case SDLK_g: --ui_state.estimated_range; break;
        case SDLK_y: ++ui_state.motor_temp; break;
        case SDLK_h: --ui_state.motor_temp; break;
        case SDLK_u: ++ui_state.inverter_temp; break;
        case SDLK_j: --ui_state.inverter_temp; break;
        case SDLK_i: ++ui_state.odometer; break;
        case SDLK_k: --ui_state.odometer; break;
        case SDLK_o: ++ui_state.trip_meter; break;
        case SDLK_l: --ui_state.trip_meter; break;
        }
    }
}

void lib_task()
{
    static uint32_t ticks = 0;
    uint32_t current_ticks = SDL_GetTicks();

    mui_Update(&ui_state, current_ticks - ticks);
    mui_Draw(&ui_state, &gl);
    mgl_FlushScreen(&gl);

    ticks = current_ticks;
}

void draw_screen(SDL_Renderer* renderer, int pixelSize)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
    for (int y = 0; y < 64; y++) {
        for (int x = 0; x < 256; x++) {
            SDL_Rect rect = { x * pixelSize, y * pixelSize, pixelSize, pixelSize };
            uint8_t intensity = (x % 2 == 1) ? ((screen[y][x / 2] & 0x0F)) : ((screen[y][x / 2] & 0xF0) >> 4);
            if (intensity == 0) continue;

            SDL_SetRenderDrawColor(renderer, 
                PALETTE[intensity][0], PALETTE[intensity][1], PALETTE[intensity][2], SDL_ALPHA_OPAQUE);
            SDL_RenderFillRect(renderer, &rect);
        }
    }
    SDL_RenderPresent(renderer);
}

#endif
