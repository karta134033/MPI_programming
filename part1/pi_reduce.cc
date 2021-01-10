#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    // --- DON'T TOUCH ---
    MPI_Init(&argc, &argv);
    double start_time = MPI_Wtime();
    double pi_result;
    long long int tosses = atoi(argv[1]);
    int world_rank, world_size;

    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    unsigned long long global_sum;
    unsigned long long local_sum;
    long long split_tosses = tosses / world_size;

    unsigned seed = 1;
    for(long long end = split_tosses; end--;) 
    {
        float x = (float)rand_r(&seed) / RAND_MAX;
		float y = (float)rand_r(&seed) / RAND_MAX;
        float distance_squared = x * x + y * y;
        local_sum = distance_squared <= 1 ? local_sum + 1: local_sum;
    }
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_UNSIGNED_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    if (world_rank == 0)
    {
        pi_result = (double)global_sum * 4 / tosses;
        // --- DON'T TOUCH ---
        double end_time = MPI_Wtime();
        printf("%lf\n", pi_result);
        printf("MPI running time: %lf Seconds\n", end_time - start_time);
        // ---
    }

    MPI_Finalize();
    return 0;
}
