//
// Created by nikto on 14.01.2023.
//

#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>

struct __attribute__ ((packed)) pixel { uint8_t b, g, r; };
struct image {
    uint64_t width, height;
    struct pixel* data;
};

struct image* create_image(uint64_t sourceWidth, uint64_t sourceHeight);
void image_free(struct image* image);
#endif
