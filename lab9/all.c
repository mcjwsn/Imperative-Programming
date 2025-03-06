#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#define TAB_SIZE 1000
#define BUF_SIZE 1000

typedef struct {
	int *values, len;
	double average;
} LineType;

typedef struct {
	int r, c, v;
} Triplet;

int get(int cols, int row, int col, const int *A) {
	return A[(row * cols) + col];
}

void set(int cols, int row, int col, int *A, int value) {
	A[(row * cols) + col] = value;
}

char isDigit(char x) {
	return '0' <= x && x <= '9' || x == '-';
}

void prod_mat(int rowsA, int colsA, int colsB, int *A, int *B, int *AB) {
	int value;
	for (int i = 0; i < rowsA; i++) {
		for (int j = 0; j < colsB; j++) {
			value = 0;

			for (int k = 0; k < colsA; k++) {
				value += get(colsA, i, k, A) * get(colsB, k, j, B);
			}

			set(colsB, i, j, AB, value);
		}
	}
}

void read_mat(int rows, int cols, int *t) {
	int value;
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			scanf("%d", &value);
			set(cols, i, j, t, value);
		}
	}
}

void print_mat(int rows, int cols, int *t) {
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			printf("%d ", get(cols, i, j, t));
		}

		printf("\n");
	}
}

int countNumbersInString(char buff[]) {
	char inNumber = 0;
	int result = 0;

	for (int i = 0; i < strlen(buff); i++) {
		if (!inNumber && isDigit(buff[i])) {
			result++;
			inNumber = 1;
		} else if (inNumber && !isDigit(buff[i])) {
			inNumber = 0;
		}
	}

	return result;
}

int numbersFromString(char buff[], int *arr) {
	char inNumber = 0, minus = 0;
	int index = -1;

	for (int i = 0; i < strlen(buff); i++) {
		if (!inNumber && isDigit(buff[i])) {
			index++;
			inNumber = 1;

			if (buff[i] == '-') {
				arr[index] = 0;
				minus = 1;
			} else {
				arr[index] = buff[i] - '0';
			}

		} else if (inNumber && isDigit(buff[i])) {
			arr[index] *= 10;
			arr[index] += buff[i] - '0';
		} else if (inNumber && !isDigit(buff[i])) {
			if (minus) {
				arr[index] *= -1;
				minus = 0;
			}

			inNumber = 0;
		}
	}

	return index + inNumber + 1;
}

int read_int_lines_cont(int *ptr_array[]) {
	char buff[BUF_SIZE];
	int result = 0, totalNumbers = 0;

	for (int i = 1; i < TAB_SIZE; i++) {
		ptr_array[i] = NULL;
	}

	while (!feof(stdin)) {
		strcpy(buff, "");
		fgets(buff, BUF_SIZE, stdin);

		ptr_array[++result] = &ptr_array[0][totalNumbers];
		totalNumbers += numbersFromString(buff, ptr_array[0] + totalNumbers);
	}

	return result;
}

void write_int_line_cont(int *ptr_array[], int n) {
	int *start = ptr_array[n + 1];

	while (start < ptr_array[n + 2]) {
		printf("%d ", *start++);
	}

	printf("\n");
}

int read_char_lines(char *array[]) {
	int result = 0;
	char buff[BUF_SIZE];

	while (!feof(stdin)) {
		strcpy(buff, "");
		fgets(buff, BUF_SIZE, stdin);
		array[result] = malloc(sizeof(char) * (strlen(buff) + 1));
		strcpy(array[result], buff);

		if (array[result][strlen(buff) - 1] == '\n') {
			array[result][strlen(buff) - 1] = '\0';
		}

		result++;
	}

	return result;
}

void write_char_line(char *array[], int n) {
	puts(array[n]);
}

void delete_char_lines(char *array[]) {
	for (int i = 0; array[i] != NULL; i++) {
		free(array[i]);
	}
}

int read_int_lines(LineType lines_array[]) {
	char buff[BUF_SIZE];
	int result = 0;

	while (!feof(stdin)) {
		strcpy(buff, "");
		fgets(buff, BUF_SIZE, stdin);

		lines_array[result].len = countNumbersInString(buff);
		lines_array[result].values = malloc(sizeof(int) * lines_array[result].len);

		numbersFromString(buff, lines_array[result++].values);
	}

	return result - 1;
}

void write_int_line(LineType lines_array[], int n) {
	for (int i = 0; i < lines_array[n].len; i++) {
		printf("%d ", lines_array[n].values[i]);
	}

	printf("\n%.2lf\n", lines_array[n].average);
}

void delete_int_lines(LineType array[], int line_count) {
	for (int i = 0; i < line_count; i++) {
		free(array[i].values);
	}
}

int cmp(const void *a, const void *b) {
	LineType *x = (LineType *)a, *y = (LineType *)b;

	return x->average > y->average ? 1 :
		x->average == y->average ? 0 : -1;
}

void sort_by_average(LineType lines_array[], int line_count) {
	long long sum;
	for (int i = 0; i < line_count; i++) {
		sum = 0;

		for (int j = 0; j < lines_array[i].len; j++) {
			sum += lines_array[i].values[j];
		}

		lines_array[i].average = (double)sum / lines_array[i].len;
	}

	qsort(lines_array, line_count, sizeof(LineType), cmp);
}

void read_sparse(Triplet *triplet_array, int n_triplets) {
	for (int i = 0; i < n_triplets; i++) {
		scanf("%d %d %d", &triplet_array[i].r, &triplet_array[i].c, &triplet_array[i].v);
	}
}

int cmp_triplets(const void *t1, const void *t2) {
	Triplet *x = (Triplet *)t1, *y = (Triplet *)t2;

	if (x->r == y->r) {
		return x->c - y->c;
	}

	return x->r - y->r;
}

void make_CSR(Triplet *triplet_array, int n_triplets, int rows, int *V, int *C, int *R) {
	qsort(triplet_array, n_triplets, sizeof(Triplet), cmp_triplets);

	int lastRow = 0;
	R[0] = 0;

	for (int i = 0; i < n_triplets; i++) {
		V[i] = triplet_array[i].v;
		C[i] = triplet_array[i].c;

		if (lastRow != triplet_array[i].r) {
			for (int j = lastRow + 1; j <= triplet_array[i].r; j++) {
				R[j] = i;
			}

			lastRow = triplet_array[i].r;
		}
	}

	for (int j = lastRow + 1; j <= rows; j++) {
		R[j] = n_triplets;
	}
}

void multiply_by_vector(int rows, const int *V, const int *C, const int *R, const int *x, int *y) {
	for (int i = 0; i < rows; i++) {
		y[i] = 0;

		for (int j = R[i]; j < R[i + 1]; j++) {
			y[i] += V[j] * x[C[j]];
		}
	}
}

void read_vector(int *v, int n) {
	for (int i = 0; i < n; i++) {
		scanf("%d", &v[i]);
	}
}

void write_vector(int *v, int n) {
	for (int i = 0; i < n; i++) {
		printf("%d ", v[i]);
	}

	printf("\n");
}

int read_int() {
	char c_buf[BUF_SIZE];
	fgets(c_buf, BUF_SIZE, stdin);
	return (int)strtol(c_buf, NULL, 10);
}

int main(void) {
	int to_do = read_int();

	int A[TAB_SIZE], B[TAB_SIZE], AB[TAB_SIZE];
	int n, lines_counter, rowsA, colsA, rowsB, colsB;
	int rows, cols, n_triplets;
	char *char_lines_array[TAB_SIZE] = { NULL };
	int continuous_array[TAB_SIZE];
	int *ptr_array[TAB_SIZE] = { NULL };
	Triplet triplet_array[TAB_SIZE];
	int V[TAB_SIZE], C[TAB_SIZE], R[TAB_SIZE];
	int x[TAB_SIZE], y[TAB_SIZE];
	LineType int_lines_array[TAB_SIZE];

	switch (to_do) {
		case 1:
			scanf("%d %d", &rowsA, &colsA);
			read_mat(rowsA, colsA, A);
			scanf("%d %d", &rowsB, &colsB);
			read_mat(rowsB, colsB, B);
			prod_mat(rowsA, colsA, colsB, A, B, AB);
			print_mat(rowsA, colsB, AB);
			break;
		case 2:
			n = read_int() - 1; // we count from 1 :)
			ptr_array[0] = continuous_array;
			read_int_lines_cont(ptr_array);
			write_int_line_cont(ptr_array, n);
			break;
		case 3:
			n = read_int() - 1;
			read_char_lines(char_lines_array);
			write_char_line(char_lines_array, n);
			delete_char_lines(char_lines_array);
			break;
		case 4:
			n = read_int() - 1;
			lines_counter = read_int_lines(int_lines_array);
			sort_by_average(int_lines_array, lines_counter);
			write_int_line(int_lines_array, n);
			delete_int_lines(int_lines_array, lines_counter);
			break;
		case 5:
			scanf("%d %d %d", &rows, &cols, &n_triplets);
			read_sparse(triplet_array, n_triplets);
			read_vector(x, cols);
			make_CSR(triplet_array, n_triplets, rows, V, C, R);
			multiply_by_vector(rows, V, C, R, x, y);
			write_vector(V, n_triplets);
			write_vector(C, n_triplets);
			write_vector(R, rows + 1);
			write_vector(y, rows);
			break;
		default:
			printf("NOTHING TO DO FOR %d\n", to_do);
			break;
	}
	return 0;
}
