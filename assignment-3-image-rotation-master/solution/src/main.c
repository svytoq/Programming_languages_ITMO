#include "bmp.h"
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char **argv) {
    if (argc < 3) {
       fprintf(stderr, "Неверное количество аргументов!\n");
    }

    FILE *in = fopen(argv[1], "rb");
    struct image *img = malloc(sizeof(struct image));
    enum read_status read_status = from_bmp(in, img);
    fclose(in);

    if (read_status == READ_ERROR) {
        fprintf(stderr, "Ошибка чтения\n");
        image_free(img);
    }

    struct image *rotated_img = rotate(img);
    image_free(img);
    FILE *out = fopen(argv[2], "wb");
    enum write_status write_status = to_bmp(out, rotated_img);
    fclose(out);
    image_free(rotated_img);

    if (write_status == WRITE_ERROR) {
        fprintf(stderr, "Ошибка записи\n");

    }

    printf("Успешно!\n");
    return 0;
}
