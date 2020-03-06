#include <random>


void noise_gaussian(Mat *src, double standard_deviation) {
    // SOURCE: https://en.cppreference.com/w/cpp/numeric/random/normal_distribution
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> dis(0.0, standard_deviation);

    Mat dst = src->clone();

    for (int i = 0; i < src->rows; i++) {
        for (int j = 0; j < src->cols; j++) {
            u_int8_t p = src->at<u_int8_t>(i, j);
            p = max(0, min(255, (int) (p + dis(gen))));
            dst.at<u_int8_t>(i, j) = p;
        }
    }

    src->release();
    *src = dst.clone();
}