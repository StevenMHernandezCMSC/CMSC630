#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

int request_filter_values(double ***kernel) {
    // TODO: request kernel-size from user.
    int kernel_size = 3;

    *kernel = static_cast<double **>(malloc(sizeof(double *) * kernel_size));

    for (int i = 0; i < kernel_size; i++) {
        (*kernel)[i] = static_cast<double *>(malloc(sizeof(double) * kernel_size));
        for (int j = 0; j < kernel_size; j++) {
            (*kernel)[i][j] = 1.0 / (double) (kernel_size * kernel_size);
        }
    }

    // TODO: request from user

    return kernel_size;
}

uint8_t _apply_kernel(Mat *src, double **kernel, int kernel_size, int i, int j) {
    double final_value = 0.0;
    for (int k_i = 0; k_i < kernel_size; k_i++) {
        for (int k_j = 0; k_j < kernel_size; k_j++) {
            final_value += kernel[k_i][k_j] * src->at<uint8_t>(i + k_i, j + k_j);
        }
    }
    return final_value;
}

void kernel_linear(Mat *src, double **kernel, int kernel_size) {
    Mat dst = Mat::ones(src->rows, src->cols, CV_8U);

    for (int i = 0; i < src->rows; i++) {
        for (int j = 0; j < src->cols; j++) {
            dst.at<uint8_t>(i, j) = _apply_kernel(src, kernel, kernel_size, i, j);
        }
    }

    *src = dst.clone();
}