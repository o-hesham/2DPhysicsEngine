#include "Physics/Constraint.h"
#include <algorithm>

///////////////Constraint///////////////

///////////////////////////////////////////////////////////////////////////////
// Mat6x6 with the all inverse mass and inverse I of bodies "a" and "b"
///////////////////////////////////////////////////////////////////////////////
//  [ 1/ma  0     0     0     0     0    ]
//  [ 0     1/ma  0     0     0     0    ]
//  [ 0     0     1/Ia  0     0     0    ]
//  [ 0     0     0     1/mb  0     0    ]
//  [ 0     0     0     0     1/mb  0    ]
//  [ 0     0     0     0     0     1/Ib ]
///////////////////////////////////////////////////////////////////////////////
MatMN Constraint::GetInvM() const
{
    MatMN invM(6, 6);

    invM.Zero();

    invM.rows[0][0] = a->invMass;
    invM.rows[1][1] = a->invMass;
    invM.rows[2][2] = a->invI;
    invM.rows[3][3] = b->invMass;
    invM.rows[4][4] = b->invMass;
    invM.rows[5][5] = b->invI;

    return invM;
}

///////////////////////////////////////////////////////////////////////////////
// VecN with the all linear and angular velocities of bodies "a" and "b"
///////////////////////////////////////////////////////////////////////////////
//  [ va.x ]
//  [ va.y ]
//  [ ωa   ]
//  [ vb.x ]
//  [ vb.y ]
//  [ ωb   ]
///////////////////////////////////////////////////////////////////////////////
VecN Constraint::GetVelocities() const
{
    VecN V(6);

    V.Zero();

    V[0] = a->velocity.x;
    V[1] = a->velocity.y;
    V[2] = a->angularVelocity;
    V[3] = b->velocity.x;
    V[4] = b->velocity.y;
    V[5] = b->angularVelocity;

    return V;
}

///////////////Joint Constraint///////////////

JointConstraint::JointConstraint()
    : Constraint(), Jacobian(1, 6), cachedLambda(1), bias(0.0f)
{
    cachedLambda.Zero();
}

JointConstraint::JointConstraint(Body *a, Body *b, const Vec2 &anchorPoint)
    : Constraint(), Jacobian(1, 6), cachedLambda(1), bias(0.0f)
{
    this->a = a;
    this->b = b;
    this->aPoint = a->WorldSpaceToLocalSpace(anchorPoint);
    this->bPoint = b->WorldSpaceToLocalSpace(anchorPoint);

    cachedLambda.Zero();
}

void JointConstraint::PreSolve(const float dt)
{
    // Get the anchor point position in world space
    const Vec2 pa = a->LocalSpaceToWorldSpace(aPoint);
    const Vec2 pb = b->LocalSpaceToWorldSpace(bPoint);

    const Vec2 ra = pa - a->position;
    const Vec2 rb = pb - b->position;

    Jacobian.Zero();

    Vec2 J1 = (pa - pb) * 2.0;
    Jacobian.rows[0][0] = J1.x; // A linear velocity.x
    Jacobian.rows[0][1] = J1.y; // A linear velocity.y

    float J2 = ra.Cross(pa - pb) * 2.0;
    Jacobian.rows[0][2] = J2; // A angular velocity

    Vec2 J3 = (pb - pa) * 2.0;
    Jacobian.rows[0][3] = J3.x; // B linear velocity.x
    Jacobian.rows[0][4] = J3.y; // B linear velocity.y

    float J4 = rb.Cross(pb - pa) * 2.0;
    Jacobian.rows[0][5] = J4; // B angular velocity

    // Warm starting (apply cached lambda)
    const MatMN Jt = Jacobian.Transpose();
    VecN impulses = Jt * cachedLambda;

    // Apply the impulses to both bodies
    a->ApplyImpulseLinear(Vec2(impulses[0], impulses[1])); // A linear impulse
    a->ApplyImpulseAngular(impulses[2]);                   // A angular impulse
    b->ApplyImpulseLinear(Vec2(impulses[3], impulses[4])); // B linear impulse
    b->ApplyImpulseAngular(impulses[5]);                   // B angular impulse

    // Compute the bias term (baumgarte stabilization)
    const float beta = 0.1f;
    float C = (pb - pa).Dot(pb - pa); // Compute the positional error
    C = std::max(0.0f, C - 0.01f);
    bias = (beta / dt) * C;
}

void JointConstraint::Solve()
{
    const VecN V = GetVelocities();
    const MatMN invM = GetInvM();

    const MatMN Jt = Jacobian.Transpose();

    VecN rhs = Jacobian * V * -1.0f;
    MatMN lhs = Jacobian * invM * Jt;
    rhs[0] -= bias;

    VecN lambda = MatMN::SolveGaussSeidel(lhs, rhs);
    cachedLambda += lambda;

    // Compute the final impulses with directionn and magnitude
    VecN impulses = Jt * lambda;

    // Apply the impulses to both bodies
    a->ApplyImpulseLinear(Vec2(impulses[0], impulses[1])); // A linear impulse
    a->ApplyImpulseAngular(impulses[2]);                   // A angular impulse
    b->ApplyImpulseLinear(Vec2(impulses[3], impulses[4])); // B linear impulse
    b->ApplyImpulseAngular(impulses[5]);                   // B angular impulse
}

void JointConstraint::PostSolve()
{
    // TODO: Maybe we should clamp the values of cached lambda to reasonable limits
}

///////////////Penetration Constraint///////////////

PenetrationConstraint::PenetrationConstraint()
    : Constraint(), Jacobian(2, 6), cachedLambda(2), bias(0.0f)
{
    cachedLambda.Zero();
    friction = 0.0f;
}

PenetrationConstraint::PenetrationConstraint(Body *a, Body *b, const Vec2 &aCollisionPoint, const Vec2 &bCollisionPoint, const Vec2 &normal)
    : Constraint(), Jacobian(2, 6), cachedLambda(2), bias(0.0f)
{
    this->a = a;
    this->b = b;
    this->aPoint = a->WorldSpaceToLocalSpace(aCollisionPoint);
    this->bPoint = b->WorldSpaceToLocalSpace(bCollisionPoint);
    this->normal = a->WorldSpaceToLocalSpace(normal);

    cachedLambda.Zero();
    friction = 0.0f;
}

void PenetrationConstraint::PreSolve(const float dt)
{
    // Get the collision points position in world space
    const Vec2 pa = a->LocalSpaceToWorldSpace(aPoint);
    const Vec2 pb = b->LocalSpaceToWorldSpace(bPoint);
    Vec2 n = a->LocalSpaceToWorldSpace(normal);

    const Vec2 ra = pa - a->position;
    const Vec2 rb = pb - b->position;

    Jacobian.Zero();

    Vec2 J1 = -n;
    Jacobian.rows[0][0] = J1.x; // A linear velocity.x
    Jacobian.rows[0][1] = J1.y; // A linear velocity.y

    float J2 = -ra.Cross(n);
    Jacobian.rows[0][2] = J2; // A angular velocity

    Vec2 J3 = n;
    Jacobian.rows[0][3] = J3.x; // B linear velocity.x
    Jacobian.rows[0][4] = J3.y; // B linear velocity.y

    float J4 = rb.Cross(n);
    Jacobian.rows[0][5] = J4; // B angular velocity

    // Populate the second row of the Jacobian (tangent vector - friction)
    friction = std::max(a->friction, b->friction);
    if (friction > 0)
    {
        Vec2 t = n.Normal(); // The tangent is the vector perpendicular to the normal
        Jacobian.rows[1][0] = -t.x;
        Jacobian.rows[1][1] = -t.y;
        Jacobian.rows[1][2] = -ra.Cross(t);
        Jacobian.rows[1][3] = t.x;
        Jacobian.rows[1][4] = t.y;
        Jacobian.rows[1][5] = rb.Cross(t);
    }

    // Warm starting (apply cached lambda)
    const MatMN Jt = Jacobian.Transpose();
    VecN impulses = Jt * cachedLambda;

    // Apply the impulses to both bodies
    a->ApplyImpulseLinear(Vec2(impulses[0], impulses[1])); // A linear impulse
    a->ApplyImpulseAngular(impulses[2]);                   // A angular impulse
    b->ApplyImpulseLinear(Vec2(impulses[3], impulses[4])); // B linear impulse
    b->ApplyImpulseAngular(impulses[5]);                   // B angular impulse

    // Compute the bias term (baumgarte stabilization)
    const float beta = 0.2f;
    float C = (pb - pa).Dot(-n); // Compute the positional error
    C = std::min(0.0f, C + 0.01f);

    // Calculate the relative velocity pre-impulse normal to compute elasticity
    Vec2 va = a->velocity + Vec2(-a->angularVelocity * ra.y, a->angularVelocity * ra.x);
    Vec2 vb = b->velocity + Vec2(-b->angularVelocity * rb.y, b->angularVelocity * rb.x);
    float vrelDotNormal = (va - vb).Dot(n); // How fast both objects are moving toward/away from each other along the normal

    // Get the restitiution between two bodies (less bouncy)
    float e = std::min(a->restitution, b->restitution);

    // Calculate bias term considering elasticity (restitution)
    bias = (beta / dt) * C + (e * vrelDotNormal); // adds bounce, if the relative velocity is negative (approaching), this makes the objects separate faster, simulating a bounce.
}

void PenetrationConstraint::Solve()
{
    const VecN V = GetVelocities();
    const MatMN invM = GetInvM();

    const MatMN Jt = Jacobian.Transpose();

    VecN rhs = Jacobian * V * -1.0f;
    MatMN lhs;
    lhs.Zero();
    lhs = Jacobian * invM * Jt;
    rhs[0] -= bias;

    VecN lambda = MatMN::SolveGaussSeidel(lhs, rhs);

    // Accumulate impulses and clamp it within contraint limits
    VecN oldLambda = cachedLambda;
    cachedLambda += lambda;
    cachedLambda[0] = (cachedLambda[0] < 0.0f) ? 0.0f : cachedLambda[0];

    if (friction > 0.0)
    {
        const float maxFriction = cachedLambda[0] * friction;
        cachedLambda[1] = std::clamp(cachedLambda[1], -maxFriction, maxFriction);
    }
    lambda = cachedLambda - oldLambda;

    // Compute the final impulses with directionn and magnitude
    VecN impulses = Jt * lambda;

    // Apply the impulses to both bodies
    a->ApplyImpulseLinear(Vec2(impulses[0], impulses[1])); // A linear impulse
    a->ApplyImpulseAngular(impulses[2]);                   // A angular impulse
    b->ApplyImpulseLinear(Vec2(impulses[3], impulses[4])); // B linear impulse
    b->ApplyImpulseAngular(impulses[5]);                   // B angular impulse
}

void PenetrationConstraint::PostSolve()
{
    // TODO: Maybe we should clamp the values of cached lambda to reasonable limits
}