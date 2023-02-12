//
// Created by nikto on 14.01.2023.
//
#include "transformer.h"
#include <stdio.h>

struct image* rotate(struct image const* source) {
    uint64_t width = source -> width;
    uint64_t height = source -> height;
    uint64_t x;
    uint64_t y;

    struct image* result_img = create_image(height, width);
    if(result_img == NULL) return NULL;

    for (size_t i = 0; i <height; i++) {
        for (size_t j = 0; j < width; j++) {
            x=(height - i - 1) + j * height;
            y=i * width + j;
            result_img -> data[x] = source -> data[y];
        }
    }

    return result_img;

}
