void non_uniform_quantization(Mat *src, const int *thresholds, const int *reproduction_levels, int num_thresholds) {
    Mat dst = Mat::ones(src->rows, src->cols, CV_8U);

    int q[256];

    for (int i = 0; i < num_thresholds - 1; i++) {
        for (int j = thresholds[i]; j < thresholds[i + 1]; j++) {
            q[j] = reproduction_levels[i];
        }
    }

    for (int i = 0; i < src->rows; i++) {
        for (int j = 0; j < src->cols; j++) {
            dst.at<uint8_t>(i, j) = q[src->at<uint8_t>(i, j)];
        }
    }

    src->release();
    *src = dst.clone();
}