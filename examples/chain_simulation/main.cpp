#include "../../include/Application.h"
#include "../../include/Physics/World.h"
#include "../../include/Physics/Body.h"
#include "../../include/Physics/Shape.h"
#include "../../include/Physics/Constraint.h"
#include "../../include/Graphics.h"
#include <iostream>
#include <memory>
#include <vector>

/**
 * Chain Simulation Demo
 *
 * This example demonstrates:
 * - Joint constraints between bodies (proper constraint-based physics)
 * - Chain physics simulation using JointConstraint
 * - Interactive pendulum systems
 * - Rigid body constraint solving
 */

class ChainDemo
{
private:
    bool running = false;
    std::unique_ptr<World> world;
    std::vector<Body *> chainBodies;
    float chainLength = 40.0f;
    int chainLinks = 8;
    bool debugMode = false;

public:
    bool IsRunning() { return running; }

    void Setup()
    {
        running = Graphics::OpenWindow();

        // Create physics world with gravity
        world = std::make_unique<World>(-9.8f);

        // Create ground
        auto ground = std::make_unique<Body>(
            std::make_unique<BoxShape>(Graphics::Width() - 100, 50),
            Graphics::Width() / 2.0f, Graphics::Height() - 50,
            0.0f);
        ground->restitution = 0.6f;
        world->AddBody(std::move(ground));

        // Create initial chain
        Vec2 initialPos(Graphics::Width() / 2.0f, 100);
        CreateChain(initialPos, chainLinks, chainLength);

        std::cout << "Chain Simulation Demo Started!" << std::endl;
        std::cout << "This demo uses proper JointConstraint for realistic chain physics." << std::endl;
        std::cout << "Controls:" << std::endl;
        std::cout << "  Left Click  - Create new chain at cursor" << std::endl;
        std::cout << "  Right Click - Create pendulum at cursor" << std::endl;
        std::cout << "  R           - Reset simulation" << std::endl;
        std::cout << "  D           - Toggle debug mode (show constraints)" << std::endl;
        std::cout << "  ESC         - Exit" << std::endl;
    }

    void CreateChain(Vec2 startPos, int linkCount, float linkDistance)
    {
        Body *previousBody = nullptr;

        // Create static anchor body at the start position
        auto anchorBody = std::make_unique<Body>(
            std::make_unique<CircleShape>(6),
            startPos.x, startPos.y,
            0.0f); // Static body (mass = 0)
        anchorBody->restitution = 0.0f;
        Body *anchorPtr = anchorBody.get();
        world->AddBody(std::move(anchorBody));

        previousBody = anchorPtr;

        for (int i = 0; i < linkCount; ++i)
        {
            // Create chain link
            auto link = std::make_unique<Body>(
                std::make_unique<CircleShape>(12),
                startPos.x, startPos.y + (i + 1) * linkDistance,
                0.5f);
            link->restitution = 0.4f;
            link->friction = 0.6f;

            Body *currentBody = link.get();
            chainBodies.push_back(currentBody);
            world->AddBody(std::move(link));

            // Create joint constraint between previous body and current body
            if (previousBody)
            {
                Vec2 connectionPoint = Vec2(
                    (previousBody->position.x + currentBody->position.x) * 0.5f,
                    (previousBody->position.y + currentBody->position.y) * 0.5f);

                auto joint = std::make_unique<JointConstraint>(
                    previousBody, currentBody, connectionPoint);
                world->AddConstraint(std::move(joint));
            }

            previousBody = currentBody;
        }

        // Add a weight at the end for more interesting physics
        auto weight = std::make_unique<Body>(
            std::make_unique<CircleShape>(20),
            startPos.x, startPos.y + (linkCount + 1) * linkDistance,
            2.0f);
        weight->restitution = 0.7f;
        Body *weightPtr = weight.get();
        chainBodies.push_back(weightPtr);
        world->AddBody(std::move(weight));

        // Connect weight to last chain link
        if (previousBody)
        {
            Vec2 connectionPoint = Vec2(
                (previousBody->position.x + weightPtr->position.x) * 0.5f,
                (previousBody->position.y + weightPtr->position.y) * 0.5f);

            auto joint = std::make_unique<JointConstraint>(
                previousBody, weightPtr, connectionPoint);
            world->AddConstraint(std::move(joint));
        }
    }

    void CreateSimplePendulum(float x, float y)
    {
        // Create static anchor
        auto anchor = std::make_unique<Body>(
            std::make_unique<CircleShape>(8),
            x, y, 0.0f); // Static
        Body *anchorPtr = anchor.get();
        world->AddBody(std::move(anchor));

        // Create pendulum bob
        auto bob = std::make_unique<Body>(
            std::make_unique<CircleShape>(25),
            x + 50, y + 100, 2.0f);
        bob->restitution = 0.8f;
        bob->friction = 0.3f;
        Body *bobPtr = bob.get();
        world->AddBody(std::move(bob));

        // Connect with joint constraint
        Vec2 connectionPoint = Vec2(
            (anchorPtr->position.x + bobPtr->position.x) * 0.5f,
            (anchorPtr->position.y + bobPtr->position.y) * 0.5f);
        auto joint = std::make_unique<JointConstraint>(anchorPtr, bobPtr, connectionPoint);
        world->AddConstraint(std::move(joint));
    }

    void ResetSimulation()
    {
        world = std::make_unique<World>(-9.8f);
        chainBodies.clear();

        // Recreate ground
        auto ground = std::make_unique<Body>(
            std::make_unique<BoxShape>(Graphics::Width() - 100, 50),
            Graphics::Width() / 2.0f, Graphics::Height() - 50,
            0.0f);
        ground->restitution = 0.6f;
        world->AddBody(std::move(ground));

        // Recreate initial chain
        Vec2 initialPos(Graphics::Width() / 2.0f, 100);
        CreateChain(initialPos, chainLinks, chainLength);
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
                else if (event.key.keysym.sym == SDLK_r)
                {
                    ResetSimulation();
                }
                else if (event.key.keysym.sym == SDLK_d)
                {
                    debugMode = !debugMode;
                }
                break;

            case SDL_MOUSEBUTTONDOWN:
            {
                int x, y;
                SDL_GetMouseState(&x, &y);

                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    // Create new chain at cursor
                    Vec2 newAnchor(x, y);
                    CreateChain(newAnchor, 5, chainLength);
                }
                else if (event.button.button == SDL_BUTTON_RIGHT)
                {
                    // Create a simple pendulum
                    CreateSimplePendulum(x, y);
                }
                break;
            }
            }
        }
    }

    void Update()
    {
        const float dt = 1.0f / 60.0f;

        // Physics world handles constraint solving automatically
        world->Update(dt);
    }

    void Render()
    {
        Graphics::ClearScreen(0xFF0d1b2a); // Dark navy background

        // Draw chain connections in debug mode
        if (debugMode)
        {
            DrawChainConnections();
        }

        // Draw all bodies
        for (const auto &body : world->GetBodies())
        {
            if (body->shape->GetType() == CIRCLE)
            {
                CircleShape *circle = dynamic_cast<CircleShape *>(body->shape.get());

                Uint32 color;
                if (body->IsStatic())
                {
                    color = 0xFF415a77; // Steel blue for static
                }
                else if (body->mass > 2.0f)
                {
                    color = 0xFFf77f00; // Orange for heavy objects
                }
                else
                {
                    color = 0xFF277da1; // Blue for chain links
                }

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

                Graphics::DrawFillRect(
                    body->position.x, body->position.y,
                    box->width, box->height, 0xFF778da9);
                Graphics::DrawRect(
                    body->position.x, body->position.y,
                    box->width, box->height, 0xFFFFFFFF);
            }
        }

        // Draw UI instructions
        // Note: You would need to implement text rendering for this

        Graphics::RenderFrame();
    }

    void DrawChainConnections()
    {
        // Draw constraint connections by iterating through world constraints
        for (const auto &constraint : world->GetConstraints())
        {
            JointConstraint *joint = dynamic_cast<JointConstraint *>(constraint.get());
            if (joint)
            {
                Graphics::DrawLine(
                    joint->a->position.x, joint->a->position.y,
                    joint->b->position.x, joint->b->position.y,
                    0xFF90e0ef); // Light blue constraint lines
            }
        }
    }

    void Destroy()
    {
        Graphics::CloseWindow();
    }
};

int main()
{
    ChainDemo demo;

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
