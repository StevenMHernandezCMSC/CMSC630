#include <iostream>
#include <fstream>
#include <random>
#include "opencv2/core/ocl.hpp"

using namespace cv;

struct matrix_sizes {
    int num_features;
    int num_rows;
};

struct matrix_sizes
load_features_csv(FileStorage fs, std::string output_directory_name, double ***features_matrix, int **classes_matrix) {
    int num_features = 0;
    int num_rows = 0;

    //
    // Count number of features
    //
    FileNode features = fs["features"];
    FileNodeIterator it_fe = features.begin(), it_end_fe = features.end();
    for (; it_fe != it_end_fe; ++it_fe) {
        num_features++;
    }

    //
    // Count number of lines
    //
    std::ifstream f;
    f.open(output_directory_name + "output.csv");
    for (std::string line; getline(f, line);) {
        if (!line.empty()) {
            num_rows++;
        }
    }
    f.close();

    //
    // Create feature-matrix and class-matrix
    //
    auto **_features_matrix = new double *[num_rows];
    for (int i = 0; i < num_rows; i++) {
        _features_matrix[i] = new double[num_features];
    }
    int *_classes_matrix = new int[num_rows];

    //
    // Select random shuffled order to store data.
    //
    std::vector<int> indices;
    indices.reserve(num_rows - 1);
    for (int i = 0; i < num_rows - 1; i++) {
        indices.push_back(i);
    }
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    shuffle(indices.begin(), indices.end(), std::default_random_engine(seed));

    //
    // Load CSV rows into matrix
    //
    f.open(output_directory_name + "output.csv");
    std::string line;
    getline(f, line); // ignore header
    int line_i = 0;
    for (; getline(f, line);) {
        int m_i = indices.at(line_i);

        std::istringstream ss(line);

        int feature_i = 0;
        while (ss) {
            std::string s;
            if (!getline(ss, s, ',')) break;
            if (feature_i < num_features) {
                _features_matrix[m_i][feature_i] = stod(s);
            } else {
                _classes_matrix[m_i] = stoi(s);
            }
            feature_i++;
        }

        line_i++;
    }
    f.close();
    num_rows--; // ignore header row now.

    *features_matrix = _features_matrix;
    *classes_matrix = _classes_matrix;

    return matrix_sizes{
            num_features = num_features,
            num_rows = num_rows
    };
}