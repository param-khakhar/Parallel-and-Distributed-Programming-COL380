# Crout Matrix Decomposition using MPI

## Contents of the Top Directory

- src/: The file main.c within the src contains the source code for the assignment.
- obj/: The object files, for the source code, currently empty.
- bin/: The binary executable of the source code.
- inputs/: The input matrix.txt file resides in this folder.
- writeup/: The Latex files present for the report.
- output/: The output decomposition for the sub-matrices L, U, and D
- util/: It contains the files for the times required by the strategies. We are considering 50 runs and then printing the mean, median and the std of the times.

## Make Instructions

- In order to **compile** the source file, use make compile.
- In order to **execute** the executable, use make execute n=N m=M i=../inputs/matrix.txt t=2 s=0, here n = Rows of the input matrix, i = path to the txt file containing the input matrix, t = number of threads, s = strategy #. 

As of now, you can use make execute n=3 i=../inputs/matrix_1.txt t=2 s=1

- In order to perform a **test** run of 50 iterations, use make test n=3 i=../inputs/matrix_1.txt t=2 s=1. The number of iterations can therefore be changed in the make file in the folder /bin.



