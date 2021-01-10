#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

void print_matrix(long long *matrix_c, long long n, long long l) {
	for (long long i = 0; i < n; i++) {
		for (long long j = 0; j < l; j++) {
			printf("%lld ", matrix_c[i * l + j]);
		}
		printf("\n");
	}
}

void matrix_multiplication(
	long long *matrix_a, long long *matrix_b, long long *matrix_c, 
	int world_rank, int world_size, long long n, long long m, long long l
) {
	for (long long i = world_rank; i < n; i += world_size) {
		for (long long j = 0; j < l; j++) {
			long long res = 0;
			for (long long k = 0; k < m; k++)
				res += matrix_a[i * m + k] * matrix_b[k * l + j];
			matrix_c[i * l + j] = res;
		}
	}
}

int main() {
	char input[50];
	char *end;
	long long n = 0, m = 0, l = 0;
	long long matrix_a_size;
	long long matrix_b_size;
	long long matrix_a_index = 0;
	long long matrix_b_index = 0;
	long long *matrix_a;  // n * m
	long long *matrix_b;  // m * l
	long long *matrix_c;  // n * l
	while(scanf("%s", input) != EOF) {
		if (n == 0)
			n = strtoll(input, &end, 10);
		else if (m == 0) {
			m = strtoll(input, &end, 10);
			matrix_a_size = n * m;
			matrix_a = (long long *)malloc(matrix_a_size * sizeof(long long));
			printf("matrix_a_size: %lld\n", matrix_a_size);
		}
		else if (l == 0) {
			l = strtoll(input, &end, 10);
			matrix_b_size = m * l;
			matrix_b = (long long *)malloc(matrix_b_size * sizeof(long long));
			printf("matrix_b_size: %lld\n", matrix_b_size);
		}
		else {
			if (matrix_a_size-- > 0)
				matrix_a[matrix_a_index++] = strtoll(input, &end, 10);
			else if (matrix_b_size-- > 0) 
				matrix_b[matrix_b_index++] = strtoll(input, &end, 10);
		}
	}
	fflush(stdin);
	matrix_c = (long long *)malloc(n * l * sizeof(long long));

	MPI_Init(NULL, NULL);
	double start_time = MPI_Wtime();
	int world_rank, world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	if (world_rank == 0) {
		int i = world_size;
		while(i-- > 1) {  // send matrix's information to workers
			MPI_Send(&n, 1, MPI_LONG_LONG, i, 0, MPI_COMM_WORLD);
			MPI_Send(&m, 1, MPI_LONG_LONG, i, 1, MPI_COMM_WORLD);
			MPI_Send(&l, 1, MPI_LONG_LONG, i, 2, MPI_COMM_WORLD);
			MPI_Send(matrix_a, n * m, MPI_LONG_LONG, i, 3, MPI_COMM_WORLD);
			MPI_Send(matrix_b, m * l, MPI_LONG_LONG, i, 4, MPI_COMM_WORLD);
			MPI_Send(matrix_c, n * l, MPI_LONG_LONG, i, 5, MPI_COMM_WORLD);
		}
		matrix_multiplication(matrix_a, matrix_b, matrix_c, world_rank, world_size, n, m, l);

		for (int source = 1; source < world_size; source++) {  // merge result
			long long *temp_matrix_c = (long long *)malloc(n * l * sizeof(long long));
			MPI_Recv(temp_matrix_c, n * l, MPI_LONG_LONG, source, 6, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
			for (long long i = source; i < n; i += world_size) {
				for (long long j = 0; j < l; j++) {
					matrix_c[i * l + j] = temp_matrix_c[i * l + j];
				}
			}
			free(temp_matrix_c);
		}

		print_matrix(matrix_c, n, l);
		free(matrix_a);
		free(matrix_b);
		free(matrix_c);
	} else {
		long long *matrix_a;
		long long *matrix_b;
		long long *matrix_c;
		long long n;
		long long m;
		long long l;
		MPI_Recv(&n, 1, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
		MPI_Recv(&m, 1, MPI_LONG_LONG, 0, 1, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
		MPI_Recv(&l, 1, MPI_LONG_LONG, 0, 2, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
		matrix_a = (long long *)malloc(n * m * sizeof(long long));
		matrix_b = (long long *)malloc(m * l * sizeof(long long));
		matrix_c = (long long *)malloc(n * l * sizeof(long long));
		MPI_Recv(matrix_a, n * m, MPI_LONG_LONG, 0, 3, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
		MPI_Recv(matrix_b, m * l, MPI_LONG_LONG, 0, 4, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
		MPI_Recv(matrix_c, n * l, MPI_LONG_LONG, 0, 5, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
		matrix_multiplication(matrix_a, matrix_b, matrix_c, world_rank, world_size, n, m, l);
		
		MPI_Send(matrix_c, n * l, MPI_LONG_LONG, 0, 6, MPI_COMM_WORLD);
		free(matrix_a);
		free(matrix_b);
		free(matrix_c);
	}
	MPI_Finalize();
	double end_time = MPI_Wtime();
    printf("MPI running time: %lf Seconds\n", end_time - start_time);
	return 0;
}