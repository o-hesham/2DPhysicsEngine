#include "../../include/Application.h"
#include "../../include/Physics/World.h"
#include "../../include/Physics/Body.h"
#include "../../include/Physics/Shape.h"
#include "../../include/Graphics.h"
#include <iostream>
#include <memory>
#include <vector>

/**
 * Collision Demo
 *
 * This example demonstrates:
 * - Different collision shapes (circles, boxes, polygons)
 * - Material properties (restitution, friction)
 * - Collision response and energy transfer
 * - Interactive collision testing
 * - Visual collision feedback
 */

class CollisionDemo
{
private:
    bool running = false;
    std::unique_ptr<World> world;
    bool debugMode = true;

    // Material presets
    enum MaterialType
    {
        BOUNCY, // High restitution, low friction
        RUBBER, // Medium restitution, high friction
        METAL,  // Low restitution, low friction
        WOOD,   // Medium restitution, medium friction
        ICE     // Medium restitution, very low friction
    };

    MaterialType currentMaterial = BOUNCY;

    struct MaterialProperties
    {
        float restitution;
        float friction;
        Uint32 color;
        const char *name;
    };

    MaterialProperties materials[5] = {
        {0.95f, 0.1f, 0xFF00ff88, "Bouncy"},
        {0.6f, 0.8f, 0xFFff6b6b, "Rubber"},
        {0.2f, 0.3f, 0xFF718096, "Metal"},
        {0.5f, 0.6f, 0xFFd4a574, "Wood"},
        {0.7f, 0.05f, 0xFF87ceeb, "Ice"}};

public:
    bool IsRunning() { return running; }

    void Setup()
    {
        running = Graphics::OpenWindow();

        // Create physics world with standard gravity
        world = std::make_unique<World>(-9.8f);

        // Create test environment
        CreateTestEnvironment();

        std::cout << "Collision Demo Started!" << std::endl;
        std::cout << "Controls:" << std::endl;
        std::cout << "  Left Click  - Create circle with current material" << std::endl;
        std::cout << "  Right Click - Create box with current material" << std::endl;
        std::cout << "  Middle Click- Create triangle (if supported)" << std::endl;
        std::cout << "  1-5         - Select material type" << std::endl;
        std::cout << "  D           - Toggle debug mode" << std::endl;
        std::cout << "  SPACE       - Drop test objects" << std::endl;
        std::cout << "  R           - Reset simulation" << std::endl;
        std::cout << "  ESC         - Exit" << std::endl;
        std::cout << "Current material: " << materials[currentMaterial].name << std::endl;
    }

    void CreateTestEnvironment()
    {
        // Create ground with different sections having different materials
        CreatePlatform(150, Graphics::Height() - 100, 250, 30, ICE);
        CreatePlatform(450, Graphics::Height() - 150, 250, 30, RUBBER);
        CreatePlatform(750, Graphics::Height() - 100, 250, 30, METAL);
        CreatePlatform(Graphics::Width() / 2.0f, Graphics::Height() - 50, Graphics::Width() - 100, 40, WOOD);

        // Create some angled platforms for interesting collisions
        CreateAngledPlatform(300, 400, 200, 20, 0.3f, BOUNCY);
        CreateAngledPlatform(700, 350, 200, 20, -0.3f, WOOD);

        // Create side walls
        CreatePlatform(25, Graphics::Height() / 2.0f, 50, Graphics::Height() - 100, METAL);
        CreatePlatform(Graphics::Width() - 25, Graphics::Height() / 2.0f, 50, Graphics::Height() - 100, METAL);

        // Add some initial test objects
        CreateTestObjects();
    }

    void CreatePlatform(float x, float y, float width, float height, MaterialType material)
    {
        auto platform = std::make_unique<Body>(
            std::make_unique<BoxShape>(width, height),
            x, y, 0.0f // Static body
        );

        auto &props = materials[material];
        platform->restitution = props.restitution;
        platform->friction = props.friction;

        world->AddBody(std::move(platform));
    }

    void CreateAngledPlatform(float x, float y, float width, float height, float angle, MaterialType material)
    {
        auto platform = std::make_unique<Body>(
            std::make_unique<BoxShape>(width, height),
            x, y, 0.0f);

        auto &props = materials[material];
        platform->restitution = props.restitution;
        platform->friction = props.friction;
        platform->rotation = angle;

        world->AddBody(std::move(platform));
    }

    void CreateTestObjects()
    {
        // Create a variety of test objects with different materials
        CreateCircle(200, 50, 20, BOUNCY);
        CreateBox(400, 80, 35, 35, RUBBER);
        CreateCircle(600, 60, 15, METAL);
        CreateBox(800, 70, 25, 40, WOOD);
        CreateCircle(1000, 55, 25, ICE);
    }

    void CreateCircle(float x, float y, float radius, MaterialType material)
    {
        auto circle = std::make_unique<Body>(
            std::make_unique<CircleShape>(radius),
            x, y, 1.0f);

        auto &props = materials[material];
        circle->restitution = props.restitution;
        circle->friction = props.friction;

        world->AddBody(std::move(circle));
    }

    void CreateBox(float x, float y, float width, float height, MaterialType material)
    {
        auto box = std::make_unique<Body>(
            std::make_unique<BoxShape>(width, height),
            x, y, 1.0f);

        auto &props = materials[material];
        box->restitution = props.restitution;
        box->friction = props.friction;

        world->AddBody(std::move(box));
    }

    void DropTestObjects()
    {
        // Drop a variety of objects for collision testing
        float centerX = Graphics::Width() / 2.0f;

        CreateCircle(centerX - 100, 50, 15, BOUNCY);
        CreateBox(centerX - 50, 60, 30, 30, RUBBER);
        CreateCircle(centerX, 70, 20, METAL);
        CreateBox(centerX + 50, 40, 25, 35, WOOD);
        CreateCircle(centerX + 100, 55, 18, ICE);
    }

    void ResetSimulation()
    {
        world = std::make_unique<World>(-9.8f);
        CreateTestEnvironment();
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
                case SDLK_d:
                    debugMode = !debugMode;
                    std::cout << "Debug mode: " << (debugMode ? "ON" : "OFF") << std::endl;
                    break;
                case SDLK_SPACE:
                    DropTestObjects();
                    break;
                case SDLK_r:
                    ResetSimulation();
                    break;
                case SDLK_1:
                    currentMaterial = BOUNCY;
                    std::cout << "Selected material: " << materials[currentMaterial].name << std::endl;
                    break;
                case SDLK_2:
                    currentMaterial = RUBBER;
                    std::cout << "Selected material: " << materials[currentMaterial].name << std::endl;
                    break;
                case SDLK_3:
                    currentMaterial = METAL;
                    std::cout << "Selected material: " << materials[currentMaterial].name << std::endl;
                    break;
                case SDLK_4:
                    currentMaterial = WOOD;
                    std::cout << "Selected material: " << materials[currentMaterial].name << std::endl;
                    break;
                case SDLK_5:
                    currentMaterial = ICE;
                    std::cout << "Selected material: " << materials[currentMaterial].name << std::endl;
                    break;
                }
                break;

            case SDL_MOUSEBUTTONDOWN:
            {
                int x, y;
                SDL_GetMouseState(&x, &y);

                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    CreateCircle(x, y, 15 + (rand() % 15), currentMaterial);
                }
                else if (event.button.button == SDL_BUTTON_RIGHT)
                {
                    float size = 20 + (rand() % 20);
                    CreateBox(x, y, size, size, currentMaterial);
                }
                else if (event.button.button == SDL_BUTTON_MIDDLE)
                {
                    // For future polygon support
                    CreateCircle(x, y, 12, currentMaterial);
                }
                break;
            }
            }
        }
    }

    void Update()
    {
        const float dt = 1.0f / 60.0f;
        world->Update(dt);
    }

    void Render()
    {
        Graphics::ClearScreen(0xFF1a1a2e); // Dark background

        // Draw all bodies with material-based colors
        auto &bodies = world->GetBodies();
        for (auto &body : bodies)
        {
            Uint32 bodyColor = GetBodyColor(body.get());

            if (body->shape->GetType() == CIRCLE)
            {
                CircleShape *circle = dynamic_cast<CircleShape *>(body->shape.get());

                Graphics::DrawFillCircle(
                    body->position.x, body->position.y,
                    circle->radius, bodyColor);

                if (debugMode)
                {
                    Graphics::DrawCircle(
                        body->position.x, body->position.y,
                        circle->radius, body->rotation, 0xFFFFFFFF);

                    // Draw rotation indicator
                    float lineEndX = body->position.x + cos(body->rotation) * circle->radius;
                    float lineEndY = body->position.y + sin(body->rotation) * circle->radius;
                    Graphics::DrawLine(
                        body->position.x, body->position.y,
                        lineEndX, lineEndY, 0xFFFFFFFF);
                }
            }
            else if (body->shape->GetType() == BOX)
            {
                BoxShape *box = dynamic_cast<BoxShape *>(body->shape.get());

                if (body->IsStatic())
                {
                    // Draw static platforms with material-specific colors but darker
                    Uint32 darkColor = (bodyColor & 0xFF000000) | ((bodyColor >> 1) & 0x7F7F7F);
                    Graphics::DrawFillRect(
                        body->position.x, body->position.y,
                        box->width, box->height, darkColor);
                }
                else
                {
                    Graphics::DrawFillRect(
                        body->position.x, body->position.y,
                        box->width, box->height, bodyColor);
                }

                if (debugMode)
                {
                    Graphics::DrawRect(
                        body->position.x, body->position.y,
                        box->width, box->height, 0xFFFFFFFF);
                }
            }
        }

        // Draw UI elements
        DrawUI();

        Graphics::RenderFrame();
    }

    Uint32 GetBodyColor(Body *body)
    {
        // Determine color based on material properties
        float restitution = body->restitution;
        float friction = body->friction;

        // Find closest material match
        int bestMatch = 0;
        float bestDistance = 999.0f;

        for (int i = 0; i < 5; ++i)
        {
            float distance = abs(materials[i].restitution - restitution) +
                             abs(materials[i].friction - friction);
            if (distance < bestDistance)
            {
                bestDistance = distance;
                bestMatch = i;
            }
        }

        return materials[bestMatch].color;
    }

    void DrawUI()
    {
        // Draw material selector
        int startX = 20;
        int startY = 20;

        for (int i = 0; i < 5; ++i)
        {
            Uint32 color = materials[i].color;
            if (i == currentMaterial)
            {
                // Highlight current selection
                Graphics::DrawFillRect(startX + i * 60, startY, 50, 30, color);
                Graphics::DrawRect(startX + i * 60, startY, 50, 30, 0xFFFFFFFF);
            }
            else
            {
                Graphics::DrawFillRect(startX + i * 60, startY, 50, 30, color & 0x7FFFFFFF);
                Graphics::DrawRect(startX + i * 60, startY, 50, 30, 0xFF888888);
            }
        }

        // Draw instructions if debug mode is on
        if (debugMode)
        {
            // Simple instruction indicators
            Graphics::DrawFillRect(20, Graphics::Height() - 60, 200, 40, 0x80000000);
        }
    }

    void Destroy()
    {
        Graphics::CloseWindow();
    }
};

int main()
{
    CollisionDemo demo;

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
