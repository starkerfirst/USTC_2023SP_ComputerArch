#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <immintrin.h>
#include <math.h>
#include <string.h>

int N = (1 << SIZE); // SIZE comes from gcc macro definition
int block_size = (1 << BLOCK_SIZE);
void gemm_baseline(float *A, float *B, float *C); 
void gemm_verify(float *A, float *B, float *C); 
void gemm_avx(float *A, float *B, float *C); 
void gemm_avx_block(float *A, float *B, float *C); 

int main(void) {
    printf("N = %d blocksize = %d:\n", N, block_size);
    // malloc A, B, C
    // MUST align to 32 bytes
    float* A = (float*)aligned_alloc(32, N * N * sizeof(float));
    float* B = (float*)aligned_alloc(32, N * N * sizeof(float));
    float* C = (float*)aligned_alloc(32, N * N * sizeof(float));

    // random initialize A, B
    // srand(time(NULL));
    for (int i = 0; i < N * N; i++) {
        A[i] = (float)rand() / RAND_MAX;
        B[i] = (float)rand() / RAND_MAX;
    }

    // baseline
    clock_t start = clock();
    gemm_baseline(A, B, C);
    clock_t end = clock();
    printf("baseline elapsed time: %f sec\n", (double)(end - start) / CLOCKS_PER_SEC);
    
    // avx
    float * B_trans = (float*)aligned_alloc(32, N * N * sizeof(float));
    memcpy(B_trans, B, N * N * sizeof(float));
    start = clock();
    gemm_avx(A, B_trans, C);
    end = clock();
    printf("avx elapsed time: %f sec\n", (double)(end - start) / CLOCKS_PER_SEC);
    // verify
    gemm_verify(A, B, C);

    // avx block
    memcpy(B_trans, B, N * N * sizeof(float));
    memset(C, 0, N * N * sizeof(float));
    start = clock();
    gemm_avx_block(A, B_trans, C);
    end = clock();
    printf("avx block elapsed time: %f sec\n", (double)(end - start) / CLOCKS_PER_SEC);
    // verify
    gemm_verify(A, B, C);

    putchar('\n');
    free(A);
    free(B);
    free(C);

}

void print_matrix(float *A) {
    for (int i = 0; i < N * N; i++) {
        printf("%f ", A[i]);
        if ((i + 1) % N == 0) {
            putchar('\n');
        }
    }
}

void gemm_baseline(float *A, float *B, float *C) {
    // tradition matrix multiplication
    for (int i = 0; i < N; i++) { // row
        for (int j = 0; j < N; j++) { // column
            float sum = 0;
            for (int k = 0; k < N; k++) { // element
                sum += A[i * N + k] * B[k * N + j];
            }
            C[i * N + j] = sum;
        }
    }
}

void gemm_verify(float *A, float *B, float *C) {
    // verify
    float* C_verify = (float*)aligned_alloc(32, N * N * sizeof(float));
    gemm_baseline(A, B, C_verify);
    for (int i = 0; i < N * N; i++) {
        if (abs(C[i] - C_verify[i]) > 1e-4){
            printf("no.%d verification failed, with C is %f and C_verify is %f \n", i, C[i], C_verify[i]);
            // printf("A:\n");
            // print_matrix(A);
            // printf("B:\n");
            // print_matrix(B);
            // printf("C:\n");
            // print_matrix(C);
            // printf("C_verify:\n");
            // print_matrix(C_verify);
            return;
        }
    }
    printf("verification passed!\n");
    free(C_verify);
}

void gemm_avx(float *A, float *B, float *C) {
    // transpose B
    for (int i = 0; i < N; i++) { // row
        for (int j = i + 1; j < N; j++) { // column
            float tmp = B[i * N + j];
            B[i * N + j] = B[j * N + i];
            B[j * N + i] = tmp;
        }
    }

    // avx
    for (int i = 0; i < N; i++) { // row
        for (int j = 0; j < N; j++) { // column
            __m256 sum = _mm256_setzero_ps(); // 8 floats -> expect speedup 8x
            for (int k = 0; k < N; k += 8) { // element
                __m256 a = _mm256_load_ps(A + i * N + k);
                __m256 b = _mm256_load_ps(B + j * N + k);
                sum = _mm256_add_ps(sum, _mm256_mul_ps(a, b));
            }
            float* sum_arr = (float*)aligned_alloc(32, 8 * sizeof(float));
            _mm256_store_ps(sum_arr, sum);
            C[i * N + j] = sum_arr[0] + sum_arr[1] + sum_arr[2] + sum_arr[3] + sum_arr[4] + sum_arr[5] + sum_arr[6] + sum_arr[7];
        }
    }
}

void gemm_avx_block(float *A, float *B, float *C) { // with BLOCK_SIZE macro
    // transpose B
    for (int i = 0; i < N; i++) { // row
        for (int j = i + 1; j < N; j++) { // column
            float tmp = B[i * N + j];
            B[i * N + j] = B[j * N + i];
            B[j * N + i] = tmp;
        }
    }

    // avx block
    for (int i = 0; i < N; i += block_size) { // row
        for (int j = 0; j < N; j += block_size) { // column
            for (int k = 0; k < N; k += block_size) { // element
                for (int ii = i; ii < i + block_size; ii++) {
                    for (int jj = j; jj < j + block_size; jj++) {
                        __m256 sum = _mm256_setzero_ps(); // 8 floats -> expect speedup 8x
                        for (int kk = k; kk < k + block_size; kk += 8) {
                            __m256 a = _mm256_load_ps(A + ii * N + kk);
                            __m256 b = _mm256_load_ps(B + jj * N + kk);
                            sum = _mm256_add_ps(sum, _mm256_mul_ps(a, b));
                        }
                        float* sum_arr = (float*)aligned_alloc(32, 8 * sizeof(float));
                        _mm256_store_ps(sum_arr, sum);
                        C[ii * N + jj] += sum_arr[0] + sum_arr[1] + sum_arr[2] + sum_arr[3] + sum_arr[4] + sum_arr[5] + sum_arr[6] + sum_arr[7];
                        free(sum_arr);
                    }
                }
            }
        }
    }
}

