#pragma once

int get_char_code(unsigned char c); 
int encode_char(unsigned char **data, int char_code, long x, long y, unsigned char color);
unsigned char get_char(int code);
int decode_char(unsigned char **data, long x, long y, unsigned char color);
