// Using Otsu's method described in (https://www.cse.unr.edu/~bebis/CS791E/Notes/Thresholding.pdf)
int histogram_threshold_calculate(Mat *src, int **histogram_buckets) {
    int best_theshold = 0;
    double best_threshold_within_group_variance = -99999999;

    double N = src->rows * src->cols;
    double p_o = 0;
    double p_b = 0;
    double sum_o = 0;
    double sum_b = 0;
    double mu_o = 0;
    double mu_b = 0;
    double mu_T = 0;

    // Initialize probabilities object
    for (int i = 0; i < 256; i++) {
        double h_i = ((double) (*histogram_buckets)[i]);
        double P_i = h_i / N;
        p_b += P_i;
        if (p_b != 0) {
            sum_b += (i * P_i);
            mu_b = sum_b / p_b;
            mu_T = sum_b;
        }
    }

    // GOAL: Minimize within group variance between object (o) and background (b).
    for (int i = 0; i < 256; i++) {
        double h_i = ((double) (*histogram_buckets)[i]);
        double P_i = h_i / N;

        p_o += P_i;
        if (p_o != 0) {
            sum_o += (i * P_i);
            mu_o = sum_o / p_o;
        }

        p_b -= P_i;
        if (p_b != 0) {
            sum_b -= (i * P_i);
            mu_b = sum_b / p_b;
        }

        double sigma_W = p_o * pow(mu_o - mu_T, 2.0) + p_b * pow(mu_b - mu_T, 2.0);

        if (best_threshold_within_group_variance < sigma_W) {
            best_threshold_within_group_variance = sigma_W;
            best_theshold = i;
        }
    }

    return best_theshold;
}