#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <memory>
#include "body.h"
#include "Constraint.h"

class World
{
private:
    float G = 9.8;
    std::vector<std::unique_ptr<Body>> bodies;
    std::vector<std::unique_ptr<Constraint>> constraints;
    std::vector<Vec2> forces;
    std::vector<float> torques;

public:
    World(float gravity);
    ~World();

    void AddBody(std::unique_ptr<Body> body);
    std::vector<std::unique_ptr<Body>> &GetBodies();

    void AddConstraint(std::unique_ptr<Constraint> constraint);
    std::vector<std::unique_ptr<Constraint>> &GetConstraints();

    void AddForce(const Vec2 &force);
    void AddTorque(float torque);
    void clearForces();
    void clearTorque();

    void Update(float dt);

    void checkCollisions();
};

#endif