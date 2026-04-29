#include "Math/VecN.h"
#include <algorithm>
#include <stdexcept>

VecN::VecN()
    : N(0) {};

VecN::VecN(int N)
    : N(N), data(std::make_unique<float[]>(N)) {};

VecN::VecN(const VecN &v)
    : N(v.N), data(std::make_unique<float[]>(v.N))
{
    std::copy(v.data.get(), v.data.get() + v.N, data.get());
}

void VecN::Zero()
{
    std::fill(data.get(), data.get() + N, 0.0f);
}

float VecN::Dot(const VecN &v) const
{
    float sum = 0.0f;
    for (int i = 0; i < N; i++)
    {
        sum += data[i] * v.data[i];
    }
    return sum;
}

VecN VecN::operator=(const VecN &rhs)
{
    if (this == &rhs)
    {
        return *this;
    }
    N = rhs.N;
    data = std::make_unique<float[]>(N);
    std::copy(rhs.data.get(), rhs.data.get() + N, data.get());

    return *this;
}

VecN VecN::operator+(const VecN &rhs) const
{
    VecN result(*this);
    result += rhs;

    return result;
}

VecN VecN::operator-(const VecN &rhs) const
{
    VecN result(*this);
    result -= rhs;

    return result;
}

VecN VecN::operator*(const float n) const
{
    VecN result(N);
    for (int i = 0; i < N; i++)
    {
        result[i] = data[i] * n;
    }
    return result;
}

const VecN &VecN::operator+=(const VecN &rhs)
{
    if (N != rhs.N)
    {
        throw std::invalid_argument("Cannot add vectors of different dimensions");
    }

    for (int i = 0; i < N; i++)
    {
        data[i] += rhs.data[i];
    }
    return *this;
}

const VecN &VecN::operator-=(const VecN &rhs)
{
    if (N != rhs.N)
    {
        throw std::invalid_argument("Cannot subtract vectors of different dimensions");
    }

    for (int i = 0; i < N; i++)
    {
        data[i] -= rhs.data[i];
    }
    return *this;
}

const VecN &VecN::operator*=(const float n)
{
    for (int i = 0; i < N; i++)
    {
        data[i] *= n;
    }
    return *this;
}

float VecN::operator[](const int index) const
{
    return data[index];
}

float &VecN::operator[](const int index)
{
    return data[index];
}