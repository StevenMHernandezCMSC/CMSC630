void histogram_threshold(Mat *src, int **histogram_buckets) {
    int best_theshold = 0;
    double best_threshold_within_group_variance = 99999999;

    double N = src->rows * src->cols;
    double p_o = 0;
    double p_b = 0;
    double mu_o = 0;
    double mu_b = 0;
    double sigma_o = 0;
    double sigma_b = 0;

    // Initialize probabilities object
    for (int i = 0; i < 256; i++) {
        double h_i = ((double) (*histogram_buckets)[i]);
        double P_i = h_i / N;
        p_b += P_i;
        mu_b += (h_i * P_i) / p_b;
        sigma_b += pow(h_i - mu_b, 2.0) * P_i / p_b;
    }

    // GOAL: Minimize within group variance between object (o) and background (b).
    for (int i = 0; i < 256; i++) {
        double h_i = ((double) (*histogram_buckets)[i]);
        double P_i = h_i / N;

        double sigma_W = (sigma_o * p_o) + (sigma_b + p_b);

        printf("%i,%lf,%lf;\n", i, h_i, sigma_W);

        if (best_threshold_within_group_variance > sigma_W) {
            best_threshold_within_group_variance = sigma_W;
            best_theshold = i;
        }

        p_o += P_i;
        mu_o += (h_i * P_i) / p_o;
        sigma_o += pow(h_i - mu_o, 2.0) * P_i / p_o;

        p_b -= P_i;
        mu_b -= (h_i * P_i) / p_b;
        sigma_b -= pow(h_i - mu_b, 2.0) * P_i / p_b;
    }

    for (int i = 0; i < src->rows; i++) {
        for (int j = 0; j < src->cols; j++) {
            src->at<uint8_t>(i, j) = src->at<uint8_t>(i, j) > best_theshold ? 255 : 0;
        }
    }
}