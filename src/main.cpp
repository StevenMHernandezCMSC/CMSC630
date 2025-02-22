#include <iostream>
#include <fstream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/ocl.hpp"
#include "filter/noise_salt_and_pepper.cpp"
#include "filter/noise_gaussian.cpp"
#include "filter/grayscale.cpp"
#include "filter/kernel_linear.cpp"
#include "filter/median_filter.cpp"
#include "filter/uniform_quantization.cpp"
#include "filter/non_uniform_quantization.cpp"
#include "segmentation/binary_theshold.cpp"
#include "segmentation/histogram_threshold_calculate.cpp"
#include "segmentation/histogram_k_means_clustering.cpp"
#include "segmentation/edge_detection/prewitt.cpp"
#include "segmentation/edge_detection/sobel.cpp"
#include "segmentation/edge_detection/improved_sobel.cpp"
#include "morphological_filter/erosion.cpp"
#include "morphological_filter/dilation.cpp"
#include "histogram/create_histogram.cpp"
#include "histogram/apply_histogram_equalization.cpp"
#include "histogram/create_histogram_mat.cpp"
#include "dataset/get_class_names.cpp"
#include "dataset/get_images_by_class.cpp"
#include "feature_extraction/calculate_area.cpp"
#include "feature_extraction/number_of_cells.h"
#include "feature_extraction/average_cell_size.h"
#include "feature_extraction/smallest_cell_size.h"
#include "feature_extraction/largest_cell_size.h"

using namespace std::chrono;

using namespace cv;

int *histogram_buckets = new int[256];

int main(int argc, char **argv) {
    std::string filename = argv[1];
    FileStorage fs(filename, FileStorage::READ);
    fs.open(filename, FileStorage::READ);

    FileNode filters = fs["filters"];
    FileNode features = fs["features"];

    auto EVENT_START_program = std::chrono::system_clock::now();
    double total_processing_times_per_process[filters.size() + 1]; // Plus one for the load_image process (not defined in batch.yml)
    double total_processing_times_per_feature_extraction_process[features.size()];
    for (int l = 0; l < filters.size() + 1; l++) {
        total_processing_times_per_process[l] = 0.0;
    }
    for (int l = 0; l < features.size(); l++) {
        total_processing_times_per_feature_extraction_process[l] = 0.0;
    }
    auto process_start = std::chrono::system_clock::now();
    auto process_end = std::chrono::system_clock::now();

    std::string *classes;
    std::string input_directory_name = fs["input"];
    std::string output_directory_name = fs["output"];
    int num_classes = get_class_names(&classes, input_directory_name);

    // Add header to the output csv
    std::ofstream f;
    f.open (output_directory_name + "output.csv");
    FileNodeIterator it_fe = features.begin(), it_end_fe = features.end();
    for (; it_fe != it_end_fe; ++it_fe) {
        process_start = std::chrono::system_clock::now();
        std::string feature_name = (std::string) (*it_fe)["name"];
        f << feature_name << ",";
    }
    f << "class\n";

    double msqe = 0.0;

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
            imwrite(output_directory_name + images[j] + "/0.original.png", src);
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
                    msqe += uniform_quantization(&src, (*it)["step_size"]);
                } else if ("non_uniform_quantization" == filter_name) {
                    int *thresholds = new int[(*it)["thresholds"].size()];
                    for (int k = 0; k < (*it)["thresholds"].size(); k++) {
                        thresholds[k] = (*it)["thresholds"][k];
                    }
                    int *reproduction_levels = new int[(*it)["reproduction_levels"].size()];
                    for (int k = 0; k < (*it)["reproduction_levels"].size(); k++) {
                        reproduction_levels[k] = (*it)["reproduction_levels"][k];
                    }
                    non_uniform_quantization(&src, thresholds, reproduction_levels, (*it)["thresholds"].size());
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
                } else if ("histogram_equalization" == filter_name) {
                    Mat histogram;
                    apply_histogram_equalization(&src, &histogram_buckets);
                } else if ("prewitt" == filter_name) {
                    prewitt(&src);
                } else if ("sobel" == filter_name) {
                    sobel(&src);
                } else if ("improved_sobel" == filter_name) {
                    improved_sobel(&src);
                } else if ("binary_threshold" == filter_name) { // TODO: probably remove
                    binary_theshold(&src, (*it)["threshold"]);
                } else if ("histogram_threshold" == filter_name) {
                    // Determine threshold
                    int threshold = histogram_threshold_calculate(&src, &histogram_buckets);
                    // Write original histogram with selected threshold
                    Mat histogram;
                    create_histogram(&src, &histogram_buckets);
                    create_histogram_mat(&histogram, &histogram_buckets, threshold);
                    std::string file_name = "/histogram." + std::to_string(filter_number) + ".0." + filter_name + ".png";
                    imwrite(output_directory_name + images[j] + file_name, histogram);
                    histogram.release();
                    // Apply threshold
                    binary_theshold(&src, threshold);
                } else if ("histogram_k_means_clustering" == filter_name) {
                    // Determine threshold
                    int threshold = histogram_k_means_clustering(&src, &histogram_buckets, 2);
                    // Write original histogram with selected threshold
                    Mat histogram;
                    create_histogram(&src, &histogram_buckets);
                    create_histogram_mat(&histogram, &histogram_buckets, threshold);
                    std::string file_name = "/histogram." + std::to_string(filter_number) + ".k_means." + filter_name + ".png";
                    imwrite(output_directory_name + images[j] + file_name, histogram);
                    histogram.release();
                    // Apply threshold
                    binary_theshold(&src, threshold);
                } else if ("dilation" == filter_name) {
                    dilation(&src);
                } else if ("erosion" == filter_name) {
                    erosion(&src);
                } else {
                    printf("Unknown filter-name: %s\nSkipping\n\n", filter_name.c_str());
                    break;
                }

                process_end = std::chrono::system_clock::now();
                total_processing_times_per_process[filter_number] += (process_end - process_start).count();

                /*
                 * Create histogram after each action
                 */
                Mat histogram;
                create_histogram(&src, &histogram_buckets);
                for (int k = 0; k < 256; k++) {
                    histogram_bucket_sums[k] += histogram_buckets[k];
                }

                create_histogram_mat(&histogram, &histogram_buckets);
                std::string file_name = "/histogram." + std::to_string(filter_number) + "." + filter_name + ".png";
                imwrite(output_directory_name + images[j] + file_name, histogram);
                histogram.release();

                std::string output_file_name = "/" + std::to_string(filter_number) + "." + filter_name + ".png";
                imwrite(output_directory_name + images[j] + output_file_name, src);
                filter_number++;
            }

            it_fe = features.begin(), it_end_fe = features.end();
            int feature_number = 1;
            for (; it_fe != it_end_fe; ++it_fe) {
                process_start = std::chrono::system_clock::now();
                std::string feature_name = (std::string) (*it_fe)["name"];
                double feature_value = -1.0;

                if ("area" == feature_name) {
                    feature_value = (double) calculate_area(&histogram_buckets);
                } else if ("num_cells" == feature_name) {
                    feature_value = (double) number_of_cells(&src);
                } else if ("average_cell_size" == feature_name) {
                    feature_value = (double) average_cell_size(&src);
                } else if ("largest_cell" == feature_name) {
                    feature_value = (double) largest_cell_size(&src);
                } else if ("smallest_cell" == feature_name) {
                    feature_value = (double) smallest_cell_size(&src);
                } else {
                    printf("Unknown feature-name: %s\n\n", feature_name.c_str());
                }

                f << feature_value << ",";

                process_end = std::chrono::system_clock::now();
                total_processing_times_per_feature_extraction_process[feature_number] += (process_end - process_start).count();
            }
            f << i << "\n";
            f.flush();

            src.release();
        }

        // Histogram Equalization
        Mat histogram_final;
        create_histogram_mat(&histogram_final, &histogram_bucket_sums);
        imwrite(output_directory_name + classes[i] + ".AVERAGED_histogram.png", histogram_final);
        histogram_final.release();
        delete[] histogram_bucket_sums;
        delete[] images;
    }

    printf("MSQE: %lf\n", msqe);

    auto EVENT_END_program = std::chrono::system_clock::now();

    printf("total_time = %lf\n", (double) (EVENT_END_program - EVENT_START_program).count());

    FileNodeIterator it = features.begin(), it_end = features.end();
    printf("time_per_process = {\n");
    printf("'0.load_image': %lf,\n", total_processing_times_per_process[0]);
    int m = 1;
    for (; it != it_end; ++it) {
        printf("'%d.%s': %lf,\n", m, ((std::string) (*it)["name"]).c_str(), total_processing_times_per_process[m]);
        m++;
    }
    printf("}\n");

    return 0;
}
