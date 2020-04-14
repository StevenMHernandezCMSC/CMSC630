#include "generic_edge_operator.cpp"

void prewitt(Mat *src) {
    int **kernel1 = generate_empty_kernel((int[3][3]) {
            {-1, 0, 1},
            {-1, 0, 1},
            {-1, 0, 1},
    });
    int **kernel2 = generate_empty_kernel((int[3][3]) {
            {-1, -1, -1},
            {0,  0,  0},
            {1,  1,  1},
    });

    generic_edge_operator(src, &kernel1, &kernel2, 3, 6.0);

    free(kernel1);
    free(kernel2);
}