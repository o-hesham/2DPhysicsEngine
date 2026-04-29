#ifndef APPLICATION_H
#define APPLICATION_H

#include "./Graphics.h"
#include "Physics/World.h"
#include <vector>
#include <memory>

class Application
{
private:
    bool debug = false;
    bool running = false;
    std::unique_ptr<World> world;

public:
    Application() = default;
    ~Application() = default;
    bool IsRunning();
    void Setup();
    void Input();
    void Update();
    void Render();
    void Destroy();
};

#endif