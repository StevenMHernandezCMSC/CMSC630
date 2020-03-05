double uniform_quantization(Mat *src, int step_size) {
    Mat dst = Mat::ones(src->rows, src->cols, CV_8U);

    int num_thresholds = ceil(256.0 / step_size);
    int q[256];

    for (int i = 0; i < num_thresholds; i++) {
        for (int j = (i * step_size); j < min(256, ((i + 1) * step_size)); j++) {
            double min_bound = i * step_size;
            double max_bound = min(256,(i + 1) * step_size);
            q[j] = (int) (((max_bound - min_bound) / 2) + min_bound);
        }
    }

    double quantization_error = 0.0;

    for (int i = 0; i < src->rows; i++) {
        for (int j = 0; j < src->cols; j++) {
            quantization_error += pow(q[src->at<uint8_t>(i, j)] - src->at<uint8_t>(i, j), 2);
            dst.at<uint8_t>(i, j) = q[src->at<uint8_t>(i, j)];
        }
    }

    quantization_error = quantization_error / (src->rows * src->cols);

    src->release();
    *src = dst.clone();

    return quantization_error;
}