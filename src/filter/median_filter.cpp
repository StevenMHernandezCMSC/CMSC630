#include <vector>
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

void median_filter(Mat *src, int **weights, int mask_size) {
    Mat dst = Mat::ones(src->rows, src->cols, CV_8U);

    uint8_t mask_dimension = (mask_size - 1) / 2;

    int buffer_size = 0;
    for (int k = 0; k < mask_size; k++) {
        for (int l = 0; l < mask_size; l++) {
            buffer_size += weights[k][l];
        }
    }
    std::vector<uint8_t> buffer(buffer_size);

    for (int i = 0 + 1; i < src->rows; i++) {
        for (int j = 0 + 1; j < src->cols; j++) {
            // Create a buffer for sorting
            int m = 0;
            for (int k = -mask_dimension; k <= mask_dimension; k++) {
                for (int l = -mask_dimension; l <= mask_dimension; l++) {
                    for (int n = 0; n < weights[k + 1][l + 1]; n++) {
                        buffer[m] = src->at<uint8_t>(i + k, j + l);
                        m++;
                    }
                }
            }

            std::sort(buffer.begin(), buffer.end());

            dst.at<uint8_t>(i, j) = buffer[((mask_size*mask_size) + 1)/2];
        }
    }

    src->release();
    *src = dst.clone();
}