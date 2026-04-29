# Forces Demo

This demo showcases various force systems and how they affect physics bodies in real-time.

## Features

- **Multiple Force Systems**: Gravitational attraction, drag, springs, friction
- **Interactive Toggles**: Enable/disable forces with keyboard controls
- **Real-time Visual Feedback**: See forces in action
- **Multiple Body Types**: Different masses and shapes to demonstrate force effects
- **Performance Monitoring**: Watch how different forces affect performance

## Controls

| Input           | Action                            |
| --------------- | --------------------------------- |
| **Left Click**  | Add new body at cursor position   |
| **Right Click** | Add heavy body at cursor position |
| **G**           | Toggle gravitational attraction   |
| **D**           | Toggle drag forces                |
| **S**           | Toggle spring forces              |
| **F**           | Toggle friction forces            |
| **R**           | Reset simulation                  |
| **SPACE**       | Pause/Unpause simulation          |
| **ESC**         | Exit the demo                     |

## Force Systems Demonstrated

### 1. **Gravitational Attraction**

```cpp
Vec2 gravForce = Force::GenerateGravitationalForce(*bodyA, *bodyB, G);
```

- Bodies attract each other based on mass and distance
- Demonstrates Newton's law of universal gravitation
- Watch large masses pull smaller objects

### 2. **Drag Forces**

```cpp
Vec2 dragForce = Force::GenerateDragForce(*body, dragCoefficient);
```

- Air/fluid resistance that opposes motion
- Proportional to velocity (or velocity squared)
- Causes moving objects to slow down naturally

### 3. **Spring Forces**

```cpp
Vec2 springForce = Force::GenerateSpringForce(*bodyA, *bodyB, restLength, k);
```

- Elastic connections between bodies
- Creates oscillating behavior
- Demonstrates Hooke's law (F = -kx)

### 4. **Friction Forces**

```cpp
Vec2 frictionForce = Force::GenerateFriction(*body, frictionCoefficient);
```

- Surface friction that resists sliding motion
- Static and kinetic friction effects
- Realistic ground interaction

## Visual Indicators

- **Green Text**: Force system is ENABLED
- **Red Text**: Force system is DISABLED
- **Body Colors**: Different masses have different colors
- **Motion Trails**: See the paths objects take under different forces

## Physics Concepts Demonstrated

### Force Accumulation

Multiple forces can act on a single body simultaneously:

```cpp
body->AddForce(gravityForce);
body->AddForce(dragForce);
body->AddForce(frictionForce);
```

### Force Scaling

Different force types scale differently:

- **Gravity**: Proportional to mass product, inverse square of distance
- **Drag**: Proportional to velocity (linear) or velocity squared
- **Spring**: Proportional to displacement from rest length
- **Friction**: Proportional to normal force

## Educational Experiments

1. **Enable only gravity**: Watch bodies cluster together
2. **Add drag forces**: See how air resistance affects motion
3. **Enable springs**: Create connected body systems
4. **Combine all forces**: Observe complex emergent behaviors

## Building and Running

```bash
# Build the demo
g++ -std=c++17 -I../../include \
    main.cpp \
    ../../src/Application.cpp ../../src/Graphics.cpp \
    ../../src/Physics/*.cpp ../../src/Math/*.cpp ../../src/SDL2Ext/*.c \
    -lSDL2 -lSDL2_image -o forces_demo

# Run the demo
./forces_demo
```

## Technical Notes

- Forces are applied each frame during the physics update
- Force accumulation happens in the Body class
- Each force generator is implemented as a separate function
- Performance scales with number of bodies and active force systems

Experiment with different combinations of forces to see how they interact and create complex, realistic physics behaviors!
