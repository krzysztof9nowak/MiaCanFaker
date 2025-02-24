# Generates gamma correction LUT

gamma = float(input('Enter gamma: '))
gamma_values = list(map(lambda x: round((x / 255) ** (1 / gamma) * 255), range(256)))
gamma_str = str(gamma_values).replace('[','{').replace(']','}')

print(f'static const uint8_t GAMMA_LUT[] = {gamma_str};')

