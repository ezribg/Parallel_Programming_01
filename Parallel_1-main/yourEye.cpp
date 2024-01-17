#include <iostream>
#include <stdio.h>
#include <string.h>
#include <iomanip>
#include <fstream>
#include <stdlib.h>
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
 
    // Notes
    // Read in text file full of letters
    // figure out how many of each letter there are in code (lowercase alphabetic)

	string tempbook;
	// hold the letter counts
	int a[26];
	int n = 0;

	for (int i = 0; i < 26; i++) {
		a[i] = 0;
	}

    // process read the text file
	if (my_rank == 0) {
		ifstream code;
		code.open("mordorsecode.txt");
		//char let;

		code >> tempbook;
		n = tempbook.length();

		code.close();
	}

	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
	char * book = new char[n + 1];

	if (my_rank == 0) {
		strcpy(book, tempbook.c_str());
	}

    // BREAK THE PROBLEM INTO PIECES
	int localn = n/p;
	char * localbook = new char[localn];
	int locala[26];

	for (int i = 0; i < 26; i++) {
		locala[i] = 0;
	}

	// Scatter
	MPI_Scatter(&book[0], localn, MPI_CHAR, localbook, localn, MPI_CHAR, 0 , MPI_COMM_WORLD);

	// DO THE LOCAL WORK
    // count how many of each letter appears in that section

	int index = 0;
	for (int i = 0; i < localn; i++) {
		// find the asci value and use that as an index
		index = int(localbook[i]) - 97;
		locala[index] += 1;
	}
	cout << endl;


	/*for (int i = 0; i < 26; i++) {
		cout << setw(3) << locala[i];
	}
	cout << endl;*/

	MPI_Reduce(&locala, &a, 26, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	if (my_rank == 0) {
		for (int i = 0; i < 26; i++) {
			cout << book[i] << ": " << a[i] << endl;
		}
		//cout << endl;
	}

	/*delete [] a;
	delete [] locala;
	delete [] book;
	delete [] localbook;*/

	// Shut down MPI
	MPI_Finalize();

	return 0;
}