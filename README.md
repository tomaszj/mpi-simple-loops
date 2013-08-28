mpi-simple-loops
================

Sample C/C++ Project featuring two MPI programs working with loops.

## MPI_RunLoops

Simple worker (plain loop) code, distributing the work to free nodes. Developed in M$ Visual Studio. It needs Windows HPC Pack SDK to work.

The best performance is shown when the number of nodes is equal to number of cores + 1 (for management process). E.g.:

    > mpiexec.exe -n 9 MPI_RunLoops.exe
    
## MPI_RunLoopsScatter

This program uses MPI_Scatter to distribute work and node #0 is also a worker here. The best performance is achieved when the number of nodes is equatl to number of cores. E.g.:

    > mpiexec.exe -n 8 MPI_RunLoopsScatter.exe
    
By default, this program distributes 1000 iterations of 10 million loops. You can specify the number of iterations yourself:

    > mpiexec.exe -n 8 MPI_RunLoopsScatter.exe 30
    
This runs 30 iterations on 8 nodes, with 10 million empty loops each.
