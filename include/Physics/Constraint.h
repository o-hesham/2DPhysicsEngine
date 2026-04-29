#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include "Physics/Body.h"
#include "Math/MatMN.h"

class Constraint
{
public:
    Body *a;
    Body *b;

    Vec2 aPoint;
    Vec2 bPoint;

    virtual ~Constraint() = default;

    MatMN GetInvM() const;
    VecN GetVelocities() const;

    virtual void PreSolve(const float dt) = 0;
    virtual void Solve() = 0;
    virtual void PostSolve() = 0;
};

class JointConstraint : public Constraint
{
private:
    MatMN Jacobian;
    VecN cachedLambda;
    float bias;

public:
    JointConstraint();
    JointConstraint(Body *a, Body *b, const Vec2 &anchorPoint);
    void PreSolve(const float dt) override;
    void Solve() override;
    void PostSolve() override;
};

class PenetrationConstraint : public Constraint
{
private:
    MatMN Jacobian;
    VecN cachedLambda;
    float bias;
    Vec2 normal;    // Normal direction of the penetration in A's local space
    float friction; // Fritction coefficient between the two penetrating bodies

public:
    PenetrationConstraint();
    PenetrationConstraint(Body *a, Body *b, const Vec2 &aCollisionPoint, const Vec2 &bCollisionPoint, const Vec2 &normal);
    void PreSolve(const float dt) override;
    void Solve() override;
    void PostSolve() override;
};

#endif