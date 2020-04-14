#include "generic_edge_operator.cpp"

void sobel(Mat *src) {
    int **kernel1 = generate_empty_kernel((int[3][3]) {
            {-1, 0, 1},
            {-2, 0, 2},
            {-1, 0, 1},
    });
    int **kernel2 = generate_empty_kernel((int[3][3]) {
            {-1, -2, -1},
            {0,  0,  0},
            {1,  2,  1},
    });

    generic_edge_operator(src, &kernel1, &kernel2, 3, 8.0);

    free(kernel1);
    free(kernel2);
}