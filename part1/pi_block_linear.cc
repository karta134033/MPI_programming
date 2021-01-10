#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv)
{
    // --- DON'T TOUCH ---
    MPI_Init(&argc, &argv);
    double start_time = MPI_Wtime();
    double pi_result;
    long long int tosses = atoi(argv[1]);
    int world_rank, world_size;
    // ---

    int dest = 0;
    int tag = 0;
    MPI_Status status;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    long long split_tosses = tosses / world_size;

    unsigned long long message = 0;
    unsigned seed = 1;
    for(long long end = split_tosses; end--;) 
    {
        float x = (float)rand_r(&seed) / RAND_MAX;
		float y = (float)rand_r(&seed) / RAND_MAX;
        float distance_squared = x * x + y * y;
        message = distance_squared <= 1 ? message + 1: message;
    }

    if (world_rank > 0)
        MPI_Send(&message, 1, MPI_UNSIGNED_LONG_LONG, dest, tag, MPI_COMM_WORLD);

    if (world_rank == 0)
    {
        pi_result += message;
        for (int source = 1; source < world_size; source++) 
        {
            MPI_Recv(&message, 1, MPI_UNSIGNED_LONG_LONG, source, tag, MPI_COMM_WORLD, &status);
            pi_result += message;
        }
        pi_result = pi_result * 4 / tosses;
        // --- DON'T TOUCH ---
        double end_time = MPI_Wtime();
        printf("%lf\n", pi_result);
        printf("MPI running time: %lf Seconds\n", end_time - start_time);
        // ---
    }

    MPI_Finalize();
    return 0;
}
