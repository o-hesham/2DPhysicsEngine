#include "physics/World.h"
#include "physics/Constants.h"
#include "physics/CollisionDetection.h"
#include "physics/Force.h"
#include "Graphics.h"
#include <iostream>

World::World(float gravity)
{
    G = -gravity;
    std::cout << "World constructor called!" << std::endl;
}

World::~World()
{
    std::cout << "World destructor called!" << std::endl;
}

void World::AddBody(std::unique_ptr<Body> body)
{
    bodies.push_back(std::move(body));
}

std::vector<std::unique_ptr<Body>> &World::GetBodies()
{
    return bodies;
}

void World::AddConstraint(std::unique_ptr<Constraint> constraint)
{
    constraints.push_back(std::move(constraint));
}

std::vector<std::unique_ptr<Constraint>> &World::GetConstraints()
{
    return constraints;
}

void World::AddForce(const Vec2 &force)
{
    forces.push_back(force);
}

void World::AddTorque(float torque)
{
    torques.push_back(torque);
}

void World::Update(float dt)
{
    // Create a vector of penetration constraints that will be solved frame per frame
    std::vector<PenetrationConstraint> penetrations;
    // Loop all bodies of the world applying forces
    for (auto &body : bodies)
    {
        Vec2 weight = Vec2(0.0, body->mass * G * PIXELS_PER_METER);
        body->AddForce(weight);

        // Apply forces to all bodies
        for (auto force : forces)
        {
            body->AddForce(force);
        }

        // Apply torques to all bodies
        for (auto torque : torques)
        {
            body->AddTorque(torque);
        }
    }
    // Integrate all the forces
    for (auto &body : bodies)
    {
        body->IntegrateForces(dt);
    }

    // TODO: Broad-Phase Collision Check!!!
    // Loop all the bodies, checking everything  with everything else
    // But we only use bounding-circle with bounding-circle
    // Generating an array with pair of colliding rigid bodies!!!

    // TODO: Narrow-Phase Collision Check!!!
    // Loop all the pair of bodies that came from the broad-phase
    // Performing the refined check

    // Check all the bodies  with all other bodies detecting collisions
    for (size_t i = 0; i <= bodies.size() - 1; i++)
    {
        for (size_t j = i + 1; j < bodies.size(); j++)
        {
            Body *a = bodies[i].get();
            Body *b = bodies[j].get();

            std::vector<Contact> contacts;
            if (CollisionDetection::IsColliding(a, b, contacts))
            {
                for (auto contact : contacts)
                {
                    // Draw collision points
                    // Graphics::DrawCircle(contact.start.x, contact.start.y, 5, 0.0, 0xFF00FFFF);
                    // Graphics::DrawCircle(contact.end.x, contact.end.y, 2, 0.0, 0xFF00FFFF);

                    // Create a new penetration constraint
                    PenetrationConstraint penetration(contact.a, contact.b, contact.start, contact.end, contact.normal);
                    penetrations.push_back(penetration);
                }
            }
        }
    }

    // Solve all constraints
    for (auto &constraint : constraints)
    {
        constraint->PreSolve(dt);
    }
    for (auto &constraint : penetrations)
    {
        constraint.PreSolve(dt);
    }

    for (int i = 0; i < 5; i++)
    {
        for (auto &constraint : constraints)
        {
            constraint->Solve();
        }
        for (auto &constraint : penetrations)
        {
            constraint.Solve();
        }
    }

    for (auto &constraint : constraints)
    {
        constraint->PostSolve();
    }
    for (auto &constraint : penetrations)
    {
        constraint.PostSolve();
    }

    // Integrate all the velocities
    for (auto &body : bodies)
    {
        body->IntegrateVelocities(dt);
        body->justSpawned = false;
    }
}