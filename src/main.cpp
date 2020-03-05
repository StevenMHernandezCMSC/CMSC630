#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/ocl.hpp"
#include "filter/noise_salt_and_pepper.cpp"
#include "filter/noise_gaussian.cpp"
#include "filter/grayscale.cpp"
#include "filter/kernel_linear.cpp"
#include "filter/median_filter.cpp"
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
    std::string filename = argv[1];
    FileStorage fs(filename, FileStorage::READ);
    fs.open(filename, FileStorage::READ);

    FileNode filters = fs["filters"];

    auto EVENT_START_program = std::chrono::system_clock::now();
    double total_processing_times_per_process[filters.size() + 1];
    for (int l = 0; l < filters.size(); l++) {
        total_processing_times_per_process[l] = 0.0;
    }
    auto process_start = std::chrono::system_clock::now();
    auto process_end = std::chrono::system_clock::now();

    std::string *classes;
    std::string input_directory_name = fs["input"];
    std::string output_directory_name = fs["output"];
    int num_classes = get_class_names(&classes, input_directory_name);

    // Load each class individually
    for (int i = 0; i < num_classes; i++) {
        std::string *images;
        int num_images = get_images_by_class(&images, input_directory_name, classes[i]);
        printf("Class [%d/%d]: %s\n", i + 1, num_classes, classes[i].c_str());

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
            printf("Loading image [%d/%d]: %s\n", j + 1, num_images, (input_directory_name + images[j]).c_str());
            src = imread(input_directory_name + images[j]);
            imwrite(output_directory_name + images[j] + "/0.original.BMP", src);
            process_end = std::chrono::system_clock::now();
            total_processing_times_per_process[0] += (process_end - process_start).count();

            // For each filter
            FileNodeIterator it = filters.begin(), it_end = filters.end();
            int filter_number = 1;
            for (; it != it_end; ++it) {
                process_start = std::chrono::system_clock::now();
                std::string filter_name = (std::string) (*it)["name"];
                if ("grayscale" == filter_name) {
                    grayscale(&src);
                } else if ("noise__salt_and_pepper" == filter_name) {
                    noise_salt_and_pepper(&src, (*it)["probability"]);
                } else if ("noise__gaussian" == filter_name) {
                    noise_gaussian(&src, (*it)["variance"]);
                } else if ("uniform_quantization" == filter_name) {
                    uniform_quantization(&src, (*it)["step_size"]);
                } else if ("non_uniform_quantization" == filter_name) {
                    // TODO: build!
                } else if ("linear_filter" == filter_name) {
                    int **kernel = new int *[(*it)["kernel"].size()];
                    for (int k = 0; k < (*it)["kernel"].size(); k++) {
                        kernel[k] = new int[(*it)["kernel"][k].size()];
                        for (int l = 0; l < (*it)["kernel"][k].size(); l++) {
                            kernel[k][l] = (*it)["kernel"][k][l];
                        }
                    }
                    int kernel_size = (*it)["kernel"].size();

                    kernel_linear(&src, kernel, kernel_size, (double) (*it)["scale"]);
                } else if ("median_filter" == filter_name) {
                    int **weights = new int *[(*it)["weights"].size()];
                    for (int k = 0; k < (*it)["weights"].size(); k++) {
                        weights[k] = new int[(*it)["weights"][k].size()];
                        for (int l = 0; l < (*it)["weights"][k].size(); l++) {
                            weights[k][l] = (*it)["weights"][k][l];
                        }
                    }
                    int mask_size = (*it)["weights"].size();

                    median_filter(&src, weights, mask_size);
                } else if ("create_histogram" == filter_name) {
                    Mat histogram;
                    create_histogram(&src, &histogram_buckets);
                    for (int k = 0; k < 256; k++) {
                        histogram_bucket_sums[k] += histogram_buckets[k];
                    }

                    create_histogram_mat(&histogram, &histogram_buckets);
                    std::string file_name = "/histogram." + std::to_string(filter_number) + ".bmp";
                    imwrite(output_directory_name + images[j] + file_name, histogram);
                    histogram.release();
                } else if ("histogram_equalization" == filter_name) {
                    Mat histogram;
                    apply_histogram_equalization(&src, &histogram_buckets);

                    create_histogram_mat(&histogram, &histogram_buckets);
                    std::string file_name = "/histogram." + std::to_string(filter_number) + ".equalization.bmp";
                    imwrite(output_directory_name + images[j] + file_name, histogram);
                    histogram.release();
                } else {
                    printf("Unknown filter-name: %s\nSkipping\n\n", filter_name.c_str());
                    break;
                }

                if ("create_histogram" != filter_name) {
                    std::string file_name = "/" + std::to_string(filter_number) + "." + filter_name + ".BMP";
                    imwrite(output_directory_name + images[j] + file_name, src);
                }
                process_end = std::chrono::system_clock::now();
                total_processing_times_per_process[filter_number] += (process_end - process_start).count();
                filter_number++;
            }

            src.release();
        }

        // Histogram Equalization
        Mat histogram_final;
        create_histogram_mat(&histogram_final, &histogram_bucket_sums);
        imwrite(output_directory_name + classes[i] + ".AVERAGED_histogram.bmp", histogram_final);
        histogram_final.release();
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
