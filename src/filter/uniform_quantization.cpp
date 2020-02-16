void uniform_quantization(Mat *src, int step_size) {
    Mat dst = Mat::ones(src->rows, src->cols, CV_8U);

    for (int i = 0; i < src->rows; i++) {
        for (int j = 0; j < src->cols; j++) {
            dst.at<uint8_t>(i, j) = step_size * floor(src->at<uint8_t>(i, j) / step_size) - floor(step_size/2);
        }
    }

    src->release();
    *src = dst.clone();
}