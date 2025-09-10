#include <stdio.h>

#define SIZE 1000
typedef int fixed_t;
#define SCALE 65536  // Q8.16 format (16 bits for fraction)

fixed_t float_to_fixed(float x) { return (fixed_t)(x * SCALE); }
float fixed_to_float(fixed_t x) { return ((float)x) / SCALE; }

// Simple 3-term Taylor expansion for exp(x) in fixed-point
fixed_t fixed_exp(fixed_t x) {
    fixed_t x2 = (x * x) / SCALE;
    fixed_t x3 = (x2 * x) / SCALE;
    return SCALE + x + x2 / 2 + x3 / 6;
}

// HLS Softmax IP in fixed-point
void Softmax(fixed_t mem_block2_dense[SIZE], fixed_t bd3[SIZE], int class[SIZE], fixed_t result[SIZE], int size) {
#pragma HLS INTERFACE s_axilite port=mem_block2_dense bundle=INPUT
#pragma HLS INTERFACE s_axilite port=bd3 bundle=parameter
#pragma HLS INTERFACE s_axilite port=class bundle=OUTPUT
#pragma HLS INTERFACE s_axilite port=result bundle=OUTPUT
#pragma HLS INTERFACE s_axilite port=size bundle=parameter
#pragma HLS INTERFACE s_axilite port=return bundle=CONTROL

    int i;
    int counter = 0;

    // Step 1: find max value
    fixed_t max_val = mem_block2_dense[0];
    for (i = 1; i < size; i++) {
#pragma HLS PIPELINE II=1
        if (mem_block2_dense[i] > max_val)
            max_val = mem_block2_dense[i];
    }

    // Step 2: compute exponentials and sum
    fixed_t sum = 0;
    for (i = 0; i < size; i++) {
#pragma HLS PIPELINE II=2
        mem_block2_dense[i] = fixed_exp(mem_block2_dense[i] - max_val);
        sum += mem_block2_dense[i];
    }

    // Step 3: normalize
    for (i = 0; i < size; i++) {
#pragma HLS PIPELINE II=1
        mem_block2_dense[i] = (mem_block2_dense[i] * SCALE) / sum;
    }

    // Step 4: write output and class
    for (i = 0; i < size; i++) {
#pragma HLS PIPELINE II=2
        class[i] = counter;
        result[i] = mem_block2_dense[i];
        counter++;
    }

    return;
}
