#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

uint8_t _apply_kernel(Mat *src, int **kernel, int kernel_size, int i, int j) {
    double final_value = 0.0;
    for (int k_i = 0; k_i < kernel_size; k_i++) {
        for (int k_j = 0; k_j < kernel_size; k_j++) {
            final_value += kernel[k_i][k_j] * src->at<uint8_t>(i + k_i, j + k_j);
        }
    }
    return final_value;
}

void kernel_linear(Mat *src, int **kernel, int kernel_size, double scale) {
    Mat dst = Mat::ones(src->rows, src->cols, CV_8U);

    for (int i = 0; i < src->rows; i++) {
        for (int j = 0; j < src->cols; j++) {
            dst.at<uint8_t>(i, j) = scale * _apply_kernel(src, kernel, kernel_size, i, j);
        }
    }

    src->release();
    *src = dst.clone();
}