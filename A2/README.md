# Crout Matrix Decomposition using MPI

## Contents of the Top Directory

- src/: The file main.c within the src contains the source code for the assignment.
- obj/: The object files, for the source code, currently empty.
- bin/: The binary executable of the source code.
- inputs/: The input matrix.txt file resides in this folder.
- writeup/: The Latex files present for the report.
- output/: The output decomposition for the sub-matrices L, U, and D.

## Make Instructions

- In order to compile the source file, use make compile.
- In order to execute the executable, use make execute n=N m=M i=../inputs/matrix.txt t=2 s=0, here n = Rows of the input matrix, m = Columns of the input matrix, i = path to the txt file containing the input matrix, t = number of threads, s = strategy #. 

As of now, you can use make execute n=3 m=3 i=../inputs/matrix.txt t=2 s=0.

