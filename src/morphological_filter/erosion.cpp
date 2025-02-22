// ASSUMPTION: all pixels in `src` are either `0` or `255`, a.k.a: binary image
void erosion(Mat *src) {
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
            // Check all pixels (based on structuring element).
            // If ALL match, then pixel is valid and can be colored `255`.
            bool valid = true;
            for (int m = -origin_offset; m <= origin_offset; m++) {
                for (int n = -origin_offset; n <= origin_offset; n++) {
                    int x_coord = i + m;
                    int y_coord =  j + n;
                    if (0 <= x_coord && x_coord <= src->rows - 1 && 0 <= y_coord && y_coord <= src->cols - 1) {
                        if (structuring_element[m + origin_offset][n + origin_offset]) {
                            if (src->at<uint8_t>(x_coord, y_coord) == 0) {
                                valid = false;
                            }
                        }
                    }
                }
            }
            dst.at<uint8_t>(i, j) = valid ? 255 : 0;
        }
    }

    src->release();
    *src = dst.clone();
}