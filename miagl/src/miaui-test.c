#include <miaui-test.h>

#include <miagl-gfx.h>

#include <stdlib.h>

void mui_RenderGlTest(miagl_ptr gl)
{
    // Test direct access to pixels
    for (int y = 0; y < 16; y++) {
        for (int x = 0; x < 16; x++) {
            mgl_SetPixel(gl, x, y, rand());
        }
    }

    for (int i = 0; i < 16; i++) {
        mgl_SetPixel(gl, i, i + 17, 8);
        mgl_SetPixel(gl, 15 - i, i + 17, 8);
    }

    for (int i = 0; i < 16; i++) {
        mgl_SetPixel(gl, i, i + 16, MIAGL_COLOR_WHITE);
        mgl_SetPixel(gl, 15 - i, i + 16, MIAGL_COLOR_WHITE);
    }

    mgl_SetColor(gl, 10);
    mgl_DrawHLine(gl, 0, 0, 36);
    mgl_DrawHLine(gl, 1, 4, 38);
    mgl_DrawHLine(gl, 0, 15, 40);
    mgl_DrawHLine(gl, 0, 16, 42);
    mgl_DrawHLine(gl, 3, 16, 44);
    mgl_DrawHLine(gl, 7, 16, 46);
    mgl_DrawHLine(gl, 8, 22, 48);
    mgl_DrawHLine(gl, 8, 24, 50);
    mgl_DrawHLine(gl, 8, 30, 52);

    mgl_SetColor(gl, 5);
    mgl_DrawVLine(gl, 0, 55, 63);
    mgl_DrawVLine(gl, 4, 58, 60);
    mgl_DrawVLine(gl, 8, 59, 80);

    mgl_SetColor(gl, 8);
    mgl_StrokeRect(gl, 20, 5, 30, 20);
    mgl_SetColor(gl, 4);
    mgl_StrokeRect(gl, 28, 7, 22, 18);

    mgl_SetColor(gl, 12);
    mgl_FillRect(gl, 20, 25, 30, 40);
    mgl_SetColor(gl, 6);
    mgl_FillRect(gl, 22, 27, 28, 38);

    mgl_SetColor(gl, MIAGL_COLOR_WHITE);
    for (int i = 0; i < 16; i++) {
        mgl_DrawLine(gl, 32, i * 3, 42, i * 4);
    }

    mgl_SetColor(gl, 3);
    mgl_DrawLineAA(gl, 80, 30, 80, 0);
    mgl_DrawLineAA(gl, 80, 30, 90, 0);
    mgl_SetColor(gl, 4);
    mgl_DrawLineAA(gl, 80, 30, 100, 0);
    mgl_DrawLineAA(gl, 80, 30, 110, 0);
    mgl_SetColor(gl, 5);
    mgl_DrawLineAA(gl, 80, 30, 110, 10);
    mgl_DrawLineAA(gl, 80, 30, 110, 20);
    mgl_SetColor(gl, 6);
    mgl_DrawLineAA(gl, 80, 30, 110, 30);
    mgl_DrawLineAA(gl, 80, 30, 110, 40);
    mgl_SetColor(gl, 7);
    mgl_DrawLineAA(gl, 80, 30, 110, 50);
    mgl_DrawLineAA(gl, 80, 30, 110, 60);
    mgl_SetColor(gl, 8);
    mgl_DrawLineAA(gl, 80, 30, 100, 60);
    mgl_DrawLineAA(gl, 80, 30, 90, 60);
    mgl_SetColor(gl, 9);
    mgl_DrawLineAA(gl, 80, 30, 80, 60);
    mgl_DrawLineAA(gl, 80, 30, 70, 60);
    mgl_SetColor(gl, 10);
    mgl_DrawLineAA(gl, 80, 30, 60, 60);
    mgl_DrawLineAA(gl, 80, 30, 50, 60);
    mgl_SetColor(gl, 11);
    mgl_DrawLineAA(gl, 80, 30, 50, 50);
    mgl_DrawLineAA(gl, 80, 30, 50, 40);
    mgl_SetColor(gl, 12);
    mgl_DrawLineAA(gl, 80, 30, 50, 30);
    mgl_DrawLineAA(gl, 80, 30, 50, 20);
    mgl_SetColor(gl, 13);
    mgl_DrawLineAA(gl, 80, 30, 50, 10);
    mgl_DrawLineAA(gl, 80, 30, 50, 0);
    mgl_SetColor(gl, 14);
    mgl_DrawLineAA(gl, 80, 30, 60, 0);
    mgl_SetColor(gl, 15);
    mgl_DrawLineAA(gl, 80, 30, 70, 0);

    mgl_FlushScreen(gl);
}
