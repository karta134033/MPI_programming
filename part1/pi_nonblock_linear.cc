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

    int dest = 0;
    int tag = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    long long split_tosses = tosses / world_size;

    if (world_rank > 0)
    {
        MPI_Request requests;
        unsigned long long message = 0;
        unsigned seed = 1;
        for(long long end = split_tosses; end--;) 
        {
            float x = (float)rand_r(&seed) / RAND_MAX;
            float y = (float)rand_r(&seed) / RAND_MAX;
            float distance_squared = x * x + y * y;
            message = distance_squared <= 1 ? message + 1: message;
        }
        MPI_Isend(&message, 1, MPI_UNSIGNED_LONG_LONG, dest, tag, MPI_COMM_WORLD, &requests);
    }
    if (world_rank == 0)
    {
        MPI_Request *requests = (MPI_Request *)malloc((world_size - 1) * sizeof(MPI_Request));
        unsigned long long *message_ptr = (unsigned long long *)malloc(world_size * sizeof(unsigned long long));
        for (int source = 1; source < world_size; source++)
            MPI_Irecv(&message_ptr[source], 1, MPI_UNSIGNED_LONG_LONG, source, tag, MPI_COMM_WORLD, &requests[source - 1]);
        
        unsigned long long message = 0;
        unsigned seed = 1;
        for(long long end = split_tosses; end--;) 
        {
            float x = (float)rand_r(&seed) / RAND_MAX;
            float y = (float)rand_r(&seed) / RAND_MAX;
            float distance_squared = x * x + y * y;
            message = distance_squared <= 1 ? message + 1: message;
        }
        pi_result += message;
        
        MPI_Waitall(world_size - 1, requests, MPI_STATUSES_IGNORE);
        for (int source = 1; source < world_size; source++) 
            pi_result += message_ptr[source];
        pi_result = pi_result * 4 / tosses;
        free(requests);
        free(message_ptr);

        // --- DON'T TOUCH ---
        double end_time = MPI_Wtime();
        printf("%lf\n", pi_result);
        printf("MPI running time: %lf Seconds\n", end_time - start_time);
        // ---
    }
    MPI_Finalize();
    return 0;
}