//
// Created by nikto on 14.01.2023.
//

#ifndef TRANSFORMER_H
#define TRANSFORMER_H
#include "image.h"

struct image* rotate(struct image const* source);
struct image* sepia_c(struct image const* source);
struct image* sepia_asm(struct image const* source);
#endif
