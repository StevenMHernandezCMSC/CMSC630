#include <queue>
#include <utility>

// TODO: calculate this instead of hard-coding
#define NUM_CLASSES 6

double knn(int k, double **features_matrix, int *classes_matrix, std::vector<int> indices, int start_training_data,
           unsigned long start_prediction_data, unsigned long data_length, int num_features) {
    int num_calculated = 0;
    int num_correct = 0;

    //
    // For each row to predict on
    //
    for (int i = start_prediction_data; i < data_length; i++) {
        num_calculated++;
        int true_class = classes_matrix[indices[i]];

        // Store k-nearest neighbors based on euclidean distance
        std::priority_queue<std::pair<double, int>> q;

        //
        // Find nearest neighbors from each "training" row
        //
        for (int j = start_training_data; j < start_prediction_data; j++) {
            double euclidean_distance = 0.0;
            for (int m = 0; m < num_features; m++) {
                euclidean_distance += pow(features_matrix[indices[i]][m] - features_matrix[indices[j]][m], 2.0);
            }

            if (q.size() < k) {
                q.push(std::make_pair(euclidean_distance, j));
            } else {
                if (q.top().first > euclidean_distance) {
                    q.pop();
                    q.push(std::make_pair(euclidean_distance, j));
                }
            }
        }

        // Make prediction
        int *selections = new int[NUM_CLASSES];
        for (int j = 0; j < NUM_CLASSES; j++) {
            selections[j] = 0;
        }
        while (!q.empty()) {
            selections[classes_matrix[indices[q.top().second]]]++;
            q.pop();
        }
        int predicted_class = 0;
        for (int j = 0; j < k; j++) {
            if (selections[j] > selections[predicted_class]) {
                predicted_class = j;
            }
        }

        // Increment accuracy
        if (predicted_class == true_class) {
            num_correct++;
        }
    }

    return ((double) num_correct) / ((double) num_calculated);
}