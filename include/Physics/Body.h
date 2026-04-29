#ifndef BODY_H
#define BODY_H

#include <SDL.h>
#include <SDL_image.h>
#include "Math/Vec2.h"
#include "Shape.h"
#include <memory>

struct Body
{
    bool firstUpdate = true;
    bool justSpawned = true;

    // Linear motion
    Vec2 position;
    Vec2 velocity;
    Vec2 acceleration;

    // Angular motion
    float rotation;
    float angularVelocity;
    float angularAcceleration;

    // Forces and torque
    Vec2 sumForces;
    float sumTorque;

    // Mass and Moment of Inertia
    float mass;
    float invMass;
    float I;
    float invI;

    // Coefficient of restitution (elasticity)
    float restitution;

    // Coefficient of fricition
    float friction;

    // Pointer to the shape of the rigid body
    std::unique_ptr<Shape> shape;

    // Pointer to an SDL texture
    SDL_Texture *texture = nullptr;

    Body(std::unique_ptr<Shape> shape, float x, float y, float mass);
    ~Body();

    void SetTexture(const char *textureFileName);

    bool IsStatic() const;

    void ApplyImpulseLinear(const Vec2 &j);
    void ApplyImpulseAngular(const float j);
    void ApplyImpulseAtPoint(const Vec2 &j, const Vec2 &r);
    void AddForce(const Vec2 &force);
    void AddTorque(float torque);
    void clearForces();
    void clearTorque();

    Vec2 LocalSpaceToWorldSpace(const Vec2 &point) const;
    Vec2 WorldSpaceToLocalSpace(const Vec2 &point) const;

    void IntegrateForces(const float dt);
    void IntegrateVelocities(const float dt);
};

#endif