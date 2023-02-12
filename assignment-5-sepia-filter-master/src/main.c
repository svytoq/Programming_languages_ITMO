#include "../include/bmp.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>

int main(int argc, char **argv) {
    if (argc < 4) {
       fprintf(stderr, stderr, "Формат использования: <входной файл> <выходной файл> <r|s|S> \n");
       return 1;
    }

    if (argv[3][0] != 'r' && argv[3][0] != 's' && argv[3][0] != 'S') {
        fprintf(stderr, "Некорректный режим.\nr - Вращение\ns - сепия на C\nS - сепия на asm\n");
        return 1;
    }

    FILE *in = fopen(argv[1], "rb");
    struct image *img = malloc(sizeof(struct image));
    enum read_status read_status = from_bmp(in, img);
    fclose(in);

    if (read_status == READ_ERROR) {
        fprintf(stderr, "Ошибка чтения\n");
        image_free(img);
        return 1;
    }

    struct image *result_img;

    struct rusage r = {0};
    getrusage(RUSAGE_SELF, &r);
    const long start = r.ru_utime.tv_usec;

    if (argv[3][0] == 'r') {
        result_img = rotate(img);
    } else if (argv[3][0] == 's') {
        result_img = sepia_c(img);
    } else if (argv[3][0] == 'S') {
        result_img = sepia_asm(img);
    }


    getrusage(RUSAGE_SELF, &r);
    printf("Трансформация заняла %ldμs\n", r.ru_utime.tv_usec - start);

    image_free(img);
    FILE *out = fopen(argv[2], "wb");
    enum write_status write_status = to_bmp(out, result_img);
    fclose(out);
    image_free(result_img);

    if (write_status == WRITE_ERROR) {
        fprintf(stderr, "Ошибка записи\n");
        return 1;
    }

    printf("Успешно!\n");
    return 0;
}
