#include "../../include/Application.h"
#include "../../include/Physics/World.h"
#include "../../include/Physics/Body.h"
#include "../../include/Physics/Shape.h"
#include "../../include/Graphics.h"
#include <iostream>
#include <memory>

/**
 * Basic Physics Demo
 *
 * This example demonstrates:
 * - Creating a physics world
 * - Adding static and dynamic bodies
 * - Basic collision and gravity
 * - Simple interactive controls
 */

class BasicDemo
{
private:
    bool running = false;
    std::unique_ptr<World> world;

public:
    bool IsRunning() { return running; }

    void Setup()
    {
        running = Graphics::OpenWindow();

        // Create physics world with Earth-like gravity
        world = std::make_unique<World>(-9.8f);

        // Create static ground
        auto ground = std::make_unique<Body>(
            std::make_unique<BoxShape>(Graphics::Width() - 100, 50),
            Graphics::Width() / 2.0f, Graphics::Height() - 50,
            0.0f // mass = 0 makes it static
        );
        ground->restitution = 0.8f; // Bouncy ground
        world->AddBody(std::move(ground));

        // Create side walls
        auto leftWall = std::make_unique<Body>(
            std::make_unique<BoxShape>(50, Graphics::Height() - 100),
            25, Graphics::Height() / 2.0f,
            0.0f);
        world->AddBody(std::move(leftWall));

        auto rightWall = std::make_unique<Body>(
            std::make_unique<BoxShape>(50, Graphics::Height() - 100),
            Graphics::Width() - 25, Graphics::Height() / 2.0f,
            0.0f);
        world->AddBody(std::move(rightWall));

        // Add some initial falling objects
        CreateFallingBall(Graphics::Width() / 4, 100);
        CreateFallingBox(3 * Graphics::Width() / 4, 150);

        std::cout << "Basic Physics Demo Started!" << std::endl;
        std::cout << "Controls:" << std::endl;
        std::cout << "  Left Click  - Create bouncy ball" << std::endl;
        std::cout << "  Right Click - Create heavy box" << std::endl;
        std::cout << "  ESC         - Exit" << std::endl;
    }

    void CreateFallingBall(float x, float y)
    {
        auto ball = std::make_unique<Body>(
            std::make_unique<CircleShape>(25),
            x, y, 1.0f);
        ball->restitution = 0.7f; // Bouncy
        ball->friction = 0.3f;    // Some friction
        world->AddBody(std::move(ball));
    }

    void CreateFallingBox(float x, float y)
    {
        auto box = std::make_unique<Body>(
            std::make_unique<BoxShape>(40, 40),
            x, y, 2.0f // Heavier than balls
        );
        box->restitution = 0.4f; // Less bouncy
        box->friction = 0.5f;    // More friction
        world->AddBody(std::move(box));
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
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    running = false;
                }
                break;

            case SDL_MOUSEBUTTONDOWN:
            {
                int x, y;
                SDL_GetMouseState(&x, &y);

                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    CreateFallingBall(x, y);
                }
                else if (event.button.button == SDL_BUTTON_RIGHT)
                {
                    CreateFallingBox(x, y);
                }
                break;
            }
            }
        }
    }

    void Update()
    {
        // Fixed timestep physics
        const float dt = 1.0f / 60.0f;
        world->Update(dt);
    }

    void Render()
    {
        Graphics::ClearScreen(0xFF1a1a2e); // Dark blue background

        // Draw all bodies
        for (const auto &body : world->GetBodies())
        {
            if (body->shape->GetType() == CIRCLE)
            {
                CircleShape *circle = dynamic_cast<CircleShape *>(body->shape.get());

                // Static bodies are white, dynamic bodies are colored by mass
                Uint32 color = body->IsStatic() ? 0xFFFFFFFF : (body->mass > 1.5f ? 0xFFff6b6b : 0xFF4ecdc4);

                Graphics::DrawFillCircle(
                    body->position.x, body->position.y,
                    circle->radius, color);
                Graphics::DrawCircle(
                    body->position.x, body->position.y,
                    circle->radius, body->rotation, 0xFFFFFFFF);
            }
            else if (body->shape->GetType() == BOX)
            {
                BoxShape *box = dynamic_cast<BoxShape *>(body->shape.get());

                Uint32 color = body->IsStatic() ? 0xFF555555 : (body->mass > 1.5f ? 0xFFff7675 : 0xFF74b9ff);

                Graphics::DrawFillRect(
                    body->position.x, body->position.y,
                    box->width, box->height, color);
                Graphics::DrawRect(
                    body->position.x, body->position.y,
                    box->width, box->height, 0xFFFFFFFF);
            }
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
    BasicDemo demo;

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
