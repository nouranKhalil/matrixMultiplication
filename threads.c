#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int res[2][2];
int arr1[][2] = {{1, 2}, {1, 2}};
int arr2[][2] = {{1, 2}, {1, 2}};

typedef struct attributes
{
    int r1;
    int c1;
    int r2;
    int c2;

} Attributes;

void thread_per_matrix_method(int r1, int c1, int arr1[][c1], int r2, int c2, int arr2[][c2]);
void thread_per_row_method(int r1, int c1, int r2, int c2);
void thread_per_element_method(int r1, int c1, int r2, int c2);
void *calculate_row(void *ptr);
void *calculate_element(void *ptr);

int main()
{

    // thread_per_matrix_method(2,2,arr1,2,2,arr2);
    // thread_per_row_method(2, 2, 2, 2);
    thread_per_element_method(2,2,2,2);

    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            printf("%d\t", res[i][j]);
        }
        printf("\n");
    }
}

void thread_per_matrix_method(int r1, int c1, int arr1[][c1], int r2, int c2, int arr2[][c2])
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
