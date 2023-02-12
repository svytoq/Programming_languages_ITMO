//
// Created by nikto on 14.01.2023.
//
#include "bmp.h"

static size_t calculate_padding(const struct image img) {
    return img.width * sizeof(struct pixel) % 4 == 0 ? 0 : 4 - img.width * sizeof(struct pixel) % 4;
}

enum read_status from_bmp(FILE *in, struct image* img) {
    struct header_info bmpHeaderStatus = read_header(in);
    if (bmpHeaderStatus.status != READ_OK) return bmpHeaderStatus.status;

    uint64_t width = img -> width = bmpHeaderStatus.header.biWidth;
    uint64_t height = img -> height = bmpHeaderStatus.header.biHeight;
    struct pixel* data = malloc(width * height * sizeof(struct pixel));

    if(data == NULL) {
        free(data);
        return READ_ERROR;
    }
    
    size_t padding =  calculate_padding(*img);
    for(size_t i = 0; i < height; i++) {
        if(fread(data + i * width, width * sizeof(struct pixel), 1, in) != 1)
            return READ_ERROR;
        if(padding != 0)
            fseek(in,(long) padding, SEEK_CUR);
    }

    img -> data = data;

    return READ_OK;
}

enum write_status to_bmp(FILE* out, struct image const* img) {
    uint64_t width = img -> width;
    uint64_t height = img -> height;

    size_t padding = calculate_padding(*img);

    struct bmp_header header = create_header(width, height);

    if (!fwrite(&header, sizeof (struct bmp_header), 1, out)) return WRITE_ERROR;

    size_t buf = 0;
    size_t img_width = img->width;
    for (uint32_t i = 0; i < img->height; i++){
        size_t width_2 = fwrite(img->data + i * img->width, sizeof(struct pixel),img_width, out);
        if (width_2 != img_width) return WRITE_ERROR;
        if(!fwrite(&buf, padding, 1, out)) return WRITE_ERROR;
    }
    return WRITE_OK;



}
