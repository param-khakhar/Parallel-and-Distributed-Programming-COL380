#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <omp.h>

/* Functions to be used for computation*/

void serial();
void parallel_for();
void sections();
void parallel_for_and_sections();
void distributed();
void save(char* out, char* argv[], double** mat, int r, int c);

int R;					//Number of Rows
int C;					//Number of Columns
int n;					//?
double** matrix;		//Working matrix

void crout(double const **A, double **L, double **U, int n) {
	int i, j, k;
	double sum = 0;

	for (i = 0; i < n; i++) {
		U[i][i] = 1;
	}

	for (j = 0; j < n; j++) {
		for (i = j; i < n; i++) {
			sum = 0;
			for (k = 0; k < j; k++) {
				sum = sum + L[i][k] * U[k][j];	
			}
			L[i][j] = A[i][j] - sum;
		}

		for (i = j; i < n; i++) {
			sum = 0;
			for(k = 0; k < j; k++) {
				sum = sum + L[j][k] * U[k][i];
			}
			if (L[j][j] == 0) {				
				exit(0);
			}
			U[j][i] = (A[j][i] - sum) / L[j][j];
		}
	}
}

void save(char* out, char* argv [], double** mat, int r, int c){

	char* temp = malloc(strlen(out)+10);
	strcpy(temp, out);
	strcat(temp, argv[5]);
	strcat(temp, "_");
	strcat(temp, argv[4]);
	strcat(temp, ".txt");

	FILE* saved;
	saved = fopen((const char*)temp, "w");
	if(!saved){

	}
	for(int i=0;i<r;i++){
		for(int j=0;j<c;j++){
			//printf("%d %d\n",i,j);
			if(saved)
				fprintf(saved, "%.03lf ", mat[i][j]);
		}
		if(saved)
			fprintf(saved, "\n");
	}
}

void serial(){
	printf("Not Yet Implemented\n");
}

void parallel_for(){
	printf("Not Yet Implemented\n");
}

void sections(){
	printf("Not Yet Implemented\n");
}

void parallel_for_and_sections(){
	printf("Not Yet Implemented\n");
}

void distributed(){
	printf("Not Yet Implemented\n");
}

int main(int argc, char* argv[]){

	R = atoi(argv[1]);
	C = atoi(argv[2]);
	
	FILE* inputFile = fopen(argv[3],"r");
	
	int threads = atoi(argv[4]);

	int strategy = atoi(argv[5]);

	// printf("Sanity Check:\n");
	// printf("Argc: %d\n", argc);
	// printf("Rows: %d\n",R);
	// printf("Columns: %d\n", C);
	// printf("Strategy: %d\n",strategy);
	// printf("# Threads: %d\n", threads);

	n = 3;

	double ** matrix = malloc(sizeof(double*)*R*C);
	double** L = malloc(sizeof(double*)*R*n);
	double** U = malloc(sizeof(double*)*n*C);
	double** D = malloc(sizeof(double*)*n*n);

	for(int i=0;i<R;i++){
		L[i] = (double*)calloc(n, sizeof(double));
	}

	for(int i=0;i<n;i++){
		U[i] = (double*)calloc(C, sizeof(double));
		D[i] = (double*)calloc(n, sizeof(double));
	}

	for(int i=0;i<R;i++){
		matrix[i] = (double*)calloc(C, sizeof(double));
	}

	for(int i=0;i<R;i++){
		for(int j=0;j<C;j++){
			fscanf(inputFile, "%lf", &matrix[i][j]);
			//printf("%lf ",matrix[i][j]);
		}
		//printf("\n");
	}
	switch(strategy){

		case 0:
			printf("Serial\n");
			crout((const double**)matrix, L, U, n);

			for(int i=0;i<n;i++){
				D[i][i] = L[i][i];
				L[i][i] = 1;
			}
			
			// for(int i=0;i<R;i++){
			// 	for(int j=0;j<n;j++){
			// 		printf("%lf ", L[i][j]);
			// 	}
			// 	printf("\n");
			// }
			// printf("\n");
			// for(int i=0;i<n;i++){
			// 	for(int j=0;j<n;j++){
			// 		printf("%lf ",D[i][j]);
			// 	}
			// 	printf("\n");
			// }
			// printf("\n");
			// for(int i=0;i<n;i++){
			// 	for(int j=0;j<C;j++){
			// 		printf("%lf ", U[i][j]);
			// 	}
			// 	printf("\n");
			// }
		break;

		case 1:
			printf("Parallel-For\n");
			parallel_for();
		break;

		case 2:
			printf("Sections\n");
			sections();
		break;

		case 3:
			printf("Parallel-For-And-Sections\n");
			parallel_for_and_sections();
		break;

		case 4:
			printf("Distributed\n");
			distributed();
		break;
	}
	char* outL = "../output/output_L_";
	save(outL, argv, L, R, n);
	char* outU = "../output/output_U_";
	save(outU, argv, U, n, C);
	char* outD = "../output/output_D_";
	save(outD, argv, D, n, n);

	return 0;
}