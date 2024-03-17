#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

// Define a struct to hold the data
typedef struct
{
    int rows;
    int cols;
    int **data;
} Matrix;

// Define a struct that is passed to the thread function
typedef struct attributes
{
    int r1;
    int c1;
    int c2;
    Matrix *m1;
    Matrix *m2;
    Matrix *res;

} Attributes;


void writeMatrixToFile(const char *filename, Matrix *res, unsigned long start, unsigned long stop, int numt);
Matrix *readMatrixFromFile(const char *filename);
void freeMatrix(Matrix *matrix);
int **allocateMatrix(int rows, int cols);
void thread_per_matrix_method(Matrix *m1, Matrix *m2, Matrix *res);
void thread_per_row_method(Matrix *m1, Matrix *m2, Matrix *res);
void thread_per_element_method(Matrix *m1, Matrix *m2, Matrix *res);
void *calculate_row(void *ptr);
void *calculate_element(void *ptr);

int main()
{
    // Read matrices and init result matrix
    const char *filename1 = "matrix1.txt";
    const char *filename2 = "matrix2.txt";
    Matrix *matrix1 = readMatrixFromFile(filename1);
    Matrix *matrix2 = readMatrixFromFile(filename2);
    Matrix *res = (Matrix *)calloc(1, sizeof(Matrix));
    res->rows = matrix1->rows;
    res->cols = matrix2->cols;
    res->data = allocateMatrix(res->rows, res->cols);

    struct timeval stop, start;

    // First method single thread
    gettimeofday(&start, NULL); // start checking time
    thread_per_matrix_method(matrix1, matrix2, res);
    gettimeofday(&stop, NULL); // end checking time

    writeMatrixToFile("thread_per_matrix.txt", res, start.tv_usec, stop.tv_usec, 1);

    // Second method thread per row
    gettimeofday(&start, NULL); // start checking time
    thread_per_row_method(matrix1, matrix2, res);
    gettimeofday(&stop, NULL); // end checking time

    writeMatrixToFile("thread_per_row.txt", res, start.tv_usec, stop.tv_usec, res->rows);

    // Third method thread per element
    gettimeofday(&start, NULL); // start checking time
    thread_per_element_method(matrix1, matrix2, res);
    gettimeofday(&stop, NULL); // end checking time

    writeMatrixToFile("thread_per_element.txt", res, start.tv_usec, stop.tv_usec, res->rows * res->cols);

    freeMatrix(matrix1);
    freeMatrix(matrix2);
    freeMatrix(res);
}

// First method
void thread_per_matrix_method(Matrix *m1, Matrix *m2, Matrix *res)
{
    for (int i = 0; i < m1->rows; i++)
    {
        for (int j = 0; j < m2->cols; j++)
        {
            for (int k = 0; k < m1->cols; k++)
            {
                res->data[i][j] += m1->data[i][k] * m2->data[k][j];
            }
        }
    }
}

// Second method
void thread_per_row_method(Matrix *m1, Matrix *m2, Matrix *res)
{
    pthread_t thread[m1->rows];
    Attributes **attributesArray = malloc(m1->rows * sizeof(Attributes*));
    for (int i = 0; i < m1->rows; i++)
    {
        Attributes *a = malloc(sizeof(Attributes));
        a->r1 = i;
        a->c2 = m2->cols;
        a->c1 = m1->cols;
        a->m1 = m1;
        a->m2 = m2;
        a->res = res;
        attributesArray[i] = a;
        pthread_create(&thread[i], NULL, calculate_row, (void *)a);
    }

    for (int i = 0; i < m1->rows; i++)
    {
        pthread_join(thread[i], NULL);
        free(attributesArray[i]);
    }
}

void *calculate_row(void *ptr)
{
    Attributes *a = (Attributes *)ptr;
    for (int j = 0; j < a->c2; j++)
    {
        for (int k = 0; k < a->c1; k++)
        {
            a->res->data[a->r1][j] += a->m1->data[a->r1][k] * a->m2->data[k][j];
        }
    }
    return NULL;
}

// Third method
void thread_per_element_method(Matrix *m1, Matrix *m2, Matrix *res)
{
    pthread_t thread[m1->rows][m2->cols];
    Attributes ***attributesArray = malloc(m1->rows * sizeof(Attributes**));
    for (int i = 0; i < m1->rows; i++)
    {
        attributesArray[i] = malloc(m2->cols * sizeof(Attributes*));
        for (int j = 0; j < m2->cols; j++)
        {
            Attributes *a = malloc(sizeof(Attributes));
            a->r1 = i;
            a->c2 = j;
            a->c1 = m1->cols;
            a->m1 = m1;
            a->m2 = m2;
            a->res = res;
            attributesArray[i][j] = a;
            pthread_create(&thread[i][j], NULL, calculate_element, (void *)a);
        }
    }

    for (int i = 0; i < m1->rows; i++)
    {
        for (int j = 0; j < m2->cols; j++)
        {
            pthread_join(thread[i][j], NULL);
            free(attributesArray[i][j]);
        }
    }
}

void *calculate_element(void *ptr)
{
    Attributes *a = (Attributes *)ptr;
    for (int k = 0; k < a->c1; k++)
    {
        a->res->data[a->r1][a->c2] += a->m1->data[a->r1][k] * a->m2->data[k][a->c2];
    }
    return NULL;
}

// Reading from file code
// Function to allocate memory for the matrix
int **allocateMatrix(int rows, int cols)
{
    int **matrix = (int **)calloc(rows, sizeof(int *));
    for (int i = 0; i < rows; i++)
    {
        matrix[i] = (int *)calloc(cols, sizeof(int));
    }
    return matrix;
}

// Function to free memory allocated for the matrix
void freeMatrix(Matrix *matrix)
{
    for (int i = 0; i < matrix->rows; i++)
    {
        free(matrix->data[i]);
    }
    free(matrix->data);
    free(matrix);
}

// Function to read matrix from file
Matrix *readMatrixFromFile(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        printf("Error opening file %s\n", filename);
        return NULL;
    }

    Matrix *matrix = (Matrix *)malloc(sizeof(Matrix));
    if (!matrix)
    {
        printf("Memory allocation failed\n");
        fclose(file);
        return NULL;
    }

    // Read rows and cols from file
    fscanf(file, "row=%d col=%d\n", &matrix->rows, &matrix->cols);

    // Allocate memory for the matrix data
    matrix->data = allocateMatrix(matrix->rows, matrix->cols);

    // Read matrix elements from file
    for (int i = 0; i < matrix->rows; i++)
    {
        for (int j = 0; j < matrix->cols; j++)
        {
            fscanf(file, "%d", &matrix->data[i][j]);
        }
    }

    fclose(file);
    return matrix;
}

// Function to write matrix to file
void writeMatrixToFile(const char *filename, Matrix *res, unsigned long start, unsigned long stop, int numt)
{
    FILE *file;
    file = fopen(filename, "w");

    if (file == NULL)
    {
        fprintf(stderr, "Unable to open file\n");
    }
    else
    {
        // Write time and thread number of threads
        fprintf(file, "\nnumber of threads %d \n", numt);
        fprintf(file, "Microseconds taken: %lu\n\n", stop - start);

        // Write matrix to file
        fprintf(file, "Matrix (%d x %d):\n", res->rows, res->cols);
        for (int i = 0; i < res->rows; i++)
        {
            for (int j = 0; j < res->cols; j++)
            {
                fprintf(file, "%d ", res->data[i][j]);
                res->data[i][j] = 0; // Empty the matrix after printing.
            }
            fprintf(file, "\n");
        }
        // Close the file
        fclose(file);
    }
}
