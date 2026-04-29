#ifndef VECN_H
#define VECN_H
#include <memory>

struct VecN
{
    int N;
    std::unique_ptr<float[]> data;

    VecN();
    VecN(int N);
    VecN(const VecN &v);

    void Zero();
    float Dot(const VecN &rhs) const;

    VecN operator=(const VecN &rhs);
    VecN operator+(const VecN &rhs) const;
    VecN operator-(const VecN &rhs) const;
    VecN operator*(const float n) const;

    const VecN &operator+=(const VecN &rhs);
    const VecN &operator-=(const VecN &rhs);
    const VecN &operator*=(const float n);

    float operator[](const int index) const;
    float &operator[](const int index);
};

#endif