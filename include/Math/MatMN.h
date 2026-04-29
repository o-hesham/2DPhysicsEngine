#ifndef MATMN_H
#define MATMN_H

#include "Math/VecN.h"

struct MatMN
{
    int M; // rows
    int N; // cols

    std::unique_ptr<VecN[]> rows; // rows of the matrix with N columns inside

    MatMN();
    MatMN(int M, int N);
    MatMN(const MatMN &m);

    void Zero();
    MatMN Transpose() const;

    const MatMN &operator=(const MatMN &rhs);
    VecN operator*(const VecN &rhs) const;
    MatMN operator*(const MatMN &rhs) const;

    static VecN SolveGaussSeidel(const MatMN &A, const VecN &b);
};

#endif