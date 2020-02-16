#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

typedef Point3_<uint8_t> Pixel;


void noise_gaussian(Mat *src, double standard_deviation) {
    // SOURCE: https://en.cppreference.com/w/cpp/numeric/random/normal_distribution
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> dis(0.0, standard_deviation);

    Mat dst = src->clone();

    for (int i = 0; i < src->rows; i++) {
        for (int j = 0; j < src->cols; j++) {
            Pixel p = src->at<Pixel>(i, j);
            p.x = max(0, min(255, (int) (p.x + dis(gen))));
            p.y = max(0, min(255, (int) (p.y + dis(gen))));
            p.z = max(0, min(255, (int) (p.z + dis(gen))));
            dst.at<Pixel>(i, j) = p;
        }
    }

    src->release();
    *src = dst.clone();
}