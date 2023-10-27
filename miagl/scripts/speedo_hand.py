# Generates a lookup table for drawing speedometer hand

import numpy as np

def normalize(v):
    norm = np.linalg.norm(v)
    if norm == 0: 
       return v
    return v / norm

if __name__ == '__main__':
    MAX_SPEED = 120
    OUTER = (83, 1, 88, 44)
    INNER = (96.5, 14, 61, 31)
    SCALE = 2
    LENGTH = 13

    speeds = [0, MAX_SPEED / 4, 3 * MAX_SPEED / 4, MAX_SPEED]
    outer_x = [OUTER[0], OUTER[0], OUTER[0] + OUTER[2], OUTER[0] + OUTER[2]]
    outer_y = [OUTER[1] + OUTER[3], OUTER[1], OUTER[1], OUTER[1] + OUTER[3]]
    inner_x = [INNER[0], INNER[0], INNER[0] + INNER[2], INNER[0] + INNER[2]]
    inner_y = [INNER[1] + INNER[3], INNER[1], INNER[1], INNER[1] + INNER[3]]

    out = []

    for speed in range(MAX_SPEED + 1):
        tip_x = np.interp(speed, speeds, outer_x)
        tip_y = np.interp(speed, speeds, outer_y)
        btm_x = np.interp(speed, speeds, inner_x)
        btm_y = np.interp(speed, speeds, inner_y)

        vec = normalize([tip_x - btm_x, tip_y - btm_y])
        A_x = tip_x - vec[0] * LENGTH - vec[1] * SCALE
        A_y = tip_y - vec[1] * LENGTH + vec[0] * SCALE
        B_x = tip_x - vec[0] * LENGTH + vec[1] * SCALE
        B_y = tip_y - vec[1] * LENGTH - vec[0] * SCALE

        out += [round(A_x), round(A_y), round(B_x), round(B_y), round(tip_x), round(tip_y)]

    out_str = str(out).replace('[','{').replace(']','}')
    print(f'static const uint16_t SPEEDO_HAND_LUT[] = {out_str};')
