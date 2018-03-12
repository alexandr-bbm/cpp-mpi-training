#include <vector>
#include <math.h>
#include "common.h"

using namespace std;

_v<_v_d> get_cluster_centers(_v<_v_d> data, _v<_v_int> ids_by_clusters) {
    unsigned long clusters_num = ids_by_clusters.size();
    _v<_v_d> cluster_centers;

    for (int cluster_id = 0; cluster_id < clusters_num; cluster_id++) {
        _v_int cluster_data_ids = ids_by_clusters[cluster_id];
        _v_d cluster_vectors_sum(data[0].size(), 0);

        for (auto cluster_data_id : cluster_data_ids) {
            cluster_vectors_sum = sum_vectors(cluster_vectors_sum, data[cluster_data_id]);
        }
        _v_d cluster_center = multiply_vector(cluster_vectors_sum, 1. / cluster_data_ids.size());
        cluster_centers.emplace_back(cluster_center);
    }

    return cluster_centers;
}

_v<_v_int> get_unique_combinations(unsigned long max_value) {
    _v<_v_int> result;
    for (int i = 0; i < max_value; i++) {
        for (int j = i; j < max_value; j++) {
            _v_int combination = {i, j};
            result.emplace_back(combination);
        }
    }
    return result;
}

double get_norm(_v_d vec) {
    double sum_of_squares = 0;
    for (auto vec_i : vec) {
        sum_of_squares += vec_i * vec_i;
    }
    return sqrt(sum_of_squares);
}

_v_d get_all_distances(_v<_v_d> vectors) {
    unsigned long num_of_clusters = vectors.size();
    _v_d all_distances;

    _v<_v_int> combinations = get_unique_combinations(num_of_clusters);

    for (auto combination: combinations) {
        int i = combination[0];
        int j = combination[1];
        if (i == j) continue;
        _v_d difference_vector = substract_vectors(vectors[i], vectors[j]);
        double norm = get_norm(difference_vector);
        all_distances.emplace_back(norm);
    }

    return all_distances;
}

double get_dist(_v<_v_d> cluster_centers) {
    _v_d all_distances = get_all_distances(cluster_centers);

    auto begin = all_distances.begin();
    auto end = all_distances.end();

    auto max_idx = distance(begin, max_element(begin, end));
    auto min_idx = distance(begin, min_element(begin, end));

    double fraction = all_distances[max_idx] / all_distances[min_idx];

    unsigned long clusters_num = cluster_centers.size();

    double outer_sum = 0;
    for (int i = 0; i < clusters_num; i++) {
        double inner_sum = 0;
        for (int j = 0; j < clusters_num; j++) {
            if (i == j) continue;
            _v_d diff_vec = substract_vectors(cluster_centers[i], cluster_centers[j]);
            double norm = get_norm(diff_vec);
            inner_sum += norm;
        }
        outer_sum += 1. / inner_sum;
    }

    return fraction * outer_sum;
}

_v_d get_center_vector(_v<_v_d> data) {
    unsigned long data_count = data.size();
    unsigned long dim = data[0].size();
    _v_d center_vector(dim, 0);
    for (auto item : data) {
        center_vector = sum_vectors(center_vector, item);
    }
    return multiply_vector(center_vector, 1. / data_count);
};

_v_d get_dispersion(_v<_v_d> data, _v_d vec) {
    unsigned long n = data.size();
    unsigned long dim = data[0].size();

    _v_d data_dispersion(dim, 0);

    for (int p = 0; p < dim; p++) {
        double sum = 0;
        for (int k = 0; k < n; k++) {
            sum += pow(data[k][p] - vec[p], 2);
        }
        data_dispersion[p] = sum / n;
    }
    return data_dispersion;
};

_v_d get_data_dispersion(_v<_v_d> data) {
    _v_d center_vector = get_center_vector(data);
    return get_dispersion(data, center_vector);
};

_v<_v_d> get_clusters_dispersions(_v<_v_d> data, _v<_v_int> ids_by_clusters, _v<_v_d> cluster_centers) {
    unsigned long dim = data[0].size();
    unsigned long clusters_num = ids_by_clusters.size();

    _v<_v_d> clusters_dispersions(clusters_num, _v_d(dim, 0));

    for (int i = 0; i < clusters_num; i++) {
        unsigned long data_count_for_cluster = ids_by_clusters[i].size();

        if (data_count_for_cluster == 0) continue;

        _v<_v_d> cluster_data(data_count_for_cluster, _v_d(0));
        for (int j = 0; j < data_count_for_cluster; j++) {
            cluster_data[j] = data[ids_by_clusters[i][j]];
        }
        clusters_dispersions[i] = get_dispersion(cluster_data, cluster_centers[i]);
    }
    return clusters_dispersions;
};

double get_scatt(_v<_v_d> data, _v<_v_int> ids_by_clusters, _v<_v_d> cluster_centers) {
    _v_d data_dispersion = get_data_dispersion(data);
    _v<_v_d> clusters_dispersions = get_clusters_dispersions(data, ids_by_clusters, cluster_centers);

    unsigned long num_of_clusters = ids_by_clusters.size();

    double sum = 0;
    double data_dispersion_norm = get_norm(data_dispersion);
    for (int i = 0; i < num_of_clusters; i++) {
        sum += get_norm(clusters_dispersions[i]) / data_dispersion_norm;
    }

    return sum / num_of_clusters;
}

_v<_v_int> get_filtered_ids_by_clusters(_v<_v_int> ids_by_clusters) {
    _v<_v_int> filtered_ids_by_clusters;
    for (auto ids: ids_by_clusters) {
        if (ids.size() == 0) continue;
        filtered_ids_by_clusters.emplace_back(ids);
    }
    return filtered_ids_by_clusters;
}


double get_SD_index(_v<_v_d> data, _v<_v_int> ids_by_clusters) {
    double alpha = 1;
    _v<_v_int> filtered_ids_by_clusters = get_filtered_ids_by_clusters(ids_by_clusters);
    _v<_v_d> cluster_centers = get_cluster_centers(data, filtered_ids_by_clusters);

    double scatt = get_dist(cluster_centers);
    double dist = get_scatt(data, filtered_ids_by_clusters, cluster_centers);

    return alpha * scatt + dist;
}