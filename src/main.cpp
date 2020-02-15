#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/ocl.hpp"
#include "filter/noise_salt_and_pepper.cpp"
#include "filter/noise_gaussian.cpp"
#include "filter/grayscale.cpp"
#include "filter/kernel_linear.cpp"
#include "histogram/create_histogram.cpp"
#include "histogram/apply_histogram_equalization.cpp"
#include "histogram/create_histogram_mat.cpp"
#include "dataset/get_class_names.cpp"

using namespace std::chrono;

using namespace cv;

int main(int argc, char **argv) {
    std::string *classes;
//    int num_classes = get_class_names(&classes, "/Users/steven/projects/school/MASTERS/CMSC630/data/raw/Cancerous cell smears/");

    // TODO: Load ALL images
    // Load raw image
    Mat src, dst;
    src = imread(argv[1]);

    // Add noise
    // TODO: request parameter from user
    noise_salt_and_pepper(&src, 0.01);
    // TODO: request parameter from user
    noise_gaussian(&src, 5);

    // Grayscale
    grayscale(&src);

    // Create Histograms
    Mat histogram;
    int *histogram_buckets = static_cast<int *>(malloc(sizeof(int) * 256));
    int max_bucket = create_histogram(&src, &histogram_buckets);
    create_histogram_mat(&histogram, &histogram_buckets, max_bucket);
    imwrite(argv[3], histogram);

    // Histogram Equalization
    max_bucket = apply_histogram_equalization(&src, &histogram_buckets);
    create_histogram_mat(&histogram, &histogram_buckets, max_bucket);
    imwrite(argv[3], histogram);

    // Linear Filter
    double **kernel;
    int kernel_size = request_filter_values(&kernel);
    kernel_linear(&src, kernel, kernel_size);

    // Save final image
    imwrite(argv[2], src);

    return 0;
}
