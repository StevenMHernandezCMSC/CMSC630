#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/ocl.hpp"
#include "filter/noise_salt_and_pepper.cpp"
#include "filter/noise_gaussian.cpp"
#include "filter/grayscale.cpp"
#include "filter/kernel_linear.cpp"
#include "filter/uniform_quantization.cpp"
#include "histogram/create_histogram.cpp"
#include "histogram/apply_histogram_equalization.cpp"
#include "histogram/create_histogram_mat.cpp"
#include "dataset/get_class_names.cpp"

using namespace std::chrono;

using namespace cv;

int main(int argc, char **argv) {
    std::string *classes;
    int num_classes = get_class_names(&classes, argv[1]);

    // TODO: Load ALL images
    // Load raw image
    Mat src;
    src = imread(argv[2]);

    // Add noise
    // TODO: request parameter from user
    noise_salt_and_pepper(&src, 0.01);
    // TODO: request parameter from user
    noise_gaussian(&src, 50);

    // Grayscale
    grayscale(&src);

    // Create Histograms
    Mat histogram;
    int *histogram_buckets = new int[256];
    int max_bucket = create_histogram(&src, &histogram_buckets);
    create_histogram_mat(&histogram, &histogram_buckets, max_bucket);
    imwrite(argv[4], histogram);

    // Histogram Equalization
    max_bucket = apply_histogram_equalization(&src, &histogram_buckets);
    create_histogram_mat(&histogram, &histogram_buckets, max_bucket);
    imwrite(argv[4], histogram);
    delete[] histogram_buckets;

    // Uniform Quantization
    // TODO: request parameter from user
    uniform_quantization(&src, 4);

    // Linear Filter
    double **kernel;
    int kernel_size = request_filter_values(&kernel);
    kernel_linear(&src, kernel, kernel_size);
    free_requested_filter_values(&kernel, kernel_size);

    // Save final image
    imwrite(argv[3], src);

    src.release();

    return 0;
}
