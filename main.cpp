#include <mpi.h>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include "get_sd_index.h"
#include "cluster_kohonen.h"

#define DATA_FILE_PATH "/Users/alexandr.bbm/Google Drive/university/parallel_programming/lab2/data.csv"
const int MAX_DATA_VALUE = 1;
const int CLUSTERS_NUM = 3;

using namespace std;

_v<_v_d> read_data_from_csv() {
    _v<_v_d> data;
    ifstream file(DATA_FILE_PATH);
    string line;

    int lineN = 0;
    while(getline(file,line))
    {
        data.emplace_back();
        stringstream lineStream(line);
        string cell;
        while(getline(lineStream,cell,','))
        {

            data[lineN].emplace_back(atof(cell.c_str()));
        }
        lineN++;
    }
    return data;
}

double cluster_and_get_index(_v<_v_d> data) {
    _v<_v_int> data_ids_by_clusters = cluster_kohonen(data, CLUSTERS_NUM, MAX_DATA_VALUE);
    return get_SD_index(data, data_ids_by_clusters);
}

int main() {
    MPI_Init(NULL, NULL);

    int processes_num;
    MPI_Comm_size(MPI_COMM_WORLD, &processes_num);

    int process_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);

    _v<_v_d> data;

    // transmitting variables
    unsigned long data_size;
    unsigned long data_vector_size;
    struct double_int { double value; int rank; };
    struct double_int clustering_index_and_rank;
    struct double_int best_clustering_index_and_rank;
    double data_cell;

    int root = 0;

    double start, end;

    // We have to seed random number generator.
    srand(time(NULL) + process_rank);

    MPI_Barrier(MPI_COMM_WORLD); /* IMPORTANT */
    start = MPI_Wtime();

    if (process_rank == root) {
        data = read_data_from_csv();
        data_size = data.size();
        data_vector_size = data[0].size();

        double clustering_indexes[processes_num - 1];

        for (int rank = 1; rank < processes_num; rank++) {
            MPI_Send(&data_size, 1, MPI_INT, rank, 0, MPI_COMM_WORLD);
            MPI_Send(&data_vector_size, 1, MPI_INT, rank, 0, MPI_COMM_WORLD);

            for (int i = 0; i < data.size(); i ++) {
                for (int j = 0; j < data[i].size(); j ++) {
                    MPI_Send(&data[i][j], 1, MPI_DOUBLE, rank, 0, MPI_COMM_WORLD);
                }
            }
            MPI_Recv(&clustering_index_and_rank, 1, MPI_DOUBLE_INT, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            cout << clustering_index_and_rank.value << endl;
         }
    } else {
        MPI_Recv(&data_size, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&data_vector_size, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        data = _v<_v_d>(data_size, _v_d(data_vector_size));
        for (int i = 0; i < data_size; i ++) {
            for (int j = 0; j < data_vector_size; j ++) {
                MPI_Recv(&data_cell, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                data[i][j] = data_cell;
            }
        }
        clustering_index_and_rank = {cluster_and_get_index(data), process_rank};
        MPI_Send(&clustering_index_and_rank, 1, MPI_DOUBLE_INT, root, 0, MPI_COMM_WORLD);
    }

    MPI_Reduce(&clustering_index_and_rank, &best_clustering_index_and_rank, 1, MPI_DOUBLE_INT, MPI_MINLOC, 0,
               MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD); /* IMPORTANT */
    end = MPI_Wtime();

    if (process_rank == root) {
        printf("Runtime = %f\n", end - start);
        printf("Best (min) clustering index = %f\n", best_clustering_index_and_rank.value);
        printf("Best (min) clustering index process rank = %d\n", best_clustering_index_and_rank.rank);
    }

    MPI_Finalize();
}
