#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <emscripten/emscripten.h>





float get_time_on_milliseconds() {
    float time = emscripten_get_now();
    printf("Time: %f\n", time);
    return time;
}

int generate_random_number() {
    return rand() % 100;
}

void generate_matrix(int rows, int cols, int **matrix) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            matrix[i][j] = generate_random_number();
        }
    }
}

typedef struct {
    int start_row;
    int end_row;
    int rows;
    int cols;
    int **matriz_a;
    int **matriz_b;
    int **resultado;
} ThreadData;

void *multiply_partial(void *arg) {
    ThreadData *data = (ThreadData *)arg;

    for (int i = data->start_row; i < data->end_row; i++) {
        for (int j = 0; j < data->rows; j++) {
            data->resultado[i][j] = 0;
            for (int k = 0; k < data->cols; k++) {
                data->resultado[i][j] += data->matriz_a[i][k] * data->matriz_b[k][j];
            }
        }
    }
    return NULL;
}

int **allocate_matrix(int rows, int cols) {
    int **matrix = malloc(rows * sizeof(int *));
    for (int i = 0; i < rows; i++) {
        matrix[i] = malloc(cols * sizeof(int));
    }
    return matrix;
}

void free_matrix(int **matrix, int rows) {
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

EMSCRIPTEN_KEEPALIVE
float calculate_matrices(int rows, int cols, int num_threads) {
    float tiempo_inicial = get_time_on_milliseconds();

    int **matriz_a = allocate_matrix(rows, cols);
    int **matriz_b = allocate_matrix(cols, rows);
    int **resultado = allocate_matrix(rows, rows);

    generate_matrix(rows, cols, matriz_a);
    generate_matrix(cols, rows, matriz_b);



    pthread_t threads[num_threads];
    ThreadData thread_data[num_threads];
    int rows_per_thread = rows / num_threads;

    for (int i = 0; i < num_threads; i++) {
        thread_data[i].start_row = i * rows_per_thread;
        thread_data[i].end_row = (i == num_threads - 1) ? rows : (i + 1) * rows_per_thread;
        thread_data[i].rows = rows;
        thread_data[i].cols = cols;
        thread_data[i].matriz_a = matriz_a;
        thread_data[i].matriz_b = matriz_b;
        thread_data[i].resultado = resultado;

        pthread_create(&threads[i], NULL, multiply_partial, &thread_data[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }


    free_matrix(matriz_a, rows);
    free_matrix(matriz_b, cols);
    free_matrix(resultado, rows);
    
    float tiempo_final = get_time_on_milliseconds();
    float execution_time = tiempo_final - tiempo_inicial;
    printf("Tiempo de ejecución: %f ms\n", execution_time);
    return execution_time;
}

// Función para obtener el tiempo de ejecución


// emcc module.c -s USE_PTHREADS=1 -s PTHREAD_POOL_SIZE=20 \
-s EXPORTED_FUNCTIONS="['_calculate_matrices']" \
-s EXPORTED_RUNTIME_METHODS="['ccall']" -o module.js