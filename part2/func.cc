#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#define THRESHOLD 2900000000
int *c_mat;
int world_rank, world_size;

void construct_matrices(
    int *n_ptr, int *m_ptr, int *l_ptr,
    int **a_mat_ptr, int **b_mat_ptr
) {
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    if (world_rank == 0) {
        char input[10];
        int matrix_a_index = 0;
        int matrix_b_index = 0;
        int i = world_size;
        *n_ptr = 0;
        *m_ptr = 0;
        *l_ptr = 0;

        while(scanf("%s", input) != EOF)
            if (*n_ptr == 0)
                *n_ptr = atoi(input);
            else if (*m_ptr == 0)
                *m_ptr = atoi(input);
            else if (*l_ptr == 0) {
                *l_ptr = atoi(input);
                break;
            }

        while(i-- > 1) {  // send matrix's information to workers
            MPI_Request *requests = (MPI_Request *)malloc(3 * sizeof(MPI_Request));
			MPI_Isend(n_ptr, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &requests[0]);
			MPI_Isend(m_ptr, 1, MPI_INT, i, 1, MPI_COMM_WORLD, &requests[1]);
			MPI_Isend(l_ptr, 1, MPI_INT, i, 2, MPI_COMM_WORLD, &requests[2]);
            free(requests);
        }
        int matrix_a_size = *n_ptr * *m_ptr;
        int matrix_b_size = *m_ptr * *l_ptr;
        *a_mat_ptr = (int *)malloc(matrix_a_size * sizeof(int));
        *b_mat_ptr = (int *)malloc(matrix_b_size * sizeof(int));
        c_mat = (int *)calloc(*n_ptr * *l_ptr, sizeof(int));
        
        while(scanf("%s", input) != EOF)
            if (matrix_a_size-- > 0)
                (*a_mat_ptr)[matrix_a_index++] = atoi(input);
            else if (matrix_b_size-- > 0) 
                (*b_mat_ptr)[matrix_b_index++] = atoi(input);
        
        fflush(stdin);
        long long threshold = (long long)*n_ptr * *m_ptr * *l_ptr;
        if (threshold < THRESHOLD) return;
        
		i = 1;
        int split_n = *n_ptr / world_size;
        int remain = *n_ptr % world_size;
        int begin = (split_n + remain) * *m_ptr;
		while(i < world_size) {  // send matrix's information to workers
            MPI_Request *requests = (MPI_Request *)malloc(2 * sizeof(MPI_Request));
			MPI_Isend(*a_mat_ptr + begin, split_n * *m_ptr, MPI_INT, i, 3, MPI_COMM_WORLD, &requests[0]);
			MPI_Isend(*b_mat_ptr, *m_ptr * *l_ptr, MPI_INT, i, 4, MPI_COMM_WORLD, &requests[1]);
            begin += split_n * *m_ptr;
            i++;
            free(requests);
		}
	}
    else {
        MPI_Recv(n_ptr, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
        MPI_Recv(m_ptr, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
        MPI_Recv(l_ptr, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
        long long threshold = (long long)*n_ptr * *m_ptr * *l_ptr;
        if (threshold < THRESHOLD) return;
        
        int split_n = *n_ptr / world_size;
        *a_mat_ptr = (int *)malloc(split_n * *m_ptr * sizeof(int));
        *b_mat_ptr = (int *)malloc(*m_ptr * *l_ptr * sizeof(int));
        c_mat = (int *)calloc(split_n * *l_ptr, sizeof(int));
        MPI_Recv(*a_mat_ptr, split_n * *m_ptr, MPI_INT, 0, 3, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
        MPI_Recv(*b_mat_ptr, *m_ptr * *l_ptr, MPI_INT, 0, 4, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
    }
}

void matrix_multiply(
    const int n, const int m, const int l,
    const int *a_mat, const int *b_mat
) {

    int split_n = n / world_size;
    long long threshold = (long long)n * m * l;
    if (threshold < THRESHOLD) {
        if (world_rank == 0) {
            for (register int i = 0; i < n; i++)
                for (register int k = 0; k < m; k++)
                    for (register int j = 0; j < l; j++) 
                        c_mat[i * l + j] += a_mat[i * m + k] * b_mat[k * l + j];

            for (register int i = 0; i < n; i++) {
                for (register int j = 0; j < l - 1; j++)
                    printf("%d ", c_mat[i * l + j]);
                printf("%d\n", c_mat[i * l + l - 1]);
            }
        }
        return;
    }

    if (world_rank == 0) {
        int remain = n % world_size;
        int begin = (split_n + remain) * l;

        MPI_Request *requests = (MPI_Request *)malloc((world_size - 1) * sizeof(MPI_Request));
        for (register int source = 1; source < world_size; source++) {  // merge result
			MPI_Irecv(c_mat + begin, split_n * l, MPI_INT, source, 6, MPI_COMM_WORLD, &requests[source - 1]);
            begin += split_n * l;
		}

        int wr0_end = split_n + remain;
        for (register int i = 0; i < wr0_end; i++)
            for (register int k = 0; k < m; k++)
                for (register int j = 0; j < l; j++) 
                    c_mat[i * l + j] += a_mat[i * m + k] * b_mat[k * l + j];

        for (register int i = 0; i < wr0_end; i++) {
            for (register int j = 0; j < l - 1; j++)
                printf("%d ", c_mat[i * l + j]);
            printf("%d\n", c_mat[i * l + l - 1]);
        }
        MPI_Waitall(world_size - 1, requests, MPI_STATUSES_IGNORE);
        
        for (register int i = split_n + remain; i < n; i++) {
            for (register int j = 0; j < l - 1; j++)
                printf("%d ", c_mat[i * l + j]);
            printf("%d\n", c_mat[i * l + l - 1]);
        }
        free(requests);
        free(c_mat);
    }
    else {
        MPI_Request requests;
        for (register int i = 0; i < split_n; i++)
            for (register int k = 0; k < m; k++)
                for (register int j = 0; j < l; j++) 
                    c_mat[i * l + j] += a_mat[i * m + k] * b_mat[k * l + j];
        MPI_Isend(c_mat, split_n * l, MPI_INT, 0, 6, MPI_COMM_WORLD, &requests);
    }
}

void destruct_matrices(int *a_mat, int *b_mat) {
    if (world_rank == 0) {
        free(a_mat);
        free(b_mat);
    }
}