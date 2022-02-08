#include "bmp.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>

static const int pixel_size = 3;

static unsigned char **create_matrix(long height, long width) {
    unsigned char **data = malloc(sizeof(char *) * height);
    assert(data);
    unsigned char *temp = malloc(sizeof(char) * height * width);
    assert(temp);
    for (int i = 0; i < height; i++)
        data[i] = &temp[i * width];
    return data;
}

void free_matrix(unsigned char **data) {
    free(data[0]);
    free(data);
}

unsigned char **load_bmp(struct Header *header, struct Info *info, FILE *file) {
    fread(header, header_size, 1, file);
    fread(info, info_size, 1, file);
    long data_pointer = header->off_bits;
    long data_size = info->size_image;
    long data_width = info->width;
    long data_height = info->height;
    long fill_bytes = data_size / data_height - pixel_size * data_width;
    fseek(file, data_pointer, SEEK_SET);
    unsigned char **data = create_matrix(data_height, pixel_size * data_width);
    for (long i = data_height - 1; i >= 0; i--) {
        fread(data[i], sizeof(char), pixel_size * data_width, file);
        fseek(file, fill_bytes, SEEK_CUR);
    }
    return data;
}

unsigned char **crop(unsigned char **data, int y, int x, long h, long w) {
    unsigned char **cropped_data = create_matrix(h, w * pixel_size);
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++)
            memcpy(&cropped_data[i][j * pixel_size], &data[i + y][(j + x) * pixel_size], pixel_size);
    return cropped_data;
}

unsigned char **rotate(unsigned char **data, long h, long w) {
    long fill_bytes = (4 - ((h * pixel_size) % 4)) % 4;
    unsigned char **rotated_data = create_matrix(w, h * pixel_size + fill_bytes);
    for (long j = 0; j < w; j++)
        for (long i = 0; i < h; i++)
            memcpy(&rotated_data[j][(h - 1 - i) * pixel_size], &data[i][j * pixel_size], pixel_size);
    for (long i = 0; i < w; i++) {
        for (long j = 0; j < fill_bytes; j++)
            rotated_data[i][h * pixel_size + j] = 0;
    }
    return rotated_data;
}

long update_sizes(long *h, long *w) {
    long width_size = *h * pixel_size + (4 - ((*h * pixel_size) % 4)) % 4;
    long t = *w;
    *w = *h;
    *h = t;
    return width_size;
}

void update_header(struct Header *header, long h, long width_size) {
    header->size = h * width_size + header_size + info_size;
    header->off_bits = header_size + info_size;
}

void update_info(struct Info *info, long h, long w, long width_size) {
    info->width = w;
    info->height = h;
    info->size_image = h * width_size;
}

void save_bmp(FILE *file, struct Header *header, struct Info *info, unsigned char **data, long h, long width_size) {
    fwrite(header, header_size, 1, file);
    fwrite(info, info_size, 1, file);
    for (int i = h - 1; i >= 0; i--)
        fwrite(data[i], sizeof(char), width_size, file);
}
