

#ifndef _AUTO_CORRELATION1D_H_
#define _AUTO_CORRELATION1D_H_

#include <stdint.h>
#include <string.h>

#include <riscv_vector.h>

void auto_correlation_2d_v64b(int64_t *c, int64_t *a, int64_t avl_rows, int64_t avl_cols);

void auto_correlation_2d_s64b(int64_t *c, int64_t *a, int64_t avl_rows, int64_t avl_cols);

#endif