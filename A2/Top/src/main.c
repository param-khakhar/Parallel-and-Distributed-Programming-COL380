#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <omp.h>

/* Functions to be used for computation*/

void serial(double const **A, double **L, double **U, int n);
void parallel_for(double** S, double const **A, double **L, double **U, int n);
void sections();
void parallel_for_and_sections();
void distributed();
void save(char* out, char* argv[], double** mat, int r, int c, bool transpose);
void write_output(char fname[], double** arr, int n );

int R;					//Number of Rows
int C;					//Number of Columns
int n;					//n
double** matrix;		//Working matrix

void write_output(char fname[], double** arr, int n){
	FILE *f = fopen(fname, "w");
	for( int i = 0; i < n; i++){
		for(int j = 0; j < n; j++){

			fprintf(f, "%0.12f ", arr[i][j]);
		}
		fprintf(f, "\n");
	}
	fclose(f);
}

/*Same as above but need to write transpose*/
void write_output_T(char fname[], double** arr, int n){
	FILE *f = fopen(fname, "w");
	for( int i = 0; i < n; i++){
		for(int j = 0; j < n; j++){

			fprintf(f, "%0.12f ", arr[j][i]);
		}
		fprintf(f, "\n");
	}
	fclose(f);
}

/*Serial Code*/
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

/*Code modified in order to parallelize*/
void crout2(double **S, double const **A, double **L, double **U, int n) {

	int i, j, k;
	double sum = 0;

	#pragma omp parallel for
	for (i = 0; i < n; i++) {
		U[i][i] = 1;
	}

	for(j = 0; j < n; ++j){
		#pragma omp parallel for
		for(i = j; i < n; ++i){
			double sum = S[i][j];
			L[i][j] = A[i][j] - sum;
		}
		#pragma omp parallel for
		for(i = j; i < n; ++i){
			double sum1 = S[j][i];
			U[i][j] = (A[j][i] - sum1)/L[j][j];
		}
		#pragma omp parallel for
		for(int row1= 0; row1 < n; ++row1){
			for(int row2 = 0; row2 < n; ++row2){
				S[row1][row2] += L[row1][j]*U[row2][j];
			}
		}
	}
}




void save(char* out, char* argv [], double** mat, int r, int c, bool transpose){

	char* temp = malloc(strlen(out)+10);
	strcpy(temp, out);
	strcat(temp, argv[4]);
	strcat(temp, "_");
	strcat(temp, argv[3]);
	strcat(temp, ".txt");

	char name[100];
	strcpy(name, temp);
	if(transpose)
		write_output_T(name, mat, r);
	else
		write_output(name, mat, r);
}

void serial(double const **A, double **L, double **U, int n){
	crout(A, L, U, n);
}

void parallel_for(double** S, double const **A, double **L, double **U, int n){

	// int i, j, k;
	// double sum = 0;
	// // double local1,local2;

	// for (i = 0; i < n; i++) {
	// 	U[i][i] = 1;
	// }

	// for (j = 0; j < n; j++) {
	// 	#pragma omp parallel for
	// 	for (i = j; i < n; i++) {
	// 		sum = 0;
	// 		// #pragma omp parallel
	// 		// {
	// 		// 	double local1 = 0;
	// 		// 	#pragma omp for
	// 		// 	for (k = 0; k < j; k++) {
	// 		// 		local1 = local1 + L[i][k] * U[k][j];	
	// 		// 	}				
	// 		// 	#pragma omp critical
	// 		// 	{
	// 		// 		sum += local1;
	// 		// 	}
	// 		// }
	// 		#pragma omp parallel for shared(j) reduction(+ :sum)
	// 		for (k = 0; k < j; k++) {
	// 			sum = sum + L[i][k] * U[k][j];	
	// 		}
	// 		L[i][j] = A[i][j] - sum;
	// 	}
	// 	#pragma omp for nowait
	// 	for (i = j; i < n; i++) {
	// 		sum = 0;
	// 		// #pragma omp parallel
	// 		// {
	// 		// 	local2 = 0;
	// 		// 	#pragma omp for
	// 		// 	for(k = 0; k < j; k++) {
	// 		// 		local2 = local2 + L[j][k] * U[k][i];
	// 		// 	}
	// 		// 	#pragma omp critical
	// 		// 	{
	// 		// 		sum += local2;
	// 		// 	}
	// 		// }
	// 		#pragma omp parallel for shared(j) reduction(+:sum) 
	// 		for(k = 0; k < j; k++) {
	// 			sum = sum + L[j][k] * U[k][i];
	// 		}
	// 		if (L[j][j] == 0) {				
	// 			exit(0);
	// 		}
	// 		U[j][i] = (A[j][i] - sum) / L[j][j];
	// 	}
	// }

	int i, j, k;
	double sum = 0;

	#pragma omp parallel for
	for (i = 0; i < n; i++) {
		U[i][i] = 1;
	}

	for(j = 0; j < n; ++j){
		#pragma omp parallel for
		for(i = j; i < n; ++i){
			double sum = S[i][j];
			L[i][j] = A[i][j] - sum;
		}
		#pragma omp parallel for
		for(i = j; i < n; ++i){
			double sum1 = S[j][i];
			U[i][j] = (A[j][i] - sum1)/L[j][j];	
		}
		#pragma omp parallel for
		for(int row1= 0; row1 < n; ++row1){
			for(int row2 = 0; row2 < n; ++row2){
				S[row1][row2] += L[row1][j]*U[row2][j];
			}
		}
	}

	// int i, j, k;
	// double sum = 0;

	// #pragma omp parallel for
	// for (i = 0; i < n; i++) {
	// 	U[i][i] = 1;
	// }

	// for(j = 0; j < n; ++j){
	// 	#pragma omp parallel
	// 	{
	// 		for(i = j; i < n; ++i){
	// 			double sum = S[i][j];
	// 			L[i][j] = A[i][j] - sum;
	// 		}
	// 		for(i = j; i < n; ++i){
	// 			double sum1 = S[j][i];
	// 			U[i][j] = (A[j][i] - sum1)/L[j][j];
	// 		}
	// 		for(int row1= 0; row1 < n; ++row1){
	// 			for(int row2 = 0; row2 < n; ++row2){
	// 				S[row1][row2] += L[row1][j]*U[row2][j];
	// 			}
	// 		}
	// 	}
	// }

	// int i, j, k;
	// double sum = 0;

	// #pragma omp parallel for
	// for (i = 0; i < n; i++) {
	// 	U[i][i] = 1;
	// }

	// for(j = 0; j < n; ++j){
	// 	#pragma omp parallel
	// 	{
	// 		#pragma omp for
	// 		for(i = j; i < n; ++i){
	// 			double sum = S[i][j];
	// 			L[i][j] = A[i][j] - sum;
	// 		}
	// 		#pragma omp for
	// 		for(i = j; i < n; ++i){
	// 			double sum1 = S[j][i];
	// 			U[i][j] = (A[j][i] - sum1)/L[j][j];
	// 		}
	// 		#pragma omp for
	// 		for(int row1= 0; row1 < n; ++row1){
	// 			for(int row2 = 0; row2 < n; ++row2){
	// 				S[row1][row2] += L[row1][j]*U[row2][j];
	// 			}
	// 		}
	// 	}
	// }
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

	double start = omp_get_wtime();

	R = atoi(argv[1]);
	C = R;
	n = R;
	
	FILE* inputFile = fopen(argv[2],"r");
	FILE* timeFile;

	int threads = atoi(argv[3]);
	int strategy = atoi(argv[4]);

	omp_set_num_threads(threads);

	double ** matrix = malloc(sizeof(double*)*R*C);
	double** L = malloc(sizeof(double*)*R*n);
	double** U = malloc(sizeof(double*)*n*C);
	double ** S = malloc(sizeof(double *)*n*n);

	// double** D = malloc(sizeof(double*)*n*n);

	for(int i=0;i<n;i++){
		S[i] = (double*)calloc(n, sizeof(double));
	}

	for(int i=0;i<R;i++){
		L[i] = (double*)calloc(n, sizeof(double));
	}

	for(int i=0;i<n;i++){
		U[i] = (double*)calloc(C, sizeof(double));
		// D[i] = (double*)calloc(n, sizeof(double));
	}

	for(int i=0;i<R;i++){
		matrix[i] = (double*)calloc(C, sizeof(double));
	}

	for(int i=0;i<R;i++){
		for(int j=0;j<C;j++){
			fscanf(inputFile, "%lf", &matrix[i][j]);
		}
	}
	switch(strategy){

		case 0:
			// printf("Serial\n");
			timeFile = fopen("../util/time_0.txt", "a");
			crout2(S, (const double**)matrix, L, U, n);
		break;

		case 1:
			// printf("Parallel-For\n");
			timeFile = fopen("../util/time_1.txt", "a");
			parallel_for(S, (const double**)matrix, L, U, n);
		break;

		case 2:
			// printf("Sections\n");
			timeFile = fopen("../util/time_2.txt", "a");
			sections();
		break;

		case 3:
			// printf("Parallel-For-And-Sections\n");
			timeFile = fopen("../util/time_3.txt", "a");
			parallel_for_and_sections();
		break;

		case 4:
			//Redundant
			// printf("Distributed\n");
			timeFile = fopen("../util/time_4.txt", "a");
			distributed();
		break;
	}

	char* outL = "../output/output_L_";
	save(outL, argv, L, R, n, false);
	char* outU = "../output/output_U_";
	save(outU, argv, U, n, C, true);

	double total = omp_get_wtime() - start;
	fprintf(timeFile, "%lf\n", total);
	return 0;
}