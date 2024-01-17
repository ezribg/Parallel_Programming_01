// Ezri Braid-Grizzell
// Jan 24 2023
// CS 452 Butler University

#include <iostream>
#include <stdio.h>
#include <string.h>
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
	
	// ACTUAL PROGRAM
    // Two rings - p is between 4 and 24

    // Evens message
    char * baton = message;
    sprintf(baton, "Sent list: ");
	
    // Odds message
    char * baton2 = message;
    sprintf(baton2, "Sent list: ");
    

    // Rings
    if (my_rank == 0) {
        cout << "Process 0" << endl;
        sprintf(baton + strlen(baton), "%d ", my_rank);
        MPI_Send(baton, strlen(baton) + 1, MPI_CHAR, 2, tag, MPI_COMM_WORLD);

        if (p % 2 == 0) {
            MPI_Recv(baton, 100, MPI_CHAR, p - 2, tag, MPI_COMM_WORLD, &status);
        } else {
            MPI_Recv(baton, 100, MPI_CHAR, p - 1, tag, MPI_COMM_WORLD, &status);
        }
        
        sprintf(baton + strlen(baton), "%d ", my_rank);
        //cout << "Success!" << endl;
        cout << baton << endl;
        
    } else if (my_rank == 1) {
        cout << "Process 1" << endl;
        sprintf(baton2 + strlen(baton2), "%d ", my_rank);
        
        if (p % 2 == 0) {
            MPI_Send(baton2, strlen(baton2) + 1, MPI_CHAR, p - 1, tag, MPI_COMM_WORLD);
        } else {
            MPI_Send(baton2, strlen(baton2) + 1, MPI_CHAR, p - 2, tag, MPI_COMM_WORLD);
        }

        MPI_Recv(baton2, 100, MPI_CHAR, 3, tag, MPI_COMM_WORLD, &status);
        sprintf(baton2 + strlen(baton2), "%d ", my_rank);
        //cout << "Success again!" << endl;
        cout << baton2 << endl;

    } else if  (my_rank % 2 == 0) {
        MPI_Recv(baton, 100, MPI_CHAR, my_rank - 2, tag, MPI_COMM_WORLD, &status);
        sprintf(baton + strlen(baton), "%d ", my_rank);
        cout << "Process " << my_rank << endl;

        if (my_rank + 2 > p - 1) {
            MPI_Send(baton, strlen(baton) + 1, MPI_CHAR, 0, tag, MPI_COMM_WORLD);
        } else {
            MPI_Send(baton, strlen(baton) + 1, MPI_CHAR, my_rank + 2, tag, MPI_COMM_WORLD);
        }
        
        cout << baton << endl;
    } else {

        if (my_rank == p - 1 || my_rank == p - 2) {
            MPI_Recv(baton2, 100, MPI_CHAR, 1, tag, MPI_COMM_WORLD, &status);
        } else {
            MPI_Recv(baton2, 100, MPI_CHAR, my_rank + 2, tag, MPI_COMM_WORLD, &status);
        }

        sprintf(baton2 + strlen(baton2), "%d ", my_rank);
        cout << "Process " << my_rank << endl;
        MPI_Send(baton2, strlen(baton2) + 1, MPI_CHAR, my_rank - 2, tag, MPI_COMM_WORLD);
        cout << baton2 << endl;
    }
	
	// Shut down MPI
	MPI_Finalize();

	return 0;
}