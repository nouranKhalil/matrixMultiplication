# Matrix Multiplication with Threads

This project implements matrix multiplication using multiple threads in C. It explores three different threading techniques:

1. Thread per matrix
2. Thread per row
3. Thread per element

## File Descriptions

1. **threads.c**: The main C source code file containing the implementation of matrix multiplication using threads.
2. **thread_per_matrix.txt**: Resulting matrix and time taken for multiplication using the thread-per-matrix technique.
3. **thread_per_row.txt**: Resulting matrix and time taken for multiplication using the thread-per-row technique.
4. **thread_per_element.txt**: Resulting matrix and time taken for multiplication using the thread-per-element technique.

## Dependencies

- pthread library for threading functionalities.

## Running the Code

To compile and run the code, execute the following command:

```bash
gcc threads.c -o threads -pthread && ./threads

