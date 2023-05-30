#include <miaui-test.h>

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

    mgl_FlushScreen(gl);
}
