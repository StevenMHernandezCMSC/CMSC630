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
    double total_processing_times_per_process[8];
    for (int l = 0; l < 8; l++) {
        total_processing_times_per_process[l] = 0.0;
    }
    auto process_start = std::chrono::system_clock::now();
    auto process_end = std::chrono::system_clock::now();


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

        int *histogram_bucket_sums = new int[256];
        for (int k = 0; k < 256; k++) {
            histogram_bucket_sums[k] = 0;
        }

        // Load each image per class
        for (int j = 0; j < num_images; j++) {
            // Create directory for
            boost::filesystem::path dir(output_directory_name + images[j] + "/");
            boost::filesystem::create_directory(dir);

            // Load raw image
            process_start = std::chrono::system_clock::now();
            Mat src;
            printf("Loading image [%d/%d]: %s\n", j+1, num_images, (input_directory_name + images[j]).c_str());
            src = imread(input_directory_name + images[j]);
            imwrite(output_directory_name + images[j] + "/0.original.BMP" , src);
            process_end = std::chrono::system_clock::now();
            total_processing_times_per_process[0] += (process_end - process_start).count();

            // Grayscale
            process_start = std::chrono::system_clock::now();
            grayscale(&src);
            imwrite(output_directory_name + images[j] + "/1.grayscale.BMP" , src);
            process_end = std::chrono::system_clock::now();
            total_processing_times_per_process[1] += (process_end - process_start).count();

            // Add Salt and Pepper noise
            process_start = std::chrono::system_clock::now();
            noise_salt_and_pepper(&src, salt_and_pepper_probability);
            imwrite(output_directory_name + images[j] + "/2.salt_and_pepper_noise.BMP" , src);
            process_end = std::chrono::system_clock::now();
            total_processing_times_per_process[2] += (process_end - process_start).count();

            // Add Gaussian noise
            process_start = std::chrono::system_clock::now();
            noise_gaussian(&src, gassian_noise_standard_deviation);
            imwrite(output_directory_name + images[j] + "/3.gaussian_noise.BMP" , src);
            process_end = std::chrono::system_clock::now();
            total_processing_times_per_process[3] += (process_end - process_start).count();

            // Create Histograms
            process_start = std::chrono::system_clock::now();
            Mat histogram1;
            create_histogram(&src, &histogram_buckets);
            create_histogram_mat(&histogram1, &histogram_buckets);
            imwrite(output_directory_name + images[j] + "/histogram.3.gaussian_noise.bmp", histogram1);
            histogram1.release();
            for (int k = 0; k < 256; k++) {
                histogram_bucket_sums[k] += histogram_buckets[k];
            }
            process_end = std::chrono::system_clock::now();
            total_processing_times_per_process[4] += (process_end - process_start).count();

            // Histogram Equalization
            process_start = std::chrono::system_clock::now();
            Mat histogram2;
            apply_histogram_equalization(&src, &histogram_buckets);
            imwrite(output_directory_name + images[j] + "/4.histogram_equalization.BMP" , src);
            create_histogram_mat(&histogram2, &histogram_buckets);
            imwrite(output_directory_name + images[j] + "/histogram.4.histogram_equalization.bmp", histogram2);
            histogram2.release();
            process_end = std::chrono::system_clock::now();
            total_processing_times_per_process[5] += (process_end - process_start).count();

            // Uniform Quantization
            process_start = std::chrono::system_clock::now();
            uniform_quantization(&src, quantization_step_size);
            imwrite(output_directory_name + images[j] + "/5.uniform_quantization.BMP" , src);
            process_end = std::chrono::system_clock::now();
            total_processing_times_per_process[6] += (process_end - process_start).count();

            // Linear Filter
            process_start = std::chrono::system_clock::now();
            kernel_linear(&src, kernel, kernel_size);
            imwrite(output_directory_name + images[j] + "/6.linear_filter.BMP" , src);
            process_end = std::chrono::system_clock::now();
            total_processing_times_per_process[7] += (process_end - process_start).count();

            src.release();
        }

        // Histogram Equalization
        Mat histogram2;
        create_histogram_mat(&histogram2, &histogram_bucket_sums);
        imwrite(output_directory_name + classes[i] + ".AVERAGED_histogram.bmp", histogram2);
        histogram2.release();
        delete[] histogram_bucket_sums;
        delete[] images;
    }


    auto EVENT_END_program = std::chrono::system_clock::now();

    printf("[EVENT_PROGRAM]:%lf\n", (double) (EVENT_END_program - EVENT_START_program).count());
    for (int m = 0; m < 8; m++) {
        printf("[EVENT_PROCESS_%d_TIME]:%lf\n", m, total_processing_times_per_process[m]);
    }

    return 0;
}
