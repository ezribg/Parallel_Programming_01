#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h" // message passing interface
using namespace std;

// New compile and run commands for MPI!
// mpicxx -o blah file.cpp
// mpirun -q -np 32 blah

int main (int argc, char * argv[]) {

	int my_rank;			// my CPU number for this process
	int p;					// number of CPUs that we have
	int source;				// rank of the sender
	int dest;				// rank of destination
	int tag = 0;			// message number
	char message[100];		// message itself
	MPI_Status status;		// return status for receive
	
	// Start MPI
	MPI_Init(&argc, &argv);
	
	// Find out my rank!
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	
	// Find out the number of processes!
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	
	// THE REAL PROGRAM IS HERE
    // Seed the generator with 71911
    int seed = 71911;
    srand(seed);
    long int n = 1000000;
    int * a = new int[n];

    int max = 0;
    int min = 0;
    int avg = 0;

    // Fill the array with random integers on process 0
    if (my_rank == 0) {
        for (int i = 0; i < n; i++) {
            a[i] = rand() % 1000;
        }
    }

    // BREAK THE PROBLEM INTO PIECES
    int localn = n/p;
    int * locala = new int[localn];

    // Scattter
    MPI_Scatter(&a[0], localn, MPI_INT, locala, localn, MPI_INT, 0, MPI_COMM_WORLD);

    // DO THE LOCAL WORK
    int localMax = 0;
    int localMin = locala[0];
    int localAvg = 0;
    
    for (int i = 0; i < localn; i++) {
        // find max
        if (locala[i] > localMax) { 
            localMax = locala[i];
        }
        // find min
        else if (locala[i] < localMin) {
            localMin = locala[i];
        }
        // find avg
        localAvg += locala[i];
    }
    
    // COLLECT THE RESULTS BACK TOGETHER
    MPI_Allreduce(&localMax, &max, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
    MPI_Allreduce(&localMin, &min, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
    MPI_Allreduce(&localAvg, &avg, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    
    if (my_rank == 0) {
        cout << "Maximum: " << max << endl;
        cout << "Minimum: " << min << endl;
        avg = avg / n;
        cout << "Average: " << avg << endl;
    }


    // Shutdown Procedures
    delete [] a;
    delete [] locala;
    
    // Note: start with maximum
    // Only need to scatter array once

	// Shut down MPI
	MPI_Finalize();

	return 0;
}