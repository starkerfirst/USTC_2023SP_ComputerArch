#include <cstdio>
#include <random>

#include <gem5/m5ops.h>

void daxpy(double *X, double *Y, double alpha, const int N)
{
    for (int i = 0; i < N; i++)
    {
        Y[i] = alpha * X[i] + Y[i];
    }
}

void daxsbxpxy(double *X, double *Y, double alpha, double beta, const int N)
{
    for (int i = 0; i < N; i++)
    {
        Y[i] = alpha * X[i] * X[i] + beta * X[i] + X[i] * Y[i];
    }
}

void stencil(double *Y, double alpha, const int N)
{
    for (int i = 1; i < N-1; i++)
    {
        Y[i] = alpha * Y[i-1] + Y[i] + alpha * Y[i+1];
    }
}


void daxpy_unroll(double *X, double *Y, double alpha, const int N)
{
    for (int i = 0; i < N/4; i++)
    {
        Y[4*i] = alpha * X[4*i] + Y[4*i];
        Y[4*i+1] = alpha * X[4*i+1] + Y[4*i+1];
        Y[4*i+2] = alpha * X[4*i+2] + Y[4*i+2];
        Y[4*i+3] = alpha * X[4*i+3] + Y[4*i+3];
    }

    for (int i = (N/4)*4; i < N; i++)
    {
        Y[i] = alpha * X[i] + Y[i];
    }
}

void daxsbxpxy_unroll(double *X, double *Y, double alpha, double beta, const int N)
{
    for (int i = 0; i < N/4; i++)
    {
        Y[4*i] = alpha * X[4*i] * X[4*i] + beta * X[4*i] + X[4*i] * Y[4*i];
        Y[4*i+1] = alpha * X[4*i+1] * X[4*i+1] + beta * X[4*i+1] + X[4*i+1] * Y[4*i+1];
        Y[4*i+2] = alpha * X[4*i+2] * X[4*i+2] + beta * X[4*i+2] + X[4*i+2] * Y[4*i+2];
        Y[4*i+3] = alpha * X[4*i+3] * X[4*i+3] + beta * X[4*i+3] + X[4*i+3] * Y[4*i+3];
    }

    for (int i = 4*(N/4); i < N; i++)
    {
        Y[i] = alpha * X[i] * X[i] + beta * X[i] + X[i] * Y[i];
    }
}

void stencil_unroll(double *Y, double alpha, const int N)
{
    for (int i = 0; i < (N-1)/4; i++)
    {
        Y[4*i+1] = alpha * Y[4*i] + Y[4*i+1] + alpha * Y[4*i+2];
        Y[4*i+2] = alpha * Y[4*i+1] + Y[4*i+2] + alpha * Y[4*i+3];
        Y[4*i+3] = alpha * Y[4*i+2] + Y[4*i+3] + alpha * Y[4*i+4];
        Y[4*i+4] = alpha * Y[4*i+3] + Y[4*i+4] + alpha * Y[4*i+5];
    }

    for (int i = 4*((N-1)/4) + 1; i < N-1; i++)
    {
        Y[i] = alpha * Y[i-1] + Y[i] + alpha * Y[i+1];
    }
}

int main()
{
    const int N = 10000;
    double *X = new double[N], *Y = new double[N], alpha = 0.5, beta = 0.1;

    //std::random_device rd;
    std::mt19937 gen(0);
    std::uniform_real_distribution<> dis(1, 2);
    for (int i = 0; i < N; ++i)
    {
        X[i] = dis(gen);
        Y[i] = dis(gen);
    }

    m5_dump_reset_stats(0, 0); // gem5 ABI
    daxpy(X, Y, alpha, N);
    m5_dump_reset_stats(0, 0);
    daxpy_unroll(X, Y, alpha, N);
    m5_dump_reset_stats(0, 0);
    daxsbxpxy(X, Y, alpha, beta, N);
    m5_dump_reset_stats(0, 0);
    daxsbxpxy_unroll(X, Y, alpha, beta, N);
    m5_dump_reset_stats(0, 0);
    stencil(Y, alpha, N);
    m5_dump_reset_stats(0, 0);
    stencil_unroll(Y, alpha, N);
    m5_dump_reset_stats(0, 0);

    double sum = 0;
    for (int i = 0; i < N; ++i)
    {
        sum += Y[i];
    }
    printf("%lf\n", sum);
    return 0;
}
