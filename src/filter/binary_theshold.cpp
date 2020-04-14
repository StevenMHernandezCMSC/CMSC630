void binary_theshold(Mat *src, int theshold) {
    for (int i = 0; i < src->rows; i++) {
        for (int j = 0; j < src->cols; j++) {
            src->at<uint8_t>(i, j) = src->at<uint8_t>(i, j) > theshold ? 255 : 0;
        }
    }
}