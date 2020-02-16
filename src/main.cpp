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
#include "dataset/get_images_by_class.cpp"

using namespace std::chrono;

using namespace cv;

int *histogram_buckets = new int[256];

int main(int argc, char **argv) {
    auto EVENT_START_program = std::chrono::system_clock::now();

    std::string *classes;
    std::string input_directory_name = argv[1];
    std::string output_directory_name = argv[2];
    int num_classes = get_class_names(&classes, input_directory_name);

    // TODO: Get user requested parameters
    double salt_and_pepper_probability = 0.01;
    double gassian_noise_standard_deviation = 5.0;
    double **kernel;
    int kernel_size = request_filter_values(&kernel);
    int quantization_step_size = 4;

    // Load each class individually
    for (int i = 0; i < num_classes; i++) {
        std::string *images;
        int num_images = get_images_by_class(&images, argv[1], classes[i]);
        printf("Class [%d/%d]: %s\n", i+1, num_classes, classes[i].c_str());

        // Load each image per class
        for (int j = 0; j < num_images; j++) {
            // Load raw image
            Mat src;
            src = imread(input_directory_name + images[j]);
            printf("Loading image [%d/%d]: %s\n", j+1, num_images, (input_directory_name + images[j]).c_str());

            // Add noise
            noise_salt_and_pepper(&src, salt_and_pepper_probability);
            noise_gaussian(&src, gassian_noise_standard_deviation);

            // Grayscale
            grayscale(&src);

            // Create Histograms
            Mat histogram1;
            int max_bucket = create_histogram(&src, &histogram_buckets);
            create_histogram_mat(&histogram1, &histogram_buckets, max_bucket);
            imwrite(output_directory_name + images[j] + ".histogram1.bmp", histogram1);
            histogram1.release();

            // Histogram Equalization
            Mat histogram2;
            max_bucket = apply_histogram_equalization(&src, &histogram_buckets);
            create_histogram_mat(&histogram2, &histogram_buckets, max_bucket);
            imwrite(output_directory_name + images[j] + ".histogram2.bmp", histogram2);
            histogram2.release();

            // Uniform Quantization
            uniform_quantization(&src, quantization_step_size);

            // Linear Filter
            kernel_linear(&src, kernel, kernel_size);

            // Save final image
            imwrite(output_directory_name + images[j], src);

            src.release();
        }

        delete[] images;
    }


    auto EVENT_END_program = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = EVENT_END_program - EVENT_START_program;

    printf("[EVENT_PROGRAM]:%lf", diff);

    return 0;
}
