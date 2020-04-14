#ifndef GENERIC_EDGE_OPERATOR
#define GENERIC_EDGE_OPERATOR

int **generate_empty_kernel(int def[3][3]) {
    int kernel_size = 3; // Hardcoded for simplicity.
    int **kernel = static_cast<int **>(malloc(sizeof(int *) * kernel_size));

    for (int i = 0; i < kernel_size; i++) {
        (kernel)[i] = static_cast<int *>(malloc(sizeof(int) * kernel_size));
        for (int j = 0; j < kernel_size; j++) {
            (kernel)[i][j] = def[i][j];
        }
    }
    return kernel;
}

void generic_edge_operator(Mat *src, int ***kernel1, int ***kernel2, int kernel_size, double scale) {
    Mat dst1 = Mat::ones(src->rows, src->cols, CV_16S);
    Mat dst2 = Mat::ones(src->rows, src->cols, CV_16S);
    Mat dst_magnitude = Mat::ones(src->rows, src->cols, CV_8U);

    // Apply first kernel
    for (int i = 0; i < src->rows; i++) {
        for (int j = 0; j < src->cols; j++) {
            dst1.at<int16_t>(i, j) = (int) max(-255.0, min(255.0, ((1.0/scale) * _apply_kernel(src, *kernel1, kernel_size, i, j))));
        }
    }

    // Apply second kernel
    for (int i = 0; i < src->rows; i++) {
        for (int j = 0; j < src->cols; j++) {
            dst2.at<int16_t>(i, j) = (int) max(-255.0, min(255.0, ((1.0/scale) * _apply_kernel(src, *kernel2, kernel_size, i, j))));
        }
    }

    // Get Magnitude
    for (int i = 0; i < src->rows; i++) {
        for (int j = 0; j < src->cols; j++) {
            double magnitude = sqrt(pow((double)dst1.at<int16_t>(i, j),2.0) + pow((double)dst2.at<int16_t>(i, j),2.0));
            printf("%i<->%i = %lf\n", dst1.at<int16_t>(i, j), dst2.at<int16_t>(i, j), magnitude);
            dst_magnitude.at<uint8_t>(i, j) = (int) max(0.0, min(255.0, magnitude));
        }
    }

    src->release();
    dst1.release();
    dst2.release();
    *src = dst_magnitude.clone();
}

#endif