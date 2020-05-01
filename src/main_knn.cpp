#include <random>
#include <algorithm>
#include "opencv2/core/ocl.hpp"
#include "dataset/load_features_csv.cpp"
#include "classification/knn.cpp"

using namespace cv;

int main(int argc, char **argv) {
    std::string filename = argv[1];
    FileStorage fs(filename, FileStorage::READ);
    fs.open(filename, FileStorage::READ);

    std::string input_directory_name = fs["input"];
    std::string output_directory_name = fs["output"];

    FileNode classifier = fs["classifier"];
    FileNodeIterator it_fe = classifier.begin();
    int k = (int) (*it_fe)["k"];

    double **features_matrix;
    int *classes_matrix;
    matrix_sizes ms = load_features_csv(fs, output_directory_name, &features_matrix, &classes_matrix);

    //
    // Normalize columns between 0 and 1
    //

    // Find max and min values per feature
    auto *min_values = new double[ms.num_features];
    auto *max_values = new double[ms.num_features];
    for (int i = 0; i < ms.num_rows; i++) {
        for (int j = 0; j < ms.num_features; j++) {
            min_values[j] = min(features_matrix[i][j], min_values[j]);
            max_values[j] = max(features_matrix[i][j], max_values[j]);
        }
    }
    // Normalize each feature based on max and min values.
    for (int i = 0; i < ms.num_rows; i++) {
        for (int j = 0; j < ms.num_features; j++) {
            features_matrix[i][j] = (features_matrix[i][j] - min_values[j]) / (max_values[j] - min_values[j]);
        }
    }

    //
    // 10-fold cross validation
    //
    int num_folds = 10;
    std::vector<int> indices;
    indices.reserve(ms.num_rows);
    for (int i = 0; i < ms.num_rows; i++) {
        indices.push_back(i);
    }
    double accuracy_sum = 0.0;
    for (int f = 0; f < num_folds; f++) {
        // Shift datasets for next fold.
        int fold_length = (int) ((double) indices.size() / (double) num_folds);
        std::rotate(indices.begin(), indices.begin() + fold_length, indices.end());

        double accuracy = knn(k, features_matrix, classes_matrix, indices, 0, indices.size() - fold_length, indices.size(), ms.num_features);
        printf("Accuracy: %lf\n", accuracy);
        accuracy_sum += accuracy;
    }

    printf("Final Accuracy: %lf\n", accuracy_sum / num_folds);
}