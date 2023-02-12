//
// Created by nikto on 14.01.2023.
//

#ifndef BMP_H
#define BMP_H

#include "header.h"
#include "image.h"
#include "transformer.h"
#include <stdio.h>



enum read_status from_bmp( FILE* in, struct image* img );

enum write_status to_bmp(FILE* out, struct image const* img);

#endif
