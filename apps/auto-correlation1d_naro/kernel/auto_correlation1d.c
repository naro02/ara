#include "auto_correlation1d.h"

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

void auto_correlation_v64b(int64_t *c, int64_t *a, int64_t length)
{
    size_t vl;
    for (int64_t k = 0; k < length; k++)
    {
        int64_t sum = 0;
        for (size_t i = 0; i < length; i += vl)
        {
            if (i + k >= length)
                break;
            // Set the vector configuration
            vl = vsetvl_e64m1(length - (i + k));
            // load elements to vector register
            vint64m1_t va = vle64_v_i64m1(&a[i], vl);
            vint64m1_t vb = vle64_v_i64m1(&a[i + k], vl);

            // dot product of va, vb
            vint64m1_t vmul;
            asm volatile("vmul.vv %0, %1, %2" : "=vr"(vmul) : "vr"(va), "vr"(vb));
            vint64m1_t vzero = vmv_v_x_i64m1(0, vl);
            vint64m1_t vsum;
            // adding elements of vmul
            asm volatile("vredsum.vs %0, %1, %2" : "=vr"(vsum) : "vr"(vmul), "vr"(vzero));
            int64_t partial_sum;
            asm volatile("vmv.x.s %0, %1" : "=r"(partial_sum) : "vr"(vsum));
            sum += partial_sum;
        }
        c[k + length - 1] = sum;
        c[-k + length - 1] = sum;
    }
}

// Scalar version of autocorrelation
void auto_correlation_s64b(int64_t *c, int64_t *a, int64_t length)
{
    for (int64_t k = 0; k < (int64_t)length; k++)
    {
        int64_t sum = 0;
        for (int64_t n = 0; n < length; n++)
        {
            if (n + k >= 0 && n + k < length)
            {
                sum += a[n] * a[n + k];
            }
        }
        c[k + length - 1] = sum;
        c[-k + length - 1] = sum;
    }
}
