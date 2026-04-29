#include "math/MatMN.h"

MatMN::MatMN()
    : M(0), N(0)
{
}

MatMN::MatMN(int M, int N)
    : M(M), N(N)
{
    rows = std::make_unique<VecN[]>(M);
    for (int i = 0; i < M; i++)
    {
        rows[i] = VecN(N);
    }
}

MatMN::MatMN(const MatMN &m)
{
    *this = m;
}

void MatMN::Zero()
{
    for (int i = 0; i < M; i++)
    {
        rows[i].Zero();
    }
}

MatMN MatMN::Transpose() const
{
    MatMN result(N, M);
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            result.rows[j][i] = rows[i][j];
    return result;
}

const MatMN &MatMN::operator=(const MatMN &rhs)
{
    M = rhs.M;
    N = rhs.N;

    rows = std::make_unique<VecN[]>(M);
    for (int i = 0; i < M; i++)
    {
        rows[i] = rhs.rows[i];
    }

    return *this;
}

VecN MatMN::operator*(const VecN &rhs) const
{
    if (rhs.N != N)
    {
        return rhs;
    }
    VecN result(M);
    for (int i = 0; i < M; i++)
    {
        result[i] = rhs.Dot(rows[i]);
    }
    return result;
}

MatMN MatMN::operator*(const MatMN &rhs) const
{
    if (rhs.M != N && rhs.N != M)
    {
        return rhs;
    }
    MatMN transposed = rhs.Transpose();
    MatMN result(M, rhs.N);
    for (int i = 0; i < M; i++)
        for (int j = 0; j < rhs.N; j++)
            result.rows[i][j] = rows[i].Dot(transposed.rows[j]);
    return result;
}

VecN MatMN::SolveGaussSeidel(const MatMN &A, const VecN &b)
{
    const int N = b.N;
    VecN X(N);
    X.Zero();

    for (int iterations = 0; iterations < N; iterations++)
    {
        for (int i = 0; i < N; i++)
        {
            float dx = (b[i] / A.rows[i][i]) - (A.rows[i].Dot(X) / A.rows[i][i]);
            if (dx == dx)
            {
                X[i] += dx;
            }
        }
    }
    return X;
}