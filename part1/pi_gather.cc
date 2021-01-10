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
    unsigned long long *global_message = NULL;
    unsigned long long local_message;
    long long split_tosses = tosses / world_size;

    if (world_rank == 0) 
        global_message = (unsigned long long *)malloc(world_size * sizeof(unsigned long long));

    MPI_Scatter(global_message, 1, MPI_UNSIGNED_LONG_LONG, &local_message, 1, MPI_UNSIGNED_LONG_LONG, 0, MPI_COMM_WORLD);
    unsigned seed = 1;
    local_message = 0;
    for(long long end = split_tosses; end--;) 
    {
        float x = (float)rand_r(&seed) / RAND_MAX;
		float y = (float)rand_r(&seed) / RAND_MAX;
        float distance_squared = x * x + y * y;
        local_message = distance_squared <= 1 ? local_message + 1: local_message;
    }
    MPI_Gather(&local_message, 1, MPI_UNSIGNED_LONG_LONG, global_message, 1, MPI_UNSIGNED_LONG_LONG, 0, MPI_COMM_WORLD);


    if (world_rank == 0)
    {
        for (int source = 0; source < world_size; source++)
            pi_result += global_message[source];
        pi_result = pi_result * 4 / tosses;

        // --- DON'T TOUCH ---
        double end_time = MPI_Wtime();
        printf("%lf\n", pi_result);
        printf("MPI running time: %lf Seconds\n", end_time - start_time);
        // ---
    }
    
    free(global_message);
    MPI_Finalize();
    return 0;
}