#include <stdio.h>
#include <math.h>

#define SIZE 40

void read_vector(double x[], int n) {
	for(int i = 0; i < n; ++i) {
		scanf("%lf", x++);
	}
}

void print_vector(double x[], int n) {
	for(int i = 0; i < n; ++i) {
		printf("%.4f ", x[i]);
	}
	printf("\n");
}

void read_mat(double A[][SIZE], int m, int n) {
	for(int i = 0; i < m; ++i) {
		for(int j = 0; j < n; ++j) {
			scanf("%lf", &A[i][j]);
		}
	}
}

void print_mat(double A[][SIZE], int m, int n) {
	for(int i = 0; i < m; ++i) {
		for(int j = 0; j < n; ++j) {
			printf("%.4f ", A[i][j]);
		}
		printf("\n");
	}
}

// 1. Calculate matrix product, AB = A X B
// A[m][p], B[p][n], AB[m][n]
void mat_product(double A[][SIZE], double B[][SIZE], double AB[][SIZE], int m, int p, int n) {
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			for (int k = 0; k < p; k++) {
				AB[i][j] += A[i][k] * B[k][j];
			}
		}
	}
}


// 2. Matrix triangulation and determinant calculation - simplified version
// (no rows' swaps). If A[i][i] == 0, function returns NAN.
// Function may change A matrix elements.
double gauss_simplified(double A[][SIZE], int n) {
	double c;
	for (int i = 0; i < n; i++) {
		if (A[i][i] == 0) return NAN;

		for (int j = i + 1; j < n; j++) {
			c = A[j][i] / A[i][i];

			for (int k = i; k < n; k++) {
				A[j][k] -= A[i][k] * c;
			}
		}
	}

	double det = 1;
	for (int i = 0; i < n; i++) {
		det *= A[i][i];
	}

	return det;
}

// 3. Matrix triangulation, determinant calculation, and Ax = b solving - extended version
// (Swap the rows so that the row with the largest, leftmost nonzero entry is on top. While
// swapping the rows use index vector - do not copy entire rows.)
// If max A[i][i] < eps, function returns 0.
// If det != 0 && b != NULL && x != NULL then vector x should contain solution of Ax = b.

void sortIndeces(double A[][SIZE], int indeces[], int n, int index, int *swaps) {
	int temp;
	for (int i = 0; i < n - index; i++) {
		for (int j = index + 1; j < n - i; j++) {
			if (fabs(A[indeces[j - 1]][index]) < fabs(A[indeces[j]][index])) {
				temp = indeces[j];
				indeces[j] = indeces[j - 1];
				indeces[j - 1] = temp;
				(*swaps)++;
			}
		}
	}
}

double gauss(double A[][SIZE], const double b[], double x[], const int n, const double eps) {
	int indeces[SIZE], swaps = 0;
	double results[SIZE];
	for (int i = 0; i < n; i++) {
		indeces[i] = i;

		if (b != NULL && x != NULL) {
			results[i] = b[i];
		}
	}

	if (b == NULL) {
		x = NULL;
	}

	double c;
	for (int i = 0; i < n; i++) {
		sortIndeces(A, indeces, n, i, &swaps);

		if (fabs(A[indeces[i]][i]) < eps) return 0;

		for (int j = i + 1; j < n; j++) {
			c = A[indeces[j]][i] / A[indeces[i]][i];

			for (int k = i; k < n; k++) {
				A[indeces[j]][k] -= A[indeces[i]][k] * c;
			}

			if (b != NULL) {
				results[indeces[j]] -= results[indeces[i]] * c;
			}
		}
	}

	double det = 1;
	for (int i = 0; i < n; i++) {
		det *= A[indeces[i]][i];
	}

	if (b != NULL) {
		for (int i = n - 1; i >= 0; i--) {
			results[indeces[i]] /= A[indeces[i]][i];
			A[indeces[i]][i] = 1;

			for (int j = i - 1; j >= 0; j--) {
				results[indeces[j]] -= results[indeces[i]] * A[indeces[j]][i];
				A[indeces[j]][i] = 0;
			}
		}
	}

	for (int i = 0; i < n; i++) {
		x[i] = results[indeces[i]];
	}

	return det * (swaps % 2 ? -1 : 1);
}

// 4. Returns the determinant; B contains the inverse of A (if det(A) != 0)
// If max A[i][i] < eps, function returns 0.
double matrix_inv(double A[][SIZE], double B[][SIZE], int n, double eps) {
	int indeces[SIZE], swaps = 0;
	for (int i = 0; i < n; i++) {
		indeces[i] = i;
	}

	double result[SIZE][SIZE];
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			result[i][j] = (i == j) ? 1 : 0;
		}
	}

	double c;
	for (int i = 0; i < n; i++) {
		sortIndeces(A, indeces, n, i, &swaps);

		if (fabs(A[indeces[i]][i]) < eps) return 0;

		for (int j = i + 1; j < n; j++) {
			c = A[indeces[j]][i] / A[indeces[i]][i];

			for (int k = 0; k < n; k++) {
				A[indeces[j]][k] -= A[indeces[i]][k] * c;
				result[indeces[j]][k] -= result[indeces[i]][k] * c;
			}
		}
	}

	double det = 1;
	for (int i = 0; i < n; i++) {
		det *= A[indeces[i]][i];
	}

	for (int i = n - 1; i >= 0; i--) {
		for (int j = 0; j < n; j++) {
			result[indeces[i]][j] /= A[indeces[i]][i];
		}
		A[indeces[i]][i] = 1;

		for (int j = i - 1; j >= 0; j--) {
			for (int k = 0; k < n; k++) {
				result[indeces[j]][k] -= result[indeces[i]][k] * A[indeces[j]][i];
			}

			A[indeces[j]][i] = 0;
		}
	}

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			B[i][j] = result[indeces[i]][j];
		}
	}

	return det * (swaps % 2 ? -1 : 1);
}

int main(void) {
	double A[SIZE][SIZE], B[SIZE][SIZE], C[SIZE][SIZE];
	double b[SIZE], x[SIZE], det, eps = 1.e-13;

	int to_do;
	int m, n, p;

	scanf ("%d", &to_do);

	switch (to_do) {
		case 1:
			scanf("%d %d %d", &m, &p, &n);
			read_mat(A, m, p);
			read_mat(B, p, n);
			mat_product(A, B, C, m, p, n);
			print_mat(C, m, n);
			break;
		case 2:
			scanf("%d", &n);
			read_mat(A, n, n);
			printf("%.4f\n", gauss_simplified(A, n));
			break;
		case 3:
			scanf("%d", &n);
			read_mat(A,n, n);
			read_vector(b, n);
			det = gauss(A, b, x, n, eps);
			printf("%.4f\n", det);
			if (det) print_vector(x, n);
			break;
		case 4:
			scanf("%d", &n);
			read_mat(A, n, n);
			det = matrix_inv(A, B, n, eps);
			printf("%.4f\n", det);
			if (det) print_mat(B, n, n);
			break;
		default:
			printf("NOTHING TO DO FOR %d\n", to_do);
			break;
	}
	return 0;
}