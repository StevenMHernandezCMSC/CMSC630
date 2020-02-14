#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/ocl.hpp"
#include "filter/noise_salt_and_pepper.cpp"
#include "filter/noise_gaussian.cpp"
#include "filter/grayscale.cpp"
#include "filter/kernel_linear.cpp"
#include "histogram/create_histogram.cpp"
#include "dataset/get_class_names.cpp"

using namespace std::chrono;

using namespace cv;

int main(int argc, char **argv) {
    std::string *classes;
    int num_classes = get_class_names(&classes, "/Users/steven/projects/school/MASTERS/CMSC630/data/raw/Cancerous cell smears/");

    for (int i = 0; i < num_classes; i++) {
        printf("Class #%d name: %s\n", i, classes[i].c_str());
    }


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
