#include "stego.h"

static const int pixel_size = 3;

int get_char_code(unsigned char c) {
    switch (c) {
        case ' ':
            return 26;
            break;
        case '.':
            return 27;
            break;
        case ',':
            return 28;
            break;
        default:
            if (c >= 'A' && c <= 'Z')
                return c - 'A';
            else
                return -1;
    }
}

unsigned char get_char(int code) {
    switch (code) {
        case 26:
            return ' ';
            break;
        case 27:
            return '.';
            break;
        case 28:
            return ',';
            break;
        default:
            if (code >= 0 && code <= 28)
                return 'A' + code;
            else
                return '\0';
    }
}

static int get_color(unsigned char c) {
    switch (c) {
        case 'B':
            return 0;
            break;
        case 'G':
            return 1;
            break;
        default:
            return 2;
    }
}

int encode_char(unsigned char **data, int char_code, long x, long y, unsigned char color) {
    int color_offset = get_color(color);
    if (color_offset == -1)
        return 1;
    data[y][pixel_size * x + color_offset] = data[y][pixel_size * x + color_offset] / 2 * 2 + char_code;
    return 0;
}

int decode_char(unsigned char **data, long x, long y, unsigned char color) {
    int color_offset = get_color(color);
    if (color_offset == -1)
        return -1;
    return data[y][pixel_size * x + color_offset] % 2;
}
