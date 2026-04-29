#include "../../include/Application.h"
#include "../../include/Physics/World.h"
#include "../../include/Physics/Body.h"
#include "../../include/Physics/Shape.h"
#include "../../include/Physics/Force.h"
#include "../../include/Graphics.h"
#include <iostream>
#include <memory>
#include <vector>

/**
 * Forces Demo
 *
 * This example demonstrates:
 * - Gravitational attraction between bodies
 * - Drag forces affecting motion
 * - Spring forces and oscillation
 * - Friction effects
 * - Custom force fields
 */

class ForcesDemo
{
private:
    bool running = false;
    std::unique_ptr<World> world;
    std::vector<Body *> planets;
    std::vector<Body *> particles;
    Vec2 springAnchor;
    Body *springBody = nullptr;

    // Force settings
    bool gravityEnabled = true;
    bool dragEnabled = false;
    bool springEnabled = false;
    bool frictionEnabled = false;
    float gravitationalConstant = 100.0f;
    float dragCoefficient = 0.1f;
    float springConstant = 200.0f;

public:
    bool IsRunning() { return running; }

    void Setup()
    {
        running = Graphics::OpenWindow();

        // Create physics world with reduced gravity for better force demonstration
        world = std::make_unique<World>(-2.0f);

        springAnchor = Vec2(Graphics::Width() / 2.0f, 100);

        // Create boundaries
        CreateBoundaries();

        // Create some massive bodies for gravitational attraction
        CreatePlanet(200, 200, 50, 10.0f);
        CreatePlanet(Graphics::Width() - 200, 300, 40, 8.0f);
        CreatePlanet(Graphics::Width() / 2.0f, Graphics::Height() - 200, 60, 12.0f);

        // Create spring-connected body
        CreateSpringBody();

        // Add some test particles
        for (int i = 0; i < 5; ++i)
        {
            CreateParticle(300 + i * 50, 100, 1.0f);
        }

        std::cout << "Forces Demo Started!" << std::endl;
        std::cout << "Controls:" << std::endl;
        std::cout << "  Left Click  - Add particle at cursor" << std::endl;
        std::cout << "  Right Click - Add heavy planet at cursor" << std::endl;
        std::cout << "  G           - Toggle gravitational forces" << std::endl;
        std::cout << "  D           - Toggle drag forces" << std::endl;
        std::cout << "  S           - Toggle spring forces" << std::endl;
        std::cout << "  F           - Toggle friction" << std::endl;
        std::cout << "  R           - Reset simulation" << std::endl;
        std::cout << "  ESC         - Exit" << std::endl;
    }

    void CreateBoundaries()
    {
        // Create invisible walls for containment
        auto leftWall = std::make_unique<Body>(
            std::make_unique<BoxShape>(10, Graphics::Height()),
            5, Graphics::Height() / 2.0f, 0.0f);
        world->AddBody(std::move(leftWall));

        auto rightWall = std::make_unique<Body>(
            std::make_unique<BoxShape>(10, Graphics::Height()),
            Graphics::Width() - 5, Graphics::Height() / 2.0f, 0.0f);
        world->AddBody(std::move(rightWall));

        auto topWall = std::make_unique<Body>(
            std::make_unique<BoxShape>(Graphics::Width(), 10),
            Graphics::Width() / 2.0f, 5, 0.0f);
        world->AddBody(std::move(topWall));

        auto bottomWall = std::make_unique<Body>(
            std::make_unique<BoxShape>(Graphics::Width(), 10),
            Graphics::Width() / 2.0f, Graphics::Height() - 5, 0.0f);
        world->AddBody(std::move(bottomWall));
    }

    void CreatePlanet(float x, float y, float radius, float mass)
    {
        auto planet = std::make_unique<Body>(
            std::make_unique<CircleShape>(radius),
            x, y, mass);
        planet->restitution = 0.3f;
        planet->friction = 0.8f;

        Body *planetPtr = planet.get();
        planets.push_back(planetPtr);
        world->AddBody(std::move(planet));
    }

    void CreateParticle(float x, float y, float mass)
    {
        auto particle = std::make_unique<Body>(
            std::make_unique<CircleShape>(8),
            x, y, mass);
        particle->restitution = 0.7f;
        particle->friction = 0.2f;

        // Give particles some initial velocity
        particle->velocity = Vec2((rand() % 200 - 100) / 10.0f, (rand() % 200 - 100) / 10.0f);

        Body *particlePtr = particle.get();
        particles.push_back(particlePtr);
        world->AddBody(std::move(particle));
    }

    void CreateSpringBody()
    {
        auto body = std::make_unique<Body>(
            std::make_unique<CircleShape>(15),
            springAnchor.x, springAnchor.y + 100,
            1.5f);
        body->restitution = 0.5f;

        springBody = body.get();
        world->AddBody(std::move(body));
    }

    void ResetSimulation()
    {
        world = std::make_unique<World>(-2.0f);
        planets.clear();
        particles.clear();
        springBody = nullptr;

        CreateBoundaries();
        CreatePlanet(200, 200, 50, 10.0f);
        CreatePlanet(Graphics::Width() - 200, 300, 40, 8.0f);
        CreatePlanet(Graphics::Width() / 2.0f, Graphics::Height() - 200, 60, 12.0f);
        CreateSpringBody();

        for (int i = 0; i < 5; ++i)
        {
            CreateParticle(300 + i * 50, 100, 1.0f);
        }
    }

    void Input()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                running = false;
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    running = false;
                    break;
                case SDLK_g:
                    gravityEnabled = !gravityEnabled;
                    std::cout << "Gravitational forces: " << (gravityEnabled ? "ON" : "OFF") << std::endl;
                    break;
                case SDLK_d:
                    dragEnabled = !dragEnabled;
                    std::cout << "Drag forces: " << (dragEnabled ? "ON" : "OFF") << std::endl;
                    break;
                case SDLK_s:
                    springEnabled = !springEnabled;
                    std::cout << "Spring forces: " << (springEnabled ? "ON" : "OFF") << std::endl;
                    break;
                case SDLK_f:
                    frictionEnabled = !frictionEnabled;
                    std::cout << "Friction: " << (frictionEnabled ? "ON" : "OFF") << std::endl;
                    break;
                case SDLK_r:
                    ResetSimulation();
                    break;
                }
                break;

            case SDL_MOUSEBUTTONDOWN:
            {
                int x, y;
                SDL_GetMouseState(&x, &y);

                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    CreateParticle(x, y, 1.0f);
                }
                else if (event.button.button == SDL_BUTTON_RIGHT)
                {
                    CreatePlanet(x, y, 35, 8.0f);
                }
                break;
            }
            }
        }
    }

    void Update()
    {
        const float dt = 1.0f / 60.0f;

        // Apply custom forces before physics update
        ApplyCustomForces();

        world->Update(dt);
    }

    void ApplyCustomForces()
    {
        // Apply gravitational forces between all massive bodies
        if (gravityEnabled)
        {
            ApplyGravitationalForces();
        }

        // Apply drag forces to all moving bodies
        if (dragEnabled)
        {
            ApplyDragForces();
        }

        // Apply spring force to spring body
        if (springEnabled && springBody)
        {
            Vec2 springForce = Force::GenerateSpringForce(*springBody, springAnchor, 100.0f, springConstant);
            springBody->AddForce(springForce);
        }

        // Apply enhanced friction
        if (frictionEnabled)
        {
            ApplyFrictionForces();
        }
    }

    void ApplyGravitationalForces()
    {
        // Apply gravitational attraction between all dynamic bodies
        auto &bodies = world->GetBodies();

        for (size_t i = 0; i < bodies.size(); ++i)
        {
            for (size_t j = i + 1; j < bodies.size(); ++j)
            {
                Body *bodyA = bodies[i].get();
                Body *bodyB = bodies[j].get();

                // Skip static bodies
                if (bodyA->IsStatic() || bodyB->IsStatic())
                    continue;

                // Calculate gravitational force
                Vec2 gravForce = Force::GenerateGavitationalForce(
                    *bodyA, *bodyB, gravitationalConstant, 100.0f, 10000.0f);

                bodyA->AddForce(gravForce);
                bodyB->AddForce(gravForce * -1.0f);
            }
        }
    }

    void ApplyDragForces()
    {
        auto &bodies = world->GetBodies();

        for (auto &body : bodies)
        {
            if (body->IsStatic())
                continue;

            Vec2 dragForce = Force::GenerateDragForce(*body, dragCoefficient);
            body->AddForce(dragForce);
        }
    }

    void ApplyFrictionForces()
    {
        auto &bodies = world->GetBodies();

        for (auto &body : bodies)
        {
            if (body->IsStatic())
                continue;

            Vec2 frictionForce = Force::GenerateFrictionForce(*body, 50.0f);
            body->AddForce(frictionForce);
        }
    }

    void Render()
    {
        Graphics::ClearScreen(0xFF0f0f23); // Dark purple background

        // Draw spring anchor and connection
        if (springEnabled && springBody)
        {
            Graphics::DrawFillCircle(springAnchor.x, springAnchor.y, 6, 0xFFff006e);
            Graphics::DrawLine(
                springAnchor.x, springAnchor.y,
                springBody->position.x, springBody->position.y,
                0xFFfb8500);
        }

        // Draw all bodies with different colors based on their properties
        auto &bodies = world->GetBodies();
        for (auto &body : bodies)
        {
            if (body->shape->GetType() == CIRCLE)
            {
                CircleShape *circle = dynamic_cast<CircleShape *>(body->shape.get());

                Uint32 color;
                if (body->IsStatic())
                {
                    continue; // Don't draw invisible walls
                }
                else if (body->mass > 5.0f)
                {
                    // Massive bodies (planets) - bright colors
                    color = 0xFFffbe0b;
                }
                else if (body.get() == springBody)
                {
                    // Spring body - special color
                    color = 0xFF8ecae6;
                }
                else
                {
                    // Regular particles - white/light blue
                    color = 0xFFf1faee;
                }

                Graphics::DrawFillCircle(
                    body->position.x, body->position.y,
                    circle->radius, color);

                // Draw velocity vectors for particles (visual feedback)
                if (!body->IsStatic() && body->velocity.MagnitudeSquared() > 1.0f)
                {
                    Vec2 velEnd = body->position + body->velocity * 5.0f;
                    Graphics::DrawLine(
                        body->position.x, body->position.y,
                        velEnd.x, velEnd.y,
                        0xFF219ebc);
                }

                // Draw outline
                Graphics::DrawCircle(
                    body->position.x, body->position.y,
                    circle->radius, body->rotation, 0xFFFFFFFF);
            }
        }

        // Draw force status indicators (simple colored rectangles)
        int yOffset = 20;
        if (gravityEnabled)
        {
            Graphics::DrawFillRect(20, yOffset, 100, 20, 0xFF219ebc);
            yOffset += 30;
        }
        if (dragEnabled)
        {
            Graphics::DrawFillRect(20, yOffset, 100, 20, 0xFFf77f00);
            yOffset += 30;
        }
        if (springEnabled)
        {
            Graphics::DrawFillRect(20, yOffset, 100, 20, 0xFFfb8500);
            yOffset += 30;
        }
        if (frictionEnabled)
        {
            Graphics::DrawFillRect(20, yOffset, 100, 20, 0xFFe63946);
            yOffset += 30;
        }

        Graphics::RenderFrame();
    }

    void Destroy()
    {
        Graphics::CloseWindow();
    }
};

int main()
{
    ForcesDemo demo;

    demo.Setup();

    while (demo.IsRunning())
    {
        demo.Input();
        demo.Update();
        demo.Render();
    }

    demo.Destroy();

    return 0;
}
