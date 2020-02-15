void create_histogram_mat(Mat *dst, int **buckets, int max_bucket_count) {
    // Create histogram output image matrix
    int image_height = min(255, max_bucket_count);
    *dst = Mat::zeros(image_height, 255, CV_8U);
    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < image_height - floor(((double) (*buckets)[i] / max_bucket_count) * image_height); j++) {
            dst->at<u_int8_t>(j, i) = 255;
        }
    }
}