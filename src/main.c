#include "bmp.h"
#include "stego.h"
#include <stdlib.h>
#include <string.h>

int crop_rotate_mode(char **argv) {
    char *in = argv[2];
    char *out = argv[3];
    int x = atoi(argv[4]);
    int y = atoi(argv[5]);
    long w = atoi(argv[6]);
    long h = atoi(argv[7]);
    FILE *file = fopen(in, "r");
    if (file == NULL)
        return 1;
    struct Header header;
    struct Info info;
    unsigned char **data = load_bmp(&header, &info, file);
    fclose(file);
    if (x + w > info.width || y + h > info.height) {
        free_matrix(data);
        return 1;
    }
    unsigned char **cropped_data = crop(data, y, x, h, w);
    free_matrix(data);
    unsigned char **rotated_data = rotate(cropped_data, h, w);
    free_matrix(cropped_data);
    long width_size = update_sizes(&h, &w);
    file = fopen(out, "w");
    update_header(&header, h, width_size);
    update_info(&info, h, w, width_size);
    save_bmp(file, &header, &info, rotated_data, h, width_size);
    fclose(file);
    free_matrix(rotated_data);
    return 0;
}

int insert_mode(char **argv) {
    char *in = argv[2];
    char *out = argv[3];
    FILE *key_file = fopen(argv[4], "r");
    FILE *msg_file = fopen(argv[5], "r");
    FILE *file = fopen(in, "r");
    if (key_file == NULL || msg_file == NULL || file == NULL) {
        fclose(key_file);
        fclose(msg_file);
        fclose(file);
        return 1;
    }
    struct Header header;
    struct Info info;
    unsigned char **data = load_bmp(&header, &info, file);
    fclose(file);
    char c;
    while ((c = fgetc(msg_file)) !=EOF) {
        int code = get_char_code(c);
        if (code == -1)
            continue;
        for (int i = 0; i < 5; i++) {
            int x, y;
            unsigned char color;
            fscanf(key_file, "%d %d %c", &x, &y, &color);
            encode_char(data, code % 2, x, y, color);
            code /= 2;
        }
    }
    FILE *fout = fopen(out, "w");
    header.off_bits = header_size + info_size;
    save_bmp(fout, &header, &info, data, info.height, info.size_image / info.height);
    fclose(fout);
    fclose(key_file);
    fclose(msg_file);
    free_matrix(data);
    return 0;
}

int extract_mode(char **argv) {
    char *in = argv[2];
    FILE *key_file = fopen(argv[3], "r");
    FILE *msg_file = fopen(argv[4], "w");
    FILE *file = fopen(in, "r");
    if (key_file == NULL || file == NULL) {
        fclose(key_file);
        fclose(msg_file);
        fclose(file);
        return 1;
    }
    struct Header header;
    struct Info info;
    unsigned char **data = load_bmp(&header, &info, file);
    fclose(file);
    int x, y;
    unsigned char color;
    int pow = 1, now = 0;
    while (fscanf(key_file, "%d %d %c", &x, &y, &color) != EOF) {
        now += decode_char(data, x, y, color) * pow;
        pow *= 2;
        if (pow == 32) {
            pow = 1;
            fprintf(msg_file, "%c", get_char(now));
            now = 0;
        }
    }
    fprintf(msg_file, "\n");
    free_matrix(data);
    fclose(key_file);
    fclose(msg_file);
    return 0;
}

int main(int argc, char **argv) {
    int return_value;
    if (argc == 8 && strcmp(argv[1], "crop-rotate") == 0)
        return_value = crop_rotate_mode(argv);
    else if (argc == 6 && strcmp(argv[1], "insert") == 0)
        return_value = insert_mode(argv);
    else if (argc == 5 && strcmp(argv[1], "extract") == 0)
        return_value = extract_mode(argv);
    else
        return_value = 1;
    return return_value;
} 
