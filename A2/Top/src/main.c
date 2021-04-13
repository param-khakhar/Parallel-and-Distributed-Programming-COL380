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
		for(int row1= j; row1 < n; ++row1){
			for(int row2 = j; row2 < n; ++row2){
				S[row1][row2] += L[row1][j]*U[row2][j];
			}
		}
	}

// Mean:  3.30750578
// Std:  0.9560909365193311
// Median:  2.9524714999999997
// Quartiles:  [2.737278   2.9524715  3.50968125]

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

// t=2 s=1
// Mean:  3.91397668
// Std:  1.7829833162995041
// Median:  2.7358255
// Quartiles:  [2.5122305 2.7358255 5.789642 ]

// t=4 s=1
// Mean:  3.67014406
// Std:  1.7390508267069298
// Median:  2.5972635000000004
// Quartiles:  [2.39861275 2.5972635  5.8257665 ]

// t=8 s=1
// Mean:  4.23569964
// Std:  1.3306256776420597
// Median:  3.8459015
// Quartiles:  [3.30708   3.8459015 4.7133185

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
	// 	for(int row1 = j; row1 < n; ++row1){
	// 		for(int row2 = j; row2 < n; ++row2){
	// 			S[row1][row2] += L[row1][j]*U[row2][j];
	// 		}
	// 	}
	// }

// t=2 s=1
// Mean:  2.1635930000000005
// Std:  0.878741528368109
// Median:  1.8104714999999998
// Quartiles:  [1.6903945  1.8104715  2.13561175]

// t=4 s=1
// Mean:  1.7625715599999998
// Std:  0.6409910899966134
// Median:  1.5783005
// Quartiles:  [1.40876925 1.5783005  1.8354945 ]

// t=8 s=1
// Mean:  2.11953688
// Std:  0.7999985569323145
// Median:  1.7527225
// Quartiles:  [1.60590625 1.7527225  2.35979475]

// t=16 s=1
// Mean:  2.09160894
// Std:  0.6012864660621727
// Median:  1.9782885000000001
// Quartiles:  [1.733857  1.9782885 2.197984 ]

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
			for(int row1= j; row1 < n; ++row1){
				for(int row2 = j; row2 < n; ++row2){
					S[row1][row2] += L[row1][j]*U[row2][j];
				}
			}
		}
	}

// t=2 s=1
// Mean:  2.25618374
// Std:  0.8671769853903137
// Median:  1.9319085
// Quartiles:  [1.78659775 1.9319085  2.33366675]

// t=4 s=1
// Mean:  1.58451446
// Std:  0.6060173870948328
// Median:  1.4308325000000002
// Quartiles:  [1.32300625 1.4308325  1.565541  ]

// t=8 s=1
// Mean:  2.51647428
// Std:  1.5373583751563074
// Median:  1.9072244999999999
// Quartiles:  [1.4208065 1.9072245 2.8263905]

// t=16 s=1
// Mean:  3.6091626199999998
// Std:  0.6000135511020027
// Median:  3.577491
// Quartiles:  [3.19749025 3.577491   3.764021  ]

}

void sections(double** S, double const **A, double **L, double **U, int n){

	int i, j, k;
	double sum = 0;

	for (i = 0; i < n; i++) {
		U[i][i] = 1;
	}

	for(j = 0; j < n; ++j){

		#pragma omp sections
		{
			#pragma omp section
			{
				for(i = j; i < n; ++i){
					double sum = S[i][j];
					L[i][j] = A[i][j] - sum;
				}
			}
			#pragma omp section
			{
				for(i = j; i < n; ++i){
					double sum1 = S[j][i];
					U[i][j] = (A[j][i] - sum1)/L[j][j];
				}
			}
		}
		for(int row1= j; row1 < n; ++row1){
			for(int row2 = j; row2 < n; ++row2){
				S[row1][row2] += L[row1][j]*U[row2][j];
			}
		}
	}

// t=2 s=2
// Mean:  3.0786281600000005
// Std:  0.7004467155157589
// Median:  2.862857
// Quartiles:  [2.82164825 2.862857   2.91581375]

// t=4 s=2
// Mean:  2.6894489800000003
// Std:  0.5254356132374162
// Median:  2.5689225
// Quartiles:  [2.52432025 2.5689225  2.5990585 ]

// t=8 s=2
// Mean:  2.60197444
// Std:  0.3235448484293428
// Median:  2.502964
// Quartiles:  [2.459689   2.502964   2.56798275]

// t=16 s=2
// Mean:  2.9670340800000004
// Std:  0.783093270916558
// Median:  2.6470700000000003
// Quartiles:  [2.57539775 2.64707    2.91303775]

}

void parallel_for_and_sections(double** S, double const **A, double **L, double **U, int n){

	int i, j, k;
	double sum = 0;

	#pragma omp parallel for
	for (i = 0; i < n; i++) {
		U[i][i] = 1;
	}

	for(j = 0; j < n; ++j){

		#pragma omp sections
		{
			#pragma omp section
			{
				for(i = j; i < n; ++i){
					double sum = S[i][j];
					L[i][j] = A[i][j] - sum;
				}
			}
			#pragma omp section
			{
				for(i = j; i < n; ++i){
					double sum1 = S[j][i];
					U[i][j] = (A[j][i] - sum1)/L[j][j];
				}
			}
		}
		#pragma omp parallel for
		for(int row1= j; row1 < n; ++row1){
			for(int row2 = j; row2 < n; ++row2){
				S[row1][row2] += L[row1][j]*U[row2][j];
			}
		}
	}
	
// t=2 s=3
// Mean:  2.18310206
// Std:  0.6817920419380211
// Median:  2.0206325
// Quartiles:  [1.8541365  2.0206325  2.16642375]

// t=4 s=3
// Mean:  1.6981086599999997
// Std:  0.6347995797715562
// Median:  1.4714455
// Quartiles:  [1.4220025  1.4714455  1.68857225]

// t=8 s=3
// Mean:  1.90322018
// Std:  0.8042048925614339
// Median:  1.66589
// Quartiles:  [1.49001675 1.66589    1.86378325]

// t=16 s=3
// Mean:  2.1602166599999997
// Std:  0.7432939171614042
// Median:  1.9509345
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
			crout2(S, (const double**)matrix, L, U, n);
			//crout((const double**)matrix, L, U, n);
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
	save(outU, argv, U, R, n, true);

	double total = omp_get_wtime() - start;
	fprintf(timeFile, "%lf\n", total);
	return 0;
}