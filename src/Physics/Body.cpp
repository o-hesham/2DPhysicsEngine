#include "Physics/Body.h"
#include "Physics/Constants.h"
#include <iostream>
#include <cmath>
#include "Graphics.h"

Body::Body(std::unique_ptr<Shape> shape, float x, float y, float mass)
    : shape(std::move(shape)), velocity(0, 0), acceleration(0, 0), rotation(0.0), angularVelocity(0.0), angularAcceleration(0.0), sumForces(0, 0), sumTorque(0.0), position(x, y), restitution(1.0), friction(0.7), mass(mass)
{
    // Inverse mass
    if (mass != 0.0)
    {
        invMass = 1.0 / mass;
    }
    else
    {
        invMass = 0.0;
    }

    // Calculate inertia
    I = this->shape->GetMomentOfInertia() * mass;

    // Inverse inertia
    if (I != 0.0)
    {
        invI = 1.0 / I;
    }
    else
    {
        invI = 0.0;
    }

    std::cout << "Body constructor called!" << std::endl;
}

Body::~Body()
{
    SDL_DestroyTexture(texture);
    std::cout << "Body destructor called!" << std::endl;
}

void Body::SetTexture(const char *textureFileName)
{
    SDL_Surface *surface = IMG_Load(textureFileName);
    if (surface)
    {
        texture = SDL_CreateTextureFromSurface(Graphics::renderer, surface);
        SDL_FreeSurface(surface);
    }
}

bool Body::IsStatic() const
{
    return fabs(invMass - 0.0) < EPSILON;
}

void Body::AddForce(const Vec2 &force)
{
    sumForces += force;
}

void Body::AddTorque(float torque)
{
    sumTorque += torque;
}

void Body::clearForces()
{
    sumForces = Vec2(0.0, 0.0);
}

void Body::clearTorque()
{
    sumTorque = 0.0;
}

void Body::ApplyImpulseLinear(const Vec2 &j)
{
    if (IsStatic())
    {
        return;
    }
    velocity += j * invMass;
}

void Body::ApplyImpulseAngular(const float j)
{
    if (IsStatic())
    {
        return;
    }
    angularVelocity += j * invI;
}

void Body::ApplyImpulseAtPoint(const Vec2 &j, const Vec2 &r)
{
    if (IsStatic())
    {
        return;
    }
    velocity += j * invMass;
    angularVelocity += r.Cross(j) * invI;
}

Vec2 Body::LocalSpaceToWorldSpace(const Vec2 &point) const
{
    Vec2 rotated = point.Rotate(rotation);
    return rotated + position;
}

Vec2 Body::WorldSpaceToLocalSpace(const Vec2 &point) const
{
    float translatedX = point.x - position.x;
    float translatedY = point.y - position.y;
    float rotatedX = cos(-rotation) * translatedX - sin(-rotation) * translatedY;
    float rotatedY = cos(-rotation) * translatedY + sin(-rotation) * translatedX;

    return Vec2(rotatedX, rotatedY);
}

/////////////// Euler Integration /////////////////
void Body::IntegrateForces(const float dt)
{
    if (IsStatic())
        return;

    // Find the acceleration based on the forces that are being applied and the mass
    acceleration = sumForces * invMass;
    // Integrate the acceleration to find the new velocity
    velocity += acceleration * dt;

    // Find the angular acceleration based on the torque that is being applied and the moment of inertia
    angularAcceleration = sumTorque * invI;
    // Integrate the angular acceleration to find the new angular velocity
    angularVelocity += angularAcceleration * dt;

    // Clear all the forces and torque acting on the object before the next physics step
    clearForces();
    clearTorque();
}

void Body::IntegrateVelocities(const float dt)
{
    if (firstUpdate)
    {
        shape->UpdateVertices(rotation, position);
        firstUpdate = false;
    }

    if (IsStatic())
        return;

    // Integrate the velocity to find the new position
    position += velocity * dt;

    // Integrate the angular velocity to find the new rotation angle
    rotation += angularVelocity * dt;

    // Update the vertices to adjust to the new position/rotation
    shape->UpdateVertices(rotation, position);
}