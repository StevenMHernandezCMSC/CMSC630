typedef Point3_<uint8_t> Pixel;

void grayscale(Mat *src) {
    Mat dst = Mat::ones(src->rows, src->cols, CV_8U);

    for (int i = 0; i < src->rows; i++) {
        for (int j = 0; j < src->cols; j++) {
            // Simply take the `red` channel of the image.
            // Using a different channel or a more natural grayscaling (using all three channels) may be more desirable.
            dst.at<uint8_t>(i, j) = src->at<Pixel>(i, j).z;
        }
    }

    src->release();
    *src = dst.clone();
}