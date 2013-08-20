// MPI_RunLoops.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <cassert>
#include <mpi.h>
#include <time.h>

#define LOOPS_COUNT 100
#define TASK_TAG 1024
#define DIE_TAG 666


using namespace std;

int process_loops(int loops);
void main_process(int world_size);
void worker_process(int rank, int world_size);

int main(int argc, char* argv[])
{
    clock_t tic = clock();

    MPI_Init(&argc, &argv);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int current_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &current_rank);

    if (current_rank == 0)
    {
        main_process(world_size);
    }
    else
    {
        worker_process(current_rank, world_size);
    }

    MPI_Finalize();
    clock_t toc = clock();

    if (current_rank == 0)
    {
        cout << "--- Operation took " << (double)(toc - tic) / CLOCKS_PER_SEC << " seconds ---" << endl;
        system("pause");
    }
	return 0;
}

void main_process(int world_size)
{
    // Coordinator process
    cout << "[Manager] Starting coordinator process" << endl;

    int sent_work_count = 0;
    int received_work_count = 0;
    long long total_processed_loops = 0;

    // Distribute initial work
    for (int i = 1; i < world_size; i++)
    {
        MPI_Send(&sent_work_count, 1, MPI_INT, i, TASK_TAG, MPI_COMM_WORLD); 
        cout << "[Manager] Sent work #" << sent_work_count << " to #" << i << endl;
            
        sent_work_count++;
    }

    // Wait for incoming responses and deal remaining work
    while (received_work_count < LOOPS_COUNT)
    {
        long processed_loops;
        MPI_Status status;
        MPI_Recv(&processed_loops, 1, MPI_LONG, MPI_ANY_SOURCE, TASK_TAG, MPI_COMM_WORLD, &status);

        received_work_count++;
        total_processed_loops += processed_loops;

        cout << "[Manager] " << processed_loops << " loops from worker #" << status.MPI_SOURCE << endl;

        if (sent_work_count < LOOPS_COUNT)
        {
            MPI_Send(&sent_work_count, 1, MPI_INT, status.MPI_SOURCE, TASK_TAG, MPI_COMM_WORLD);
            cout << "[Manager] Sent work #" << sent_work_count << " to #" << status.MPI_SOURCE << endl;

            sent_work_count++;
        }
    }

    // Kill the workers
    for (int i = 1; i < world_size; i++)
    {
        int kill_message = -1;
        MPI_Send(&kill_message, 1, MPI_INT, i, DIE_TAG, MPI_COMM_WORLD);
    }

    cout << "[Manager] Finished work with " << total_processed_loops << " processed loops" << endl;
}

void worker_process(int rank, int world_size)
{
    // Worker process.
    cout << "[Worker #" << rank << "] Started." << endl;

    while(1)
    {
        int received_data;
        MPI_Status status;
        MPI_Recv(&received_data, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        if (status.MPI_TAG == TASK_TAG)
        {
            int current_loop_id = (int)received_data;
            cout << "[Worker #" << rank << "] Received work #" << current_loop_id << endl;
            int loops_to_perform = current_loop_id * 5000000;
            long result = process_loops(loops_to_perform);

            MPI_Send(&result, 1, MPI_LONG, 0, TASK_TAG, MPI_COMM_WORLD);
            cout << "[Worker #" << rank << "] Sent " << result << " loops as work #" << current_loop_id << endl;
        }
        else if (status.MPI_TAG == DIE_TAG)
        {
            cout << "[Worker #" << rank << "] Killed." << endl;
            break;
        }
        else
        {
            // Should have discovered a proper tag already.
            MPI_Abort(MPI_COMM_WORLD, 666);
        }
    }
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