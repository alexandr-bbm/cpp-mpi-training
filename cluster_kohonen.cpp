#include <cstdio>
#include "common.h"

using namespace std;

double calculate_distance(_v_d a, _v_d b, int dimension) {
    double distance = 0;

    for (int i = 0; i < dimension; i++)
        distance += (a[i] - b[i]) * (a[i] - b[i]);

    return distance;
}

_v<_v_int> cluster_kohonen(_v<_v_d> data, int clusters_num, int max_data_value) {
    double training_speed = 0.7;
    _v<_v_d> weights;
    double distance[clusters_num];
    unsigned long patterns_num = data.size();
    unsigned long dimension = data[0].size();
    int selected_vector_ID, winner_ID;

    // Set random weights
    for(int i = 0; i < clusters_num; i++) {
        weights.emplace_back();
        for(int j = 0; j < dimension; j++) {
            // changed line to generate random numbers from 0 to 1 (coressponding to data set);
            double rand_value = ((double) rand() / (RAND_MAX)) * max_data_value;
            weights[i].emplace_back(rand_value);
        }
    }

    for(int iteration_ID = 0; iteration_ID < 1000; iteration_ID++) {
        for (int j = 0; j < clusters_num; j++)
            distance[j] = 0;

        selected_vector_ID = rand() % patterns_num;
        winner_ID = 0;
        // changed line: move duplicated code to function `calculate_distance`
        distance[winner_ID] = calculate_distance(data[selected_vector_ID], weights[winner_ID], dimension);

        for (int j = 1; j < clusters_num; j++) {
            // changed line: weights[winner_ID] --> weights[j]
            // changed line: move duplicated code to function `calculate_distance`
            distance[j] = calculate_distance(data[selected_vector_ID], weights[j], dimension);

            // changed sign: < --> >
            if (distance[winner_ID] > distance[j])
                winner_ID = j;
        }
        for (int i = 0; i < dimension; i++)
            weights[winner_ID][i] += training_speed *
                                     (data[selected_vector_ID][i] - weights[winner_ID][i]);
    }

    // create clustering results
    _v<_v_int> data_ids_by_clusters;
    for (int j = 0; j < clusters_num; j++) {
        data_ids_by_clusters.emplace_back();
    }

    for (int i = 0; i < patterns_num; i++) {
        double min_distance = numeric_limits<double>::max();
        int min_distance_cluster_idx = 0;
        for (int j = 0; j < clusters_num; j++) {
            distance[j] = calculate_distance(data[i], weights[j], dimension);
            if (distance[j] < min_distance) {
                min_distance = distance[j];
                min_distance_cluster_idx = j;
            }
        }
        data_ids_by_clusters[min_distance_cluster_idx].emplace_back(i);
    }

    return data_ids_by_clusters;
}
