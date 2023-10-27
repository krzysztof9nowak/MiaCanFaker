import json
from os import path
from PIL import Image

FONTS_DIR = path.realpath(path.dirname(__file__) + '/../fonts')

if __name__ == '__main__':
    output = [{ 'font_height': 8 }]

    FIRST_LINE = 51
    FIRST_COLUMN = 76
    LINE_HEIGHT = 13
    COLUMN_WIDTH = 12
    LINES = [ 0, 16, 32, 48, 64, 80, 96, 112, 160, 176, 192, 208, 224, 240 ]
    ZERO_RIGHT = [ 17, 18, ord('a'), ord('r') ]

    img = Image.open(FONTS_DIR + '/helvetica_bold8.png')
    for i, line in enumerate(LINES):
        current_y = FIRST_LINE + i * LINE_HEIGHT
        for column in range(16):
            current_x = FIRST_COLUMN + column * COLUMN_WIDTH
            bbox_minx = COLUMN_WIDTH
            bbox_miny = LINE_HEIGHT
            bbox_maxx = 0
            bbox_maxy = 0
            
            for y in range(LINE_HEIGHT):
                for x in range(COLUMN_WIDTH):
                    color = img.getpixel((x + current_x, y + current_y))
                    if color == (255, 255, 255, 255):
                        bbox_minx = min(bbox_minx, x)
                        bbox_miny = min(bbox_miny, y)
                        bbox_maxx = max(bbox_maxx, x)
                        bbox_maxy = max(bbox_maxy, y)
                    
            if bbox_maxx >= bbox_minx and bbox_maxy >= bbox_miny:
                output.append({
                    'glyph': chr(line + column),
                    'x': bbox_minx + current_x,
                    'y': bbox_miny + current_y,
                    'w': bbox_maxx - bbox_minx + 1,
                    'h': bbox_maxy - bbox_miny + 1,
                    'left': bbox_minx - 1,
                    'right': 0 if (line + column) in ZERO_RIGHT else 1,
                    'baseline': 10 - bbox_miny
                })

    # Add space
    output.append({
        'glyph': ' ',
        'x': 77,
        'y': 62,
        'w': 1,
        'h': 1,
        'left': 0,
        'right': 2,
        'baseline': 0,
    })
    
    with open(FONTS_DIR + '/helvetica_bold8.json', 'w') as out_json:
        json.dump(output, out_json, indent=4)
    