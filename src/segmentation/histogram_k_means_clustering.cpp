#include <random>

int histogram_k_means_clustering(Mat *src, int **histogram_buckets, int k) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    double cluster_means[] = {255 * dis(gen), 255 * dis(gen)};
    double cluster_sums[] = {0.0, 0.0};
    double cluster_counts[] = {0.0, 0.0};
    int previous_counts_0 = 0;

    // K-means.
    int tries = 0;
    while (true) {
        // Initialize
        for (int j = 0; j < k; j++) {
            cluster_sums[j] = 0.0;
            cluster_counts[j] = 0.0;
        }

        // Gather Statistics based on closest clusters per histogram bucket
        for (int i = 0; i < 256; i++) {
            int closest_cluster = 0;
            double cluster_distance = 999999.0;
            for (int j = 0; j < k; j++) {
                double d = abs(cluster_means[j] - ((double) i));
                if (d < cluster_distance) {
                    closest_cluster = j;
                    cluster_distance = d;
                }
            }
            cluster_sums[closest_cluster] += ((double) (*histogram_buckets)[i]) * ((double) i);
            cluster_counts[closest_cluster] += (double) (*histogram_buckets)[i];
        }

        // Update cluster means.
        for (int j = 0; j < k; j++) {
            if (cluster_counts[j] > 0) {
                cluster_means[j] = cluster_sums[j] / cluster_counts[j];
            }
        }

        // Continue until convergence
        if (previous_counts_0 == (int) cluster_counts[0] || tries > 99) {
            break;
        }

        previous_counts_0 = (int) cluster_counts[0];
        tries++;
    }

    return (int) round(min(cluster_means[0], cluster_means[1]) + abs(cluster_means[0] - cluster_means[1]));
}