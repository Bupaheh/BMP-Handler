#pragma once
#include <stdio.h>
#include <stdint.h>

struct Info {
    uint32_t size;
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t bit_count;
    uint32_t compression;
    uint32_t size_image;
    uint32_t x_px_per_meter;
    uint32_t y_px_per_meter;
    uint32_t clr_used;
    uint32_t clr_important;
} __attribute__((packed));

struct Header {
    uint16_t type;
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t off_bits;
} __attribute__((packed));

static const int info_size = 40;
static const int header_size = 14;

unsigned char **load_bmp(struct Header *header, struct Info *info, FILE *file);
unsigned char **crop(unsigned char **data, int y, int x, long h, long w);
unsigned char **rotate(unsigned char **data, long h, long w);
long update_sizes(long *h, long *w);
void update_header(struct Header *header, long h, long width_size);
void update_info(struct Info *info, long h, long w, long width_size);
void save_bmp(FILE *file, struct Header *header, struct Info *info, unsigned char **data, long h, long width_size);
void free_matrix(unsigned char **data);
