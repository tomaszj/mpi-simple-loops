// MPI_RunLoops.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <cassert>
#include <mpi.h>

#define DEFAULT_ITERATIONS_COUNT 1000
#define TASK_TAG 1024

using namespace std;

int process_loops(int loops);
void perform_process(int total_iterations, int current_rank, int world_size);

int main(int argc, char* argv[])
{
	int total_iterations = 0;
 
	if (argc == 2)
	{
		char *input = argv[1];
		total_iterations = strtol(input, NULL, 10);
	}
	else
	{
		total_iterations = DEFAULT_ITERATIONS_COUNT;
	}

	MPI_Init(&argc, &argv);

	double tic = MPI_Wtime();

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int current_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &current_rank);

	perform_process(total_iterations, current_rank, world_size);

	double toc = MPI_Wtime();
    MPI_Finalize();

    if (current_rank == 0)
    {
        cout << "--- Operation took " << (toc - tic) << " seconds ---" << endl;
        system("pause");
    }
	return 0;
}

void perform_process(int total_iterations, int current_rank, int world_size)
{
    // Start of process
	cout << "[Node #" << current_rank << "] Starting process" << endl;

	int *iterations_distribution = new int[world_size];

	if (current_rank == 0)
	{
		int iterations_left = total_iterations;
		int iterations_per_worker = total_iterations / world_size;
	
		// Fill in a number of loops except for the last worker
		for (int i = 0; i < world_size - 1; i++)
		{
			iterations_distribution[i] = iterations_per_worker;
			iterations_left -= iterations_per_worker;
		}

		// Last worker gets the remaining loops to do
		iterations_distribution[world_size - 1] = iterations_left;
	
		cout << "[Node #0] Base nodes iterations: " << iterations_per_worker << " last node: " << iterations_left << endl;
	}
	
	// Distribute and perform the work
	int iterations;
	MPI_Scatter(iterations_distribution, 1, MPI_INT, &iterations, 1, MPI_INT, 0, MPI_COMM_WORLD);

	cout << "[Node #" << current_rank << "] Got " << iterations << " iterations to perform." << endl;

	// Perform the work and collect the result
	long long total_loops = 0;
	for (int i = 0; i < iterations; i++)
	{
        int loops_to_perform = 10000000;
        long result = process_loops(loops_to_perform);
		total_loops += result;
    }

	// Gather incoming responses
	long long *received_loops = new long long[world_size];
	MPI_Gather(&total_loops, 1, MPI_LONG_LONG, received_loops, 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);
    
	if (current_rank == 0)
	{
		long long total_processed_loops = 0;
		for (int i = 0; i < world_size; i++)
		{
			total_processed_loops += received_loops[i];
		}

		cout << "[Node #0] Finished work with " << total_processed_loops << " processed loops" << endl;
	}

	cout << "[Node #" << current_rank << "] dies." << endl;

	// Clean up
	delete[] iterations_distribution;
	delete[] received_loops;
}

int process_loops(int loops)
{
    int i;
    for (i = 0; i < loops; i++)
    {
        i++;
    }

    return i;
}