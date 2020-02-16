int *new_color = new int[256];

// REFERENCE: https://www.math.uci.edu/icamp/courses/math77c/demos/hist_eq.pdf
int apply_histogram_equalization(Mat *src, int **histogram_buckets) {
    Mat dst = Mat::ones(src->rows, src->cols, CV_8U);

    int num_pixels = src->rows * src->cols;
    double p[256];
    double sum = 0;

    int *new_histogram_buckets = new int[256];
    for (int i = 0; i < 256; i++) {
        new_histogram_buckets[i] = 0;
        new_color[i] = 0;
    }

    int max_bucket = 0;

    for (int i = 0; i < 256; i++) {
        p[i] = (double) (*histogram_buckets)[i] / (double) num_pixels;
        sum += p[i];

        new_histogram_buckets[(int) floor(256 * sum)] += (*histogram_buckets)[i];
        new_color[i] = (int) floor(256 * sum);
        max_bucket = max(max_bucket, new_histogram_buckets[(int) floor(256 * sum)]);
    }

    delete *histogram_buckets;
    *histogram_buckets = new_histogram_buckets;

    for (int i = 0; i < src->rows; i++) {
        for (int j = 0; j < src->cols; j++) {
            dst.at<uint8_t>(i, j) = new_color[src->at<uint8_t>(i, j)];
        }
    }

    src->release();
    *src = dst.clone();

    return max_bucket;
}