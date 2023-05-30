from PIL import Image
from os import listdir, path, truncate

IMAGES_DIR = path.realpath(path.dirname(__file__) + '/../images')
INC_DIR = path.realpath(path.dirname(__file__) + '/../inc')
SRC_DIR = path.realpath(path.dirname(__file__) + '/../src')
print(f'Running in {IMAGES_DIR}')

out_inc = open(INC_DIR + '/miaui-resources.h', 'w')
out_src = open(SRC_DIR + '/miaui-resources.c', 'w')

out_inc.write('''#ifndef _MIAUI_RESOURCES_H_
#define _MIAUI_RESOURCES_H_
#include <stdint.h>

''')
out_inc.write('// DO NOT CHANGE THIS FILE!\n') 
out_inc.write('// RUN scripts/make_resources.py TO REGENERATE IT!\n\n')

out_src.write('#include <miaui-resources.h>\n\n')
out_src.write('// DO NOT CHANGE THIS FILE!\n') 
out_src.write('// RUN scripts/make_resources.py TO REGENERATE IT!\n\n')

for file in sorted(listdir(IMAGES_DIR)):
    print(f'Processing {file}...')
    var_name = 'IMG_' + file[0:file.rfind('.')].upper()

    out_inc.write(f'extern const uint32_t {var_name}[];\n')

    img = Image.open(IMAGES_DIR + '/' + file).convert('L')
    width = img.width
    height = img.height

    c_array = f'const uint32_t {var_name}[] = '
    c_array_content = [ hex(width * 65536 + height) ]

    for y in range(height):
        word = 0
        factor = 1
        for x in range(width):
            brightness = img.getpixel((x, y))
            truncated = brightness // 16
            word += truncated * factor
            factor <<= 4
            if factor == (1 << 32):
                c_array_content.append(hex(word))
                word = 0
                factor = 1

        if factor > 1:
            c_array_content.append(hex(word))

    c_array += str(c_array_content).replace('[', '{').replace(']', '}').replace("'", '')
    c_array += ';\n'

    out_src.write(c_array)

out_inc.write('\n#endif\n')
