#include <stdio.h>
#include <math.h>
#include "xil_io.h"

// Define your softmax IP register addresses from Vivado Address Editor
#define SOFTMAX_CONTROL_OFFSET 0x40000000
#define SOFTMAX_INPUT_OFFSET   0x40010000
#define SOFTMAX_OUTPUT_OFFSET  0x40020000
#define SOFTMAX_PARAM_OFFSET   0x40030000

#define SIZE 1000

typedef int fixed_t;
#define SCALE 65536  // Q8.16 fixed-point

// Helper functions
fixed_t float_to_fixed(float x) { return (fixed_t)(x * SCALE); }
float fixed_to_float(fixed_t x) { return ((float)x) / SCALE; }

int main() {
    float input_float[SIZE];
    fixed_t input_fixed[SIZE];
    fixed_t output_fixed[SIZE];
    float output_float[SIZE];

    // Example input: just 0.0, 1.0, 2.0, … for testing
    for (int i = 0; i < SIZE; i++) {
        input_float[i] = (float)i / SIZE; // normalized values
        input_fixed[i] = float_to_fixed(input_float[i]);
    }

    // Write input to HLS IP
    for (int i = 0; i < SIZE; i++) {
        Xil_Out32(SOFTMAX_INPUT_OFFSET + i*4, input_fixed[i]);
    }

    // Write parameter (size)
    Xil_Out32(SOFTMAX_PARAM_OFFSET, SIZE);

    // Start Softmax IP
    Xil_Out32(SOFTMAX_CONTROL_OFFSET, 0x01);

    // Wait for completion (polling)
    while ((Xil_In32(SOFTMAX_CONTROL_OFFSET) & 0x2) == 0);

    // Read output
    for (int i = 0; i < SIZE; i++) {
        output_fixed[i] = Xil_In32(SOFTMAX_OUTPUT_OFFSET + i*4);
        output_float[i] = fixed_to_float(output_fixed[i]);
        printf("Output[%d] = %f\n", i, output_float[i]);
    }

    return 0;
}
