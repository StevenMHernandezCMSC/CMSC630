#include "generic_edge_operator.cpp"

void improved_sobel(Mat *src) {
    int **kernel1 = generate_empty_kernel((int[3][3]) {
            {-3, 0, 3},
            {-10, 0, 10},
            {-3, 0, 3},
    });
    int **kernel2 = generate_empty_kernel((int[3][3]) {
            {-3, -10, -3},
            {0,  0,  0},
            {3,  10,  3},
    });

    generic_edge_operator(src, &kernel1, &kernel2, 3, 32.0);

    free(kernel1);
    free(kernel2);
}