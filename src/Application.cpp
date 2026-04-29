#include "Application.h"
#include "Physics/Constants.h"
#include "Physics/Force.h"
#include "Physics/CollisionDetection.h"
#include "Physics/Contact.h"

bool Application::IsRunning()
{
    return running;
}
///////////////////////////////////////////////////////////////////////////////
// Setup function (executed once in the beginning of the simulation)
///////////////////////////////////////////////////////////////////////////////
void Application::Setup()
{
    running = Graphics::OpenWindow();

    // Create a physics world with gravity of -9.8 m/s²
    world = std::make_unique<World>(-9.8);

    auto floor = std::make_unique<Body>(std::make_unique<BoxShape>(Graphics::Width() - 50, 50), Graphics::Width() / 2.0, Graphics::Height() - 50, 0.0);
    auto leftWall = std::make_unique<Body>(std::make_unique<BoxShape>(50, Graphics::Height() - 100), 50, Graphics::Height() / 2.0 - 25, 0.0);
    auto rightWall = std::make_unique<Body>(std::make_unique<BoxShape>(50, Graphics::Height() - 100), Graphics::Width() - 50, Graphics::Height() / 2.0 - 25, 0.0);

    world->AddBody(std::move(floor));
    world->AddBody(std::move(leftWall));
    world->AddBody(std::move(rightWall));
}

///////////////////////////////////////////////////////////////////////////////
// Input processing
///////////////////////////////////////////////////////////////////////////////
void Application::Input()
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
                running = false;
            if (event.key.keysym.sym == SDLK_d)
                debug = !debug;
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT)
            {
                int x, y;
                SDL_GetMouseState(&x, &y);

                auto ball = std::make_unique<Body>(std::make_unique<CircleShape>(64), x, y, 1.0);
                ball->SetTexture("assets/basketball.png");
                ball->restitution = 0.7;
                world->AddBody(std::move(ball));
            }
            else if (event.button.button == SDL_BUTTON_RIGHT)
            {
                int x, y;
                SDL_GetMouseState(&x, &y);

                auto box = std::make_unique<Body>(std::make_unique<BoxShape>(140, 140), x, y, 1.0);
                box->SetTexture("assets/metal.png");
                box->restitution = 0.1;
                world->AddBody(std::move(box));
            }
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Update function (called several times per second to update objects)
///////////////////////////////////////////////////////////////////////////////
void Application::Update()
{
    Graphics::ClearScreen(0xFF0F0721);

    // Wait some time until target frame time is reached in milliseconds
    static int timePreviousFrame;
    int timeToWait = MILLISECS_PER_FRAME - (SDL_GetTicks() - timePreviousFrame);
    if (timeToWait > 0)
    {
        SDL_Delay(timeToWait);
    }

    // Calculate deltatime in seconds
    float deltaTime = (SDL_GetTicks() - timePreviousFrame) / 1000.0f;
    if (deltaTime > 0.016)
    {
        deltaTime = 0.016;
    }

    // Set the time of the current frame to be used in the next one
    timePreviousFrame = SDL_GetTicks();

    // Update world bodies(integrating, collision detection, etc.
    world->Update(deltaTime);
}

///////////////////////////////////////////////////////////////////////////////
// Render function (called several times per second to draw objects)
///////////////////////////////////////////////////////////////////////////////
void Application::Render()
{
    // Draw all bodies
    for (auto &body : world->GetBodies())
    {

        if (body->shape->GetType() == CIRCLE)
        {
            CircleShape *circleshape = dynamic_cast<CircleShape *>(body->shape.get());
            if (!debug && body->texture)
            {
                Graphics::DrawTexture(body->position.x, body->position.y, circleshape->radius * 2, circleshape->radius * 2, body->rotation, body->texture);
            }
            else
            {
                Graphics::DrawCircle(body->position.x, body->position.y, circleshape->radius, body->rotation, 0xFF00FF00);
            }
        }
        if (body->shape->GetType() == BOX)
        {
            BoxShape *boxShape = dynamic_cast<BoxShape *>(body->shape.get());

            if (!debug && body->texture)
            {
                Graphics::DrawTexture(body->position.x, body->position.y, boxShape->width, boxShape->height, body->rotation, body->texture);
            }
            else
            {
                Graphics::DrawPolygon(body->position.x, body->position.y, boxShape->worldVertices, 0xFF00FF00);
            }
        }
        if (body->shape->GetType() == POLYGON)
        {
            PolygonShape *polygonShape = dynamic_cast<PolygonShape *>(body->shape.get());
            if (!debug)
            {
                Graphics::DrawFillPolygon(body->position.x, body->position.y, polygonShape->worldVertices, 0xFF00FF00);
            }
            else
            {
                Graphics::DrawPolygon(body->position.x, body->position.y, polygonShape->worldVertices, 0xFF00FF00);
            }
        }
    }

    Graphics::RenderFrame();
}

///////////////////////////////////////////////////////////////////////////////
// Destroy function to delete objects and close the window
///////////////////////////////////////////////////////////////////////////////
void Application::Destroy()
{
    Graphics::CloseWindow();
}