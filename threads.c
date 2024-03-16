#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

// int res[2][2];
// int arr1[][2] = {{1, 2}, {1, 2}};
// int arr2[][2] = {{1, 2}, {1, 2}};

int res[10][10];
int arr1[][5] = {{1, 2, 3, 4, 5},
                 {6, 7, 8, 9, 10},
                 {11, 12, 13, 14, 15},
                 {16, 17, 18, 19, 20},
                 {21, 22, 23, 24, 25},
                 {26, 27, 28, 29, 30},
                 {31, 32, 33, 34, 35},
                 {36, 37, 38, 39, 40},
                 {41, 42, 43, 44, 45},
                 {46, 47, 48, 49, 50}};
int arr2[][10] = {
    {1, 2, 3, 4, 5, 6, 7, 8, 9, 10},
    {11, 12, 13, 14, 15, 16, 17, 18, 19, 20},
    {21, 22, 23, 24, 25, 26, 27, 28, 29, 30},
    {31, 32, 33, 34, 35, 36, 37, 38, 39, 40},
    {41, 42, 43, 44, 45, 46, 47, 48, 49, 50}};

typedef struct attributes
{
    int r1;
    int c1;
    int r2;
    int c2;

} Attributes;

void thread_per_matrix_method(int r1, int c1, int r2, int c2);
void thread_per_row_method(int r1, int c1, int r2, int c2);
void thread_per_element_method(int r1, int c1, int r2, int c2);
void *calculate_row(void *ptr);
void *calculate_element(void *ptr);

int main()
{
    struct timeval stop, start;

    gettimeofday(&start, NULL); // start checking time
    thread_per_matrix_method(10, 5, 5, 10);
    gettimeofday(&stop, NULL); // end checking time

    printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);

    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            printf("%d\t", res[i][j]);
            res[i][j] = 0;
        }
        printf("\n");
    }

    gettimeofday(&start, NULL); // start checking time
    thread_per_row_method(10, 5, 5, 10);
    gettimeofday(&stop, NULL); // end checking time

    printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);

    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            printf("%d\t", res[i][j]);
            res[i][j] = 0;
        }
        printf("\n");
    }

    gettimeofday(&start, NULL); // start checking time
    thread_per_element_method(10, 5, 5, 10);
    gettimeofday(&stop, NULL); // end checking time

    printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);

    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            printf("%d\t", res[i][j]);
            res[i][j] = 0;
        }
        printf("\n");
    }
}

void thread_per_matrix_method(int r1, int c1, int r2, int c2)
{
    for (int i = 0; i < r1; i++)
    {
        for (int j = 0; j < c2; j++)
        {
            for (int k = 0; k < c1; k++)
            {
                res[i][j] += arr1[i][k] * arr2[k][j];
            }
        }
    }
}

void thread_per_row_method(int r1, int c1, int r2, int c2)
{
    pthread_t thread[r1];
    for (int i = 0; i < r1; i++)
    {
        Attributes *a = malloc(sizeof(Attributes));
        a->r1 = i;
        a->c2 = c2;
        a->c1 = c1;
        pthread_create(&thread[i], NULL, calculate_row, (void *)a);
    }

    for (int i = 0; i < r1; i++)
    {
        pthread_join(thread[i], NULL);
    }
}

void *calculate_row(void *ptr)
{
    Attributes *a = (Attributes *)ptr;
    for (int j = 0; j < a->c2; j++)
    {
        for (int k = 0; k < a->c1; k++)
        {
            res[a->r1][j] += arr1[a->r1][k] * arr2[k][j];
        }
    }
    return NULL;
}

void thread_per_element_method(int r1, int c1, int r2, int c2)
{
    pthread_t thread[r1][c2];
    for (int i = 0; i < r1; i++)
    {
        for (int j = 0; j < c2; j++)
        {
            Attributes *a = malloc(sizeof(Attributes));
            a->r1 = i;
            a->c2 = j;
            a->c1 = c1;
            pthread_create(&thread[i][j], NULL, calculate_element, (void *)a);
        }
    }

    for (int i = 0; i < r1; i++)
    {
        for (int j = 0; j < c2; j++)
        {
            pthread_join(thread[i][j], NULL);
        }
    }
}

void *calculate_element(void *ptr)
{
    Attributes *a = (Attributes *)ptr;
    for (int k = 0; k < a->c1; k++)
    {
        res[a->r1][a->c2] += arr1[a->r1][k] * arr2[k][a->c2];
    }
    return NULL;
}
