#include <random>
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;


void noise_salt_and_pepper(Mat *src, double noise_probability) {
    // SOURCE: https://en.cppreference.com/w/cpp/numeric/random/uniform_real_distribution
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    Mat dst = src->clone();

    for (int i = 0; i < src->rows; i++) {
        for (int j = 0; j < src->cols; j++) {
            if (dis(gen) < noise_probability) {
                if (dis(gen) < 0.5) {
                    dst.at<u_int8_t>(i, j) = 0;
                } else {
                    dst.at<u_int8_t>(i, j) = 255;
                }
            }
        }
    }

    src->release();
    *src = dst.clone();
}