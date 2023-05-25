#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <cuda_runtime.h>

#define N 1024
#define BLOCK_SIZE 8

// Matrix dimensions
// (chosen as multiples of the thread block size for simplicity)
#define WA N // Matrix A width
#define HA N // Matrix A height
#define WB N // Matrix B width
#define HB WA  // Matrix B height
#define WC WB  // Matrix C width 
#define HC HA  // Matrix C height

//sequential code implemented on cpu
void computeGold(float* C, const float* A, const float* B, unsigned int hA, unsigned int wA, unsigned int wB)
{
    for (unsigned int i = 0; i < hA; ++i)
        for (unsigned int j = 0; j < wB; ++j)
		{
            double sum = 0;
            for (unsigned int k = 0; k < wA; ++k) 
			{
                double a = A[i * wA + k];
                double b = B[k * wB + j];
                sum += a * b;
            }
            C[i * wB + j] = (float)sum;
        }
}

// Initialize a matrix with random float entries.
void randomInit(float* data, int size)
{
    for (int i = 0; i < size; ++i)
        data[i] = rand() / (float)RAND_MAX;
}

//Compare the cpu's result with gpu's 
void printDiff(float *data1, float *data2, int width, int height)
{
  int i,j,k;
  int error_count=0;
  for (j=0; j<height; j++) 
  {
    for (i=0; i<width; i++) 
	{
      k = j*width+i;
      if (abs(data1[k] - data2[k]) > 1e-3) 
	  {
         error_count++;
      }
    }
  }
  printf("Total Errors = %d \n", error_count);
}

// matrix multiplication kernel on GPU
__global__ void matrixMul( float* C, float* A, float* B, int wA, int wB, int sign) // sign = 1: using shared memory, sign = 0: not using shared memory
{
     // Declaration of the shared memory array As used to
    // store the sub-matrix of A
    __shared__ float As[BLOCK_SIZE][BLOCK_SIZE];

    // Declaration of the shared memory array Bs used to
    // store the sub-matrix of B
    __shared__ float Bs[BLOCK_SIZE][BLOCK_SIZE];

	// Block index
    int bx = blockIdx.x;
    int by = blockIdx.y;

    // Thread index
    int tx = threadIdx.x;
    int ty = threadIdx.y;

    // Index of the first sub-matrix of A processed by the block
    int aBegin = wA * BLOCK_SIZE * by;

    // Index of the last sub-matrix of A processed by the block
    int aEnd   = aBegin + wA - 1;

    // Step size used to iterate through the sub-matrices of A
    int aStep  = BLOCK_SIZE;

    // Index of the first sub-matrix of B processed by the block
    int bBegin = BLOCK_SIZE * bx;

    // Step size used to iterate through the sub-matrices of B
    int bStep  = BLOCK_SIZE * wB;

    // Csub is used to store the element of the block sub-matrix
    // that is computed by the thread
    float Csub = 0;

    // Loop over all the sub-matrices of A and B
    // required to compute the block sub-matrix
    if (sign == 1)
    for (int a = aBegin, b = bBegin; a <= aEnd; a += aStep, b += bStep) 
	{
        // Load the matrices from device memory
        // to shared memory; each thread loads
        // one element of each matrix
        As[ty][tx] = A[a + wA * ty + tx];
        Bs[ty][tx] = B[b + wB * ty + tx];

        // Synchronize to make sure the matrices are loaded
        __syncthreads();

        // Multiply the two matrices together;
        // each thread computes one element
        // of the block sub-matrix
        for (int k = 0; k < BLOCK_SIZE; ++k)
            Csub += As[ty][k] * Bs[k][tx];

        // Synchronize to make sure that the preceding
        // computation is done before loading two new
        // sub-matrices of A and B in the next iteration
        __syncthreads();
    }
    else
    for (int a = aBegin, b = bBegin; a <= aEnd; a += aStep, b += bStep) 
    {
        for (int k = 0; k < BLOCK_SIZE; ++k)
            Csub += A[a + wA * ty + k] * B[b + wB * k + tx];

        // Synchronize to make sure that the preceding
        // computation is done before loading two new
        // sub-matrices of A and B in the next iteration
        __syncthreads();
    }

    // Write the block sub-matrix to device memory;
    // each thread writes one element
    int c = wB * BLOCK_SIZE * by + BLOCK_SIZE * bx;
    C[c + wB * ty + tx] = Csub;
}


int main(int argc, char **argv)
{
	// set seed for rand()
    srand((unsigned)time(NULL));

    // allocate host memory for matrices A and B
    unsigned int size_A = WA * HA;
    unsigned int mem_size_A = sizeof(float) * size_A;
    float* h_A = (float*) malloc(mem_size_A);
    unsigned int size_B = WB * HB;
    unsigned int mem_size_B = sizeof(float) * size_B;
    float* h_B = (float*) malloc(mem_size_B);
    unsigned int size_C = WC * HC;
    unsigned int mem_size_C = sizeof(float) * size_C;
    float* d_C;

    // initialize host memory
    randomInit(h_A, size_A);
    randomInit(h_B, size_B);

    // CPU
    // compute reference solution
    float* reference = (float*) malloc(mem_size_C);
    double cpu_timer = 0;
    double begin,end;
    begin = clock();
    computeGold(reference, h_A, h_B, HA, WA, WB);
    end = clock();
    cpu_timer = (double)(end - begin)/CLOCKS_PER_SEC;

    // print timers
    printf("CPU Processing time: %f (s) \n", cpu_timer);

    // GPU
    // allocate device memory
    float* d_A;
    cudaMalloc((void**) &d_A, mem_size_A);
    float* d_B;
    cudaMalloc((void**) &d_B, mem_size_B);

    // copy host memory to device
    cudaMemcpy(d_A, h_A, mem_size_A, cudaMemcpyHostToDevice) ;
    cudaMemcpy(d_B, h_B, mem_size_B, cudaMemcpyHostToDevice) ;

    // allocate device memory for result
    cudaMalloc((void**) &d_C, mem_size_C);

    // allocate host memory for the result
    float* h_C = (float*) malloc(mem_size_C);
    
    // create and start gpu_timer by cuda_event
    float gpu_timer = 0;
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    // setup execution parameters
    dim3 threads(BLOCK_SIZE, BLOCK_SIZE);
    dim3 grid(WC / threads.x, HC / threads.y);

    // execute the kernel -- mode 1
    cudaEventRecord(start, 0);
    matrixMul<<< grid, threads >>>(d_C, d_A, d_B, WA, WB, 1);
    cudaEventRecord(stop, 0);
	cudaEventSynchronize(stop);

    // stop and destroy gpu_timer
    cudaEventElapsedTime(&gpu_timer, start, stop);
    printf("GPU Processing time with shared mem: %f (s) \n", gpu_timer/1000);

    // copy result from device to host
    cudaMemcpy(h_C, d_C, mem_size_C, cudaMemcpyDeviceToHost);
    printDiff(reference, h_C, WC, HC);

    // execute the kernel -- mode 1
    cudaEventRecord(start, 0);
    matrixMul<<< grid, threads >>>(d_C, d_A, d_B, WA, WB, 0);
    cudaEventRecord(stop, 0);
	cudaEventSynchronize(stop);

    // stop and destroy gpu_timer
    cudaEventElapsedTime(&gpu_timer, start, stop);
    printf("GPU Processing time without shared mem: %f (s) \n", gpu_timer/1000);

    // copy result from device to host
    cudaMemcpy(h_C, d_C, mem_size_C, cudaMemcpyDeviceToHost);
    printDiff(reference, h_C, WC, HC);

    // clean up memory
    free(h_A);
    free(h_B);
    free(h_C);
    free(reference);
    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);
    cudaEventDestroy(start);
    cudaEventDestroy(stop);

}