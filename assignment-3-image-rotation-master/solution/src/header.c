//
// Created by nikto on 14.01.2023.
//
#include "header.h"

struct bmp_header create_header(uint64_t width, uint64_t height) {
    struct bmp_header header;
    header.bfType = bfTypeDefault;
    header.bfileSize = sizeof(struct bmp_header) + (width * height * sizeof(struct pixel));
    header.bfReserved = Default;
    header.bOffBits = sizeof(struct bmp_header);
    header.biSize = biSizeDefault;
    header.biWidth = width;
    header.biHeight = height;
    header.biPlanes = biPlanesDefault;
    header.biBitCount = biBitCountDefault;
    header.biCompression = Default;
    header.biSizeImage = header.bfileSize - header.bOffBits;
    header.biXPelsPerMeter = Default;
    header.biYPelsPerMeter = Default;
    header.biClrUsed = Default;
    header.biClrImportant = Default;
    return header;

}


struct header_info read_header(FILE *in) {
    enum read_status status = READ_OK;
    struct bmp_header newHeader;

    if (fread(&newHeader, sizeof(struct bmp_header), 1, in) != 1) {
        status = READ_ERROR;
    }

    return (struct header_info) {
            .header = newHeader,
            .status = status
    };
}
