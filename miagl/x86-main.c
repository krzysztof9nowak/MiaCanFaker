#ifdef __x86_64__

#include <SDL.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void process_event(SDL_Event* event);
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

int main(int argc, char **argv) 
{
    int scale = 7;
    if (argc == 2) {
        scale = atoi(argv[1]);

        if (scale < 1) scale = 1;
        if (scale > 8) scale = 8;
    }

    const int SCREEN_WIDTH = 256 * scale;
    const int SCREEN_HEIGHT = 64 * scale;

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

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                break;
            }

            process_event(&event);
        }
        
        draw_screen(renderer, scale);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

void process_event(SDL_Event* event)
{
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
