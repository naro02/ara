#ifndef _AUTO_CORRELATION3D_H_
#define _AUTO_CORRELATION3D_H_

#include <stdint.h>
#include <string.h>

#include <riscv_vector.h>

void auto_correlation_3d_v64b(int64_t *c, int64_t *a, int64_t avl_rows, int64_t avl_cols, int64_t avl_depth);

void auto_correlation_3d_s64b(int64_t *c, int64_t *a, int64_t avl_rows, int64_t avl_cols, int64_t avl_depth);

#endif
