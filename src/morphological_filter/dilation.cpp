// ASSUMPTION: all pixels in `src` are either `0` or `255`, a.k.a: binary image
void dilation(Mat *src) {
    int structuring_element[3][3] = {
            {1, 0, 1},
            {0, 1, 0},
            {1, 0, 1},
    };
    int structuring_element_size = 3;
    // ASSUMPTION: origin is at the center of the square structuring element
    int origin_offset = ((structuring_element_size - 1) / 2);

    Mat dst = Mat::ones(src->rows, src->cols, CV_8U);

    for (int i = 0; i < src->rows; i++) {
        for (int j = 0; j < src->cols; j++) {
            // If ANY of the pixels from the structuring element match, then pixel is colored `255`.
            dst.at<uint8_t>(i, j) = 0;
            for (int m = -origin_offset; m < origin_offset; m++) {
                for (int n = -origin_offset; n < origin_offset; n++) {
                    if (structuring_element[m + origin_offset][n + origin_offset] && src->at<uint8_t>(i + m, j + n)) {
                        dst.at<uint8_t>(i, j) = 255;
                    }
                }
            }
        }
    }

    src->release();
    *src = dst.clone();
}