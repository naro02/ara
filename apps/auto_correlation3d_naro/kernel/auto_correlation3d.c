#include "auto_correlation3d.h"

#ifndef SPIKE
#include "printf.h"
#else
#include <stdio.h>
#endif

// Helper function to print vector elements
void print_vector_elements(vint64m1_t v, size_t vl)
{
    int64_t elements[vl];
    vse64_v_i64m1(elements, v, vl);
    printf("[");
    for (size_t i = 0; i < vl; i++)
    {
        printf("%ld", elements[i]);
        if (i < vl - 1)
            printf(", ");
    }
    printf("]\n");
}

// Scalar version of 3D auto-correlation
void auto_correlation_3d_s64b(int64_t *c, int64_t *a, int64_t rows, int64_t cols, int64_t depth)
{
    int64_t c_rows = 2 * rows - 1;
    int64_t c_cols = 2 * cols - 1;
    int64_t c_depth = 2 * depth - 1;

    // Initialize result matrix c to zero
    memset(c, 0, c_rows * c_cols * c_depth * sizeof(int64_t));

    // Compute the 3D auto-correlation
    for (int64_t m = -rows + 1; m < rows; m++)
    {
        for (int64_t n = -cols + 1; n < cols; n++)
        {
            for (int64_t o = -depth + 1; o < depth; o++)
            {
                int64_t sum = 0;
                for (int64_t i = 0; i < rows; i++)
                {
                    for (int64_t j = 0; j < cols; j++)
                    {
                        for (int64_t k = 0; k < depth; k++)
                        {
                            if ((i + m) >= 0 && (i + m) < rows &&
                                (j + n) >= 0 && (j + n) < cols &&
                                (k + o) >= 0 && (k + o) < depth)
                            {
                                sum += a[i * cols * depth + j * depth + k] * a[(i + m) * cols * depth + (j + n) * depth + (k + o)];
                            }
                        }
                    }
                }
                c[(m + rows - 1) * c_cols * c_depth + (n + cols - 1) * c_depth + (o + depth - 1)] = sum;
            }
        }
    }
}

// Vectorized version of 3D auto-correlation
void auto_correlation_3d_v64b(int64_t *c, int64_t *a, int64_t rows, int64_t cols, int64_t depth)
{
    int64_t c_rows = 2 * rows - 1;
    int64_t c_cols = 2 * cols - 1;
    int64_t c_depth = 2 * depth - 1;
    size_t vl;

    // Initialize result matrix to zero
    memset(c, 0, c_rows * c_cols * c_depth * sizeof(int64_t));

    // Compute the 3D auto-correlation for non-negative lags
    for (int64_t m = -rows + 1; m < rows; m++)
    {
        for (int64_t n = -cols + 1; n < cols; n++)
        {
            for (int64_t o = 0; o < depth; o++)
            {
                int64_t sum = 0;
                for (int64_t i = 0; i < rows; i++)
                {
                    for (int64_t j = 0; j < cols; j++)
                    {
                        for (int64_t k = 0; k < depth; k += vl)
                        {
                            if ((i + m) < 0 || (i + m) >= rows)
                                continue;
                            if ((j + n) < 0 || (j + n) >= cols)
                                continue;
                            if ((k + o) < 0 || (k + o) >= depth)
                                break;

                            // Set the vector configuration
                            vl = vsetvl_e64m1(depth - (k + o));

                            // Load elements to vector registers
                            vint64m1_t va = vle64_v_i64m1(&a[i * cols * depth + j * depth + k], vl);
                            vint64m1_t vb = vle64_v_i64m1(&a[(i + m) * cols * depth + (j + n) * depth + (k + o)], vl);

                            // Dot product of va, vb
                            vint64m1_t vmul;
                            asm volatile("vmul.vv %0, %1, %2" : "=vr"(vmul) : "vr"(va), "vr"(vb));
                            vint64m1_t vzero = vmv_v_x_i64m1(0, vl);
                            vint64m1_t vsum;
                            // Adding elements of vmul
                            asm volatile("vredsum.vs %0, %1, %2" : "=vr"(vsum) : "vr"(vmul), "vr"(vzero));
                            int64_t partial_sum;
                            asm volatile("vmv.x.s %0, %1" : "=r"(partial_sum) : "vr"(vsum));
                            sum += partial_sum;
                        }
                    }
                }
                c[(m + rows - 1) * c_cols * c_depth + (n + cols - 1) * c_depth + (o + depth - 1)] = sum;
            }
        }
    }

    // Use symmetry to fill in the values for negative depth lags
    for (int64_t m = -rows + 1; m < rows; m++)
    {
        for (int64_t n = -cols + 1; n < cols; n++)
        {
            for (int64_t o = -depth + 1; o < 0; o++)
            {
                c[(m + rows - 1) * c_cols * c_depth + (n + cols - 1) * c_depth + (o + depth - 1)] =
                    c[(rows - m - 1) * c_cols * c_depth + (cols - n - 1) * c_depth + (depth - o - 1)];
            }
        }
    }
}
