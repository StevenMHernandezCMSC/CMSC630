#include "opencv2/core/ocl.hpp"
#include <cstdio>
#include <iostream>

using namespace std::chrono;

using namespace cv;

void create_histogram(Mat *src, Mat *dst) {
    // Initialize histogram buckets
    int buckets[255];
    for (int i = 0; i < 255; i++) {
        buckets[i] = 0;
    }

    // Fill histogram buckets
    for (int i = 0; i < src->rows; i++) {
        for (int j = 0; j < src->cols; j++) {
            int bucket_index = (int) src->at<u_int8_t>(i, j);
            buckets[bucket_index]++;
        }
    }

    // Determine largest bucket
    int max_bucket_count = 1;
    for (int i = 0; i < 255; i++) {
        max_bucket_count = max(max_bucket_count, buckets[i]);
    }

    // Create histogram output image matrix
    int image_height = min(255, max_bucket_count);
    *dst = Mat::zeros(image_height, 255, CV_8U);
    for (int i = 0; i < 255; ++i) {
        for (int j = 0; j < image_height - floor(((double) buckets[i] / max_bucket_count) * image_height); j++) {
            dst->at<u_int8_t>(j, i) = 255;
        }
    }
}