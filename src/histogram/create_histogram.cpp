#include "opencv2/core/ocl.hpp"
#include <cstdio>
#include <iostream>

using namespace std::chrono;

using namespace cv;

int create_histogram(Mat *src, int **buckets) {
    for (int i = 0; i < 256; i++) {
        (*buckets)[i] = 0;
    }

    // Fill histogram buckets
    for (int i = 0; i < src->rows; i++) {
        for (int j = 0; j < src->cols; j++) {
            int bucket_index = (int) src->at<u_int8_t>(i, j);
            (*buckets)[bucket_index]++;
        }
    }

    // Determine largest bucket
    int max_bucket_count = 1;
    for (int i = 0; i < 256; i++) {
        max_bucket_count = max(max_bucket_count, (*buckets)[i]);
    }

    return max_bucket_count;
}