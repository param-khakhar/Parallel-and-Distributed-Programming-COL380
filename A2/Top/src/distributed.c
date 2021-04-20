#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <mpi.h>


/* Functions to be used for computation*/
void save(char* out, char* argv[], double** mat, int r, int c, bool transpose);
void write_output(char fname[], double** arr, int n );
void forw_elim(double **origin, double *master_row, size_t dim);

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

int main(int argc, char* argv[]){

	// double start = omp_get_wtime();
	int threads;
	int strategy;
	int p, my_rank;
	int n = 0;
	const int root_p = 0;
	// if(my_rank == 0)
	// {
		R = atoi(argv[1]);
		C = R;
		n = R;
		
		FILE* inputFile = fopen(argv[2],"r");
		FILE* timeFile;

		threads = atoi(argv[3]);
		strategy = atoi(argv[4]);

		double* matrix = malloc(sizeof(double)*n*n);
// 		// double** L = malloc(sizeof(double*)*n*n);
// 		// double** U = malloc(sizeof(double*)*n*n);
// 		// double ** S = malloc(sizeof(double *)*n*n);

// 		// double** D = malloc(sizeof(double*)*n*n);

// 		// for(int i=0;i<n;i++){
// 		// 	S[i] = (double*)calloc(n, sizeof(double));
// 		// }

// 		// for(int i=0;i<R;i++){
// 		// 	L[i] = (double*)calloc(n, sizeof(double));
// 		// }

// 		// for(int i=0;i<n;i++){
// 		// 	U[i] = (double*)calloc(C, sizeof(double));
// 		// 	// D[i] = (double*)calloc(n, sizeof(double));
// 		// }

// 		// for(int i=0;i<R;i++){
// 		// 	matrix[i] = (double*)calloc(C, sizeof(double));
// 		// }

		for(int i=0;i<n*n;i++){
				fscanf(inputFile, "%lf", &matrix[i]);
		}

   MPI_Init(NULL, NULL);
   MPI_Comm_size(MPI_COMM_WORLD, &threads);
   MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

   int i, j, k, d = 0;

   double start = MPI_Wtime();

   for (i = 0; i < n-1; i++) {
      for (j = d + 1; j < n; j++) {
        	if (j % threads == my_rank){
				double m = matrix[j*n + d]/matrix[d*n + d];
				for(k=1; k < n-d; k++)
				{
					matrix[j*n + d + k] = matrix[j*n + d + k] - m * matrix[d*n + d + k];
				}
				matrix[j*n + d] = m;
        	}
      	}
      	for (j = d + 1; j < n; j++) {
        	MPI_Bcast(&matrix[j*n + d], n - d, MPI_DOUBLE, j % threads, MPI_COMM_WORLD);
      	}
		d += 1;
   	}

   	double end = MPI_Wtime();

   	if (my_rank == 0) {
		for(int i=0;i<n*n;i++)
			printf("%lf ",matrix[i]);
		printf("\n");
		printf("mpi: %f s\n", end - start);
   	}	
	free(matrix);
   	MPI_Finalize();
	
   	return 0;
}