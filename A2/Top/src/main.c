#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <omp.h>

/* Functions to be used for computation*/

void serial(double const **A, double **L, double **U, int n);
void parallel_for(double** S, double const **A, double **L, double **U, int n);
void sections(double** S, double const **A, double **L, double **U, int n);
void parallel_for_and_sections(double** S, double const **A, double **L, double **U, int n);
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
// Mean:  2.25318152
// Std:  0.4604102547026181
// Median:  2.0479595
// Quartiles:  [1.99467825 2.0479595  2.489866  ]
}

/*Code modified in order to parallelize*/
void crout2(double **S, double const **A, double **L, double **U, int n) {

	int i, j, k;
	double sum = 0;

	for (i = 0; i < n; i++) {
		U[i][i] = 1;
	}

	for(j = 0; j < n; ++j){
		for(i = j; i < n; ++i){
			double sum = S[i][j];
			L[i][j] = A[i][j] - sum;
		}
		for(i = j; i < n; ++i){
			double sum1 = S[j][i];
			U[i][j] = (A[j][i] - sum1)/L[j][j];
		}
		for(int row1= 0; row1 < n; ++row1){
			for(int row2 = 0; row2 < n; ++row2){
				S[row1][row2] += L[row1][j]*U[row2][j];
			}
		}
	}

// 	Mean:  6.2975699800000005
// Std:  0.497581226479878
// Median:  6.1632725
// Quartiles:  [6.0516255  6.1632725  6.31161275]

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

	// #pragma omp parallel for
	// for (i = 0; i < n; i++) {
	// 	U[i][i] = 1;
	// }

	// for (j = 0; j < n; j++) {
	// 	#pragma omp parallel for
	// 	for (i = j; i < n; i++) {
	// 		sum = 0;
	// 		for (k = 0; k < j; k++) {
	// 			sum = sum + L[i][k] * U[k][j];	
	// 		}
	// 		L[i][j] = A[i][j] - sum;
	// 	}

	// 	#pragma omp parallel for
	// 	for (i = j; i < n; i++) {
	// 		sum = 0;
	// 		for(k = 0; k < j; k++) {
	// 			sum = sum + L[j][k] * U[k][i];
	// 		}
	// 		if (L[j][j] == 0) {				
	// 			exit(0);
	// 		}
	// 		U[j][i] = (A[j][i] - sum) / L[j][j];
	// 	}
	// }

	// int i, j, k;
	// double sum = 0;

	// #pragma omp parallel for
	// for (i = 0; i < n; i++) {
	// 	U[i][i] = 1;
	// }

	// for(j = 0; j < n; ++j){
	// 	#pragma omp parallel for
	// 	for(i = j; i < n; ++i){
	// 		double sum = S[i][j];
	// 		L[i][j] = A[i][j] - sum;
	// 	}
	// 	#pragma omp parallel for
	// 	for(i = j; i < n; ++i){
	// 		double sum1 = S[j][i];
	// 		U[i][j] = (A[j][i] - sum1)/L[j][j];	
	// 	}
	// 	#pragma omp parallel for
	// 	for(int row1= 0; row1 < n; ++row1){
	// 		for(int row2 = 0; row2 < n; ++row2){
	// 			S[row1][row2] += L[row1][j]*U[row2][j];
	// 		}
	// 	}
	// }

// t=2 s=1
// Mean:  4.19174432
// Std:  0.662018468307507
// Median:  4.0943185
// Quartiles:  [3.76337475 4.0943185  4.33190825]

// t=4 s=1
// Mean:  3.0817451599999997
// Std:  0.7477344144730631
// Median:  2.898651
// Quartiles:  [2.7412885 2.898651  3.1518205]

// 8 threads
// 	Mean:  3.7330928800000005
// Std:  0.9807395354835989
// Median:  3.430679
// Quartiles:  [3.349906   3.430679   3.73360825]

// 16 Threads
// Mean:  3.7744499200000003
// Std:  0.5987152255394493
// Median:  3.7429959999999998
// Quartiles:  [3.380117  3.742996  4.0613185]



	int i, j, k;
	double sum = 0;

	#pragma omp parallel for
	for (i = 0; i < n; i++) {
		U[i][i] = 1;
	}

	for(j = 0; j < n; ++j){
		#pragma omp parallel
		{
			#pragma omp for
			for(i = j; i < n; ++i){
				double sum = S[i][j];
				L[i][j] = A[i][j] - sum;
			}
			#pragma omp for
			for(i = j; i < n; ++i){
				double sum1 = S[j][i];
				U[i][j] = (A[j][i] - sum1)/L[j][j];
			}
			#pragma omp for
			for(int row1= 0; row1 < n; ++row1){
				for(int row2 = 0; row2 < n; ++row2){
					S[row1][row2] += L[row1][j]*U[row2][j];
				}
			}
		}
	}
// t=2 s=1
// Mean:  4.1586450600000004
// Std:  0.5188075478612242
// Median:  4.022812
// Quartiles:  [3.831518   4.022812   4.36518525]

// t=4 s=1
// Mean:  3.4565296200000004
// Std:  0.9625059157183167
// Median:  3.231115
// Quartiles:  [2.905497 3.231115 3.587474]

// t=8 s=1
// Mean:  3.89679238
// Std:  0.6694392891157761
// Median:  3.7067810000000003
// Quartiles:  [3.4236095 3.706781  4.385932 ]

// t=16 s=1
// Mean:  3.6091626199999998
// Std:  0.6000135511020027
// Median:  3.577491
// Quartiles:  [3.19749025 3.577491   3.764021  ]

}

void sections(double** S, double const **A, double **L, double **U, int n){

	int i, j, k;
	double sum = 0;
	#pragma omp parallel sections
	{
		#pragma omp section
		{
			for (i = 0; i < n; i++) {
				U[i][i] = 1;
			}

			for(j = 0; j < n; ++j){
				for(i = j; i < n; ++i){
					double sum = S[i][j];
					L[i][j] = A[i][j] - sum;
				}
			}

			for(i = j; i < n; ++i){
				double sum1 = S[j][i];
				U[i][j] = (A[j][i] - sum1)/L[j][j];
			}
		}
	}
		for(int row1= 0; row1 < n; ++row1){
			for(int row2 = 0; row2 < n; ++row2){
				S[row1][row2] += L[row1][j]*U[row2][j];
			}
		}
// t=2 s=2
// Mean:  0.9801052156862745
// Std:  0.4806103530153117
// Median:  0.846232
// Quartiles:  [0.808065  0.846232  0.9963385]

// t=4 s=2
// Mean:  1.0899570200000002
// Std:  0.16323668633655733
// Median:  1.062184
// Quartiles:  [1.01467025 1.062184   1.10006075]

// t=8 s=2
// Mean:  1.01833792
// Std:  0.3990826651578262
// Median:  0.9323505000000001
// Quartiles:  [0.90091325 0.9323505  0.98323025]

// Mean:  1.0455215999999998
// Std:  0.4453031272143056
// Median:  0.9482455
// Quartiles:  [0.8892845  0.9482455  1.06243975]
}

void parallel_for_and_sections(double** S, double const **A, double **L, double **U, int n){

	int i, j, k;
	double sum = 0;
	#pragma omp parallel sections
	{
		#pragma omp section
		{
			for (i = 0; i < n; i++) {
				U[i][i] = 1;
			}
		
			for(j = 0; j < n; ++j){
				for(i = j; i < n; ++i){
					double sum = S[i][j];
					L[i][j] = A[i][j] - sum;
				}
			}
		
			for(i = j; i < n; ++i){
				double sum1 = S[j][i];
				U[i][j] = (A[j][i] - sum1)/L[j][j];
			}
		}
	}
		#pragma omp parallel for
		for(int row1= 0; row1 < n; ++row1){
			for(int row2 = 0; row2 < n; ++row2){
				S[row1][row2] += L[row1][j]*U[row2][j];
			}
		}

	// t=2 s=3
	// Mean:  1.22844352
	// Std:  0.6086452464517977
	// Median:  1.0297475
	// Quartiles:  [0.9110895  1.0297475  1.30143325]

	// t=4 s=3
	// Mean:  1.1463481
	// Std:  0.19602874673876278
	// Median:  1.0765985
	// Quartiles:  [1.00363175 1.0765985  1.250061  ]

	// t=8 s=3
	// Mean:  0.9093152000000001
	// Std:  0.18949808953295547
	// Median:  0.8536950000000001
	// Quartiles:  [0.8135435  0.853695   0.89459375]

	// t=16 s=3
	// Mean:  1.14158366
	// Std:  1.0566564951923707
	// Median:  0.8344125
	// Quartiles:  [0.8047885  0.8344125  1.05973775]
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

	double ** matrix = malloc(sizeof(double*)*n*n);
	double** L = malloc(sizeof(double*)*n*n);
	double** U = malloc(sizeof(double*)*n*n);
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
			//crout2(S, (const double**)matrix, L, U, n);
			crout((const double**)matrix, L, U, n);
			// for(int i=0;i<n;i++){
			// 	for(int j=0;j<n;j++){
			// 		printf("%lf ",U[i][j]);
			// 	}
			// 	printf("\n");
			// }
			// for(int i=0;i<n;i++){
			// 	for(int j=0;j<n;j++){
			// 		printf("%lf ",L[i][j]);
			// 	}
			// 	printf("\n");
			// }
		break;

		case 1:
			// printf("Parallel-For\n");
			timeFile = fopen("../util/time_1.txt", "a");
			parallel_for(S, (const double**)matrix, L, U, n);
		break;

		case 2:
			// printf("Sections\n");
			timeFile = fopen("../util/time_2.txt", "a");
			sections(S, (const double**)matrix, L, U, n);
			//printf("here\n");
		break;

		case 3:
			// printf("Parallel-For-And-Sections\n");
			timeFile = fopen("../util/time_3.txt", "a");
			parallel_for_and_sections(S, (const double**)matrix, L, U, n);
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
	// save(outU, argv, U, n, C, true);
	save(outU, argv, U, R, n, false);

	double total = omp_get_wtime() - start;
	fprintf(timeFile, "%lf\n", total);
	return 0;
}