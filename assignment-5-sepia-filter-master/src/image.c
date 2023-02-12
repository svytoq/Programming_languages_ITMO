//
// Created by nikto on 14.01.2023.
//
#include "../include/image.h"
#include <stdlib.h>

struct image* create_image(uint64_t sourceWidth, uint64_t sourceHeight) {
    struct image *result = malloc(sizeof(struct image));
    result->data = malloc(sourceWidth * sourceHeight * sizeof(struct pixel));
    result->width = sourceWidth;
    result->height = sourceHeight;
    return result;

}

void image_free(struct image *image) {
    if (image != NULL) {
        free(image->data);
    }
    free(image);
}
