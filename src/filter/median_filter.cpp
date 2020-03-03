#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

void median_filter(Mat *src, double **kernel, int kernel_size) {
    Mat dst = Mat::ones(src->rows, src->cols, CV_8U);

    uint8_t mask_size = 3; // TODO: user should input
    uint8_t mask_dimension = (mask_size - 1) / 2;

    std::array<uint8_t, 9> buffer; // TODO: user should input

    for (int i = 0 + 1; i < src->rows; i++) {
        for (int j = 0 + 1; j < src->cols; j++) {
            // Create a buffer for sorting
            int m = 0;
            for (int k = -mask_dimension; k <= mask_dimension; k++) {
                for (int l = -mask_dimension; l <= mask_dimension; l++) {
                    buffer[m] = src->at<uint8_t>(i + k, j + l);
                    m++;
                }
            }

            std::sort(buffer.begin(), buffer.end());

            // select median
            dst.at<uint8_t>(i, j) = buffer[((mask_size*mask_size) + 1)/2];
        }
    }

    src->release();
    *src = dst.clone();
}