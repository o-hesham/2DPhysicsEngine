#ifndef VEC2_H
#define VEC2_H
#include <iostream>

struct Vec2
{
    float x;
    float y;

    Vec2();
    Vec2(float x, float y);
    ~Vec2() = default;

    void Add(const Vec2 &v);
    void Sub(const Vec2 &v);
    void Scale(const float n);
    Vec2 Rotate(const float angle) const;

    float Magnitude() const;
    float MagnitudeSquared() const;

    Vec2 &Normalize();
    Vec2 UnitVector() const;
    Vec2 Normal() const;

    float Dot(const Vec2 &rhs) const;
    float Cross(const Vec2 &rhs) const;

    Vec2 &operator=(const Vec2 &rhs);
    bool operator==(const Vec2 &rhs) const;
    bool operator!=(const Vec2 &rhs) const;

    Vec2 operator+(const Vec2 &rhs) const;
    Vec2 operator-(const Vec2 &rhs) const;
    Vec2 operator*(const float n) const;
    Vec2 operator/(const float n) const;
    Vec2 operator-();

    Vec2 operator+=(const Vec2 &rhs);
    Vec2 operator-=(const Vec2 &rhs);
    Vec2 operator*=(const float n);
    Vec2 operator/=(const float n);
    // friend std::ostream &operator<<(std::ostream &os, const Vec2 &v);
};

#endif