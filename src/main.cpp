#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/ocl.hpp"
#include "filters/noise_salt_and_pepper.cpp"
#include "filters/noise_gaussian.cpp"
#include "filters/grayscale.cpp"
#include "filters/kernel_linear.cpp"
#include "histograms/create_histogram.cpp"

using namespace std::chrono;

using namespace cv;

int main(int argc, char **argv) {
    // TODO: Load ALL images
    // Load raw image
    Mat src, dst;
    src = imread(argv[1]);

    // Add noise
    // TODO: request parameter from user
    noise_salt_and_pepper(&src, 0.1);
    // TODO: request parameter from user
    noise_gaussian(&src, 25);

    // Grayscale
    grayscale(&src);

    // Create Histograms
    Mat histogram;
    create_histogram(&src, &histogram);
    imwrite(argv[3], histogram);

    // Linear Filter
    double **kernel;
    int kernel_size = request_filter_values(&kernel);
    kernel_linear(&src, kernel, kernel_size);

    // Save final image
    imwrite(argv[2], src);

    return 0;
}
