void uniform_quantization(Mat *src, int step_size) {
    Mat dst = Mat::ones(src->rows, src->cols, CV_8U);

    int num_thresholds = ceil(256 / step_size);
    int q[256];

    for (int i = 0; i < num_thresholds - 1; i++) {
        for (int j = (i * step_size); j < ((i + 1) * step_size); j++) {
            q[j] = (int) round((i + 0.5) * step_size);
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