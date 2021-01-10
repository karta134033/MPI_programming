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
    // ---

    MPI_Win win;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    long long split_tosses = tosses / world_size;
    unsigned long long *buffer = (unsigned long long *)malloc(world_size * sizeof(unsigned long long));
    MPI_Win_create(buffer, world_size * sizeof(unsigned long long), sizeof(unsigned long long), MPI_INFO_NULL, MPI_COMM_WORLD, &win);
    MPI_Win_fence(0, win);

    unsigned long long message = 0;
    unsigned seed = 1;
    for(long long end = split_tosses; end--;) 
    {
        float x = (float)rand_r(&seed) / RAND_MAX;
        float y = (float)rand_r(&seed) / RAND_MAX;
        float distance_squared = x * x + y * y;
        message = distance_squared <= 1 ? message + 1: message;
    }
    MPI_Put(&message, 1, MPI_UNSIGNED_LONG_LONG, 0, world_rank, 1, MPI_UNSIGNED_LONG_LONG, win);
    MPI_Win_fence(0, win);
    MPI_Win_free(&win);

    if (world_rank == 0)
    {
        for (int source = 0; source < world_size; source++)
            pi_result += buffer[source];
        pi_result = pi_result * 4 / tosses;

        // --- DON'T TOUCH ---
        double end_time = MPI_Wtime();
        printf("%lf\n", pi_result);
        printf("MPI running time: %lf Seconds\n", end_time - start_time);
        // ---
    }
    
    free(buffer);
    MPI_Finalize();
    return 0;
}