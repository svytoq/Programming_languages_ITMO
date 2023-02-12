//
// Created by nikto on 14.01.2023.
//
#include "../include/transformer.h"
#include <stdio.h>
#define min(x, y) (x < y ? x : y)

const float c[3][3] = {
    {0.393, 0.769, 0.189},
    {0.349, 0.686, 0.168},
    {0.272, 0.534, 0.131}
};


static struct pixel sepia_pixel(struct pixel pixel) {
    float sr =  min(c[0][0] * pixel.r + c[0][1] * pixel.g + c[0][2] * pixel.b, 255);
    float sg =  min(c[1][0] * pixel.r + c[1][1] * pixel.g + c[1][2] * pixel.b, 255);
    float sb =  min(c[2][0] * pixel.r + c[2][1] * pixel.g + c[2][2] * pixel.b, 255);

    struct pixel res = {0};
    res.r = sr;
    res.g = sg;
    res.b = sb;

    return res;
}

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

struct image* sepia_c(struct image const* source) {
    struct image* result_img = create_image(source->width, source->height);

    for (size_t i = 0; i < source->height; i++) {
        for (size_t j = 0; j < source->width; j++) {
            size_t x = source->width * i + j;
            result_img->data[x] = sepia_pixel(source->data[x]);
        }
    }

    return result_img;
}

extern void sepia_asm_helper(struct pixel const*, struct pixel*);

struct image* sepia_asm(struct image const* source) {
    struct image* result_img = create_image(source->width, source->height);
    size_t s = source->width * source->height;

    // Каждый раз по четыре пикселей
    for (size_t i = 0; i < s/4; i++) {
        sepia_asm_helper(source->data + i*4, result_img->data + i*4);
    }

    // И остаток обычным путём попиксельно пройдем
    for (size_t i = s - s%4; i < s; i++) {
        result_img->data[i] = sepia_pixel(source->data[i]);
    }

    return result_img;
}