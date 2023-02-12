//
// Created by nikto on 14.01.2023.
//

#ifndef HEADER_H
#define HEADER_H



#include "image.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define bfTypeDefault 0x4d42
#define biBitCountDefault 24
#define Default 0
#define biPlanesDefault 1
#define biSizeDefault 40


struct __attribute__ ((packed)) bmp_header {
    uint16_t bfType;
    uint32_t bfileSize;
    uint32_t bfReserved;
    uint32_t bOffBits;
    uint32_t biSize;
    uint32_t biWidth;
    uint32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    uint32_t biXPelsPerMeter;
    uint32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
};


enum read_status  {
    READ_OK = 0,
    READ_ERROR,

};

enum write_status  {
    WRITE_OK = 0,
    WRITE_ERROR,
};


struct header_info {
    struct bmp_header header;
    enum read_status status;
};
struct bmp_header create_header(uint64_t width, uint64_t height);
struct header_info read_header(FILE* in);
#endif //HEADER_H
