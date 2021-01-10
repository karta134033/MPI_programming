#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>

int main(int argc, char **argv)
{
    // --- DON'T TOUCH ---
    MPI_Init(&argc, &argv);
    double start_time = MPI_Wtime();
    double pi_result;
    long long int tosses = atoi(argv[1]);
    int world_rank, world_size;
    // ---
    
    unsigned long long message;
    int tag = 0;
    MPI_Status status;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int level = (int)(log(world_size) / log(2));
    long long split_tosses = tosses / world_size;
    
    unsigned long long  curr_number_in_circle = 0; // current process's number_in_circle
    unsigned seed = 1;
    for(long long end = split_tosses; end--;) 
    {
        float x = (float)rand_r(&seed) / RAND_MAX;
		float y = (float)rand_r(&seed) / RAND_MAX;
        float distance_squared = x * x + y * y;
        curr_number_in_circle = distance_squared <= 1 ? curr_number_in_circle + 1: curr_number_in_circle;
    }

    int base = 1;
    int parent_num = 0;
    for (int i = 1; i <= level; i++)  // This decides who is sender who is receiver, according to the current level of tree
    {
        int stride = pow(2, i);
        for (int j = base; j < world_size; j += stride) 
        {   
            if (j == world_rank) 
            {
                int dest = j - base;
                message = curr_number_in_circle;
                MPI_Send(&message, 1, MPI_UNSIGNED_LONG_LONG, dest, tag, MPI_COMM_WORLD);
                break;
            }
        }
        for (int j = 0; j < world_size; j += stride) 
        {   
            if (j == world_rank) 
            {
                MPI_Recv(&message, 1, MPI_UNSIGNED_LONG_LONG, j + base, tag, MPI_COMM_WORLD, &status);
                curr_number_in_circle += message;
                pi_result = curr_number_in_circle;
                break;
            }
        }
        base *= 2;
    }


    if (world_rank == 0)
    {
        // TODO: PI result
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