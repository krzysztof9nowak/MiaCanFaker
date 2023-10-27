from itertools import count
import json
from os import listdir, path
from PIL import Image

IMAGES_DIR = path.realpath(path.dirname(__file__) + '/../images')
FONTS_DIR = path.realpath(path.dirname(__file__) + '/../fonts')
INC_DIR = path.realpath(path.dirname(__file__) + '/../inc')
SRC_DIR = path.realpath(path.dirname(__file__) + '/../src')
print(f'Running in {IMAGES_DIR}')

out_inc = open(INC_DIR + '/miaui-resources.h', 'w')
out_src = open(SRC_DIR + '/miaui-resources.c', 'w')

out_inc.write('''#ifndef _MIAUI_RESOURCES_H_
#define _MIAUI_RESOURCES_H_
#include <miagl-font.h>

#include <stddef.h>
#include <stdint.h>

''')
out_inc.write('// DO NOT CHANGE THIS FILE!\n') 
out_inc.write('// RUN scripts/make_resources.py TO REGENERATE IT!\n')

out_src.write('#include <miaui-resources.h>\n\n')
out_src.write('// DO NOT CHANGE THIS FILE!\n') 
out_src.write('// RUN scripts/make_resources.py TO REGENERATE IT!\n\n')

print('====== PROCESSING IMAGES ======')
for file in sorted(listdir(IMAGES_DIR)):
    try:
        print(f'Processing {file}...')
        background = file.lower().startswith('bg_')
        if background:
            var_name = 'BG_' + file[3:file.rfind('.')].upper()
        else:
            var_name = 'IMG_' + file[0:file.rfind('.')].upper()

        out_inc.write(f'extern const uint32_t {var_name}[];\n')

        img = Image.open(IMAGES_DIR + '/' + file).convert('L')
        width = img.width
        height = img.height

        c_array = f'const uint32_t {var_name}[] = '
        c_array_content = [ hex(width * 65536 + height) ]

        if background:
            c_array_content = []

        BG_ORDER = [1, 0, 3, 2, 5, 4, 7, 6]

        for y in range(height):
            word = 0
            counter = 0
            for x in range(width):
                brightness = img.getpixel((x, y))
                truncated = brightness // 16
                if not background:
                    word <<= 4
                    word += truncated
                else:
                    word |= truncated * (1 << BG_ORDER[counter] * 4)
                counter += 1
                if counter == 8:
                    c_array_content.append(hex(word))
                    word = 0
                    counter = 0

            if counter > 0:
                c_array_content.append(hex(word << (32 - counter * 4)))

        c_array_content.append(hex(0)) # add padding
        c_array += str(c_array_content).replace('[', '{').replace(']', '}').replace("'", '')
        c_array += ';\n'

        out_src.write(c_array)
    except Exception as e:
        print(e)

out_inc.write('\n')
out_src.write('\n')

print('====== PROCESSING  FONTS ======')
for file in sorted(listdir(FONTS_DIR)):
    try:
        if not file.lower().endswith('.json'):
            continue

        print(f'Processing {file}...')
        var_name = 'FNT_' + file[:-5].upper()
        glyph_var_name = 'GLYPH_FNT_' + file[:-5].upper() + '_'
        atlas_name = file[:-5] + '.png'
        print(f'-> Atlas name: {atlas_name}')

        glyph_dict = {}
        font_height = 0

        with open(FONTS_DIR + '/' + file, 'r', encoding='utf-8') as json_file:
            img = Image.open(FONTS_DIR + '/' + atlas_name).convert('L')
            data = json.load(json_file)

            font_height = data[0]['font_height']

            for glyph in data[1:]:
                glyph_name = glyph['glyph']
                glyph_obj = {}
                glyph_obj['varname'] = glyph_var_name + str(ord(glyph_name))
                bmp_array_content = [ hex(glyph['w'] * 65536 + glyph['h']) ]

                for y in range(glyph['y'], glyph['y'] + glyph['h']):
                    word = 0
                    counter = 0
                    for x in range(glyph['x'], glyph['x'] + glyph['w']):
                        brightness = img.getpixel((x, y))
                        truncated = brightness // 16
                        word <<= 4
                        word += truncated
                        counter += 1
                        if counter == 8:
                            bmp_array_content.append(hex(word))
                            word = 0
                            counter = 0

                    if counter > 0:
                        bmp_array_content.append(hex(word << (32 - counter * 4)))
                
                bmp_array_content.append(hex(0)) # add padding
                    
                glyph_obj['bmparray'] = bmp_array_content
                glyph_obj['left'] = glyph['left']
                glyph_obj['right'] = glyph['right']
                glyph_obj['baseline'] = glyph['baseline']

                glyph_dict[glyph_name] = glyph_obj

        out_src.write(f'// FONT {file[:-5]} //\n\n')
        for key, value in sorted(glyph_dict.items()):
            current_var_name = value['varname']
            c_array = f'static const uint32_t {current_var_name}[] = '
            c_array += str(value['bmparray']).replace('[', '{').replace(']', '}').replace("'", '')
            c_array += ';\n'
            out_src.write(c_array)
        out_src.write('\n')
        out_src.write(f'const mgl_glyph_t {var_name}[] = ' + '{\n')

        # Meta glyph
        out_src.write('    { ' + f'NULL, 0, 0, {font_height - 1}, {len(glyph_dict)}' + ' },\n')

        for key, value in sorted(glyph_dict.items()):
            current_var_name = value['varname']
            current_left = value['left']
            current_right = value['right']
            current_baseline = value['baseline']
            out_src.write('    { ' + f'{current_var_name}, {current_left}, {current_right}, {current_baseline}, {ord(key)}' + ' },\n')

        out_src.write('};\n\n')
        out_inc.write(f'extern const mgl_glyph_t {var_name}[];\n')

                

    except Exception as e:
        print(e)


out_inc.write('\n#endif\n')
