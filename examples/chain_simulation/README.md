# Chain Simulation Demo

This demo demonstrates the 2D Physics Engine's **joint constraint system** for creating connected bodies and realistic chain physics.

## Features

- **Proper Joint Constraints**: Uses `JointConstraint` class for rigid connections between bodies
- **Interactive Chain Creation**: Click anywhere to create new chain systems with proper constraints
- **Realistic Chain Physics**: Distance-preserving constraints that maintain rigid connections
- **Pendulum Systems**: Simple and compound pendulum demonstrations
- **Debug Visualization**: Toggle constraint visualization to see joint connections

## Controls

| Input           | Action                                          |
| --------------- | ----------------------------------------------- |
| **Left Click**  | Create new chain at cursor position             |
| **Right Click** | Create simple pendulum at cursor position       |
| **R**           | Reset simulation to initial state               |
| **D**           | Toggle debug mode (show constraint connections) |
| **ESC**         | Exit the demo                                   |

## Physics Concepts Demonstrated

### 1. **Joint Constraints**

Chain links are connected using proper `JointConstraint` objects that maintain rigid distance relationships between bodies.

### 2. **Constraint-Based Physics**

```cpp
auto joint = std::make_unique<JointConstraint>(bodyA, bodyB, connectionPoint);
world->AddConstraint(std::move(joint));
```

### 3. **Static Anchor Points**

The first link of each chain is connected to a static body (mass = 0) that serves as an immovable anchor point.

### 4. **Multi-Body Constraint Systems**

Demonstrates how multiple connected bodies interact through a network of joint constraints with proper impulse resolution.

## Technical Implementation

- **Joint Constraint Solver**: Uses the engine's built-in constraint solving system with Baumgarte stabilization
- **Rigid Connections**: Maintains exact distances without the instability of spring forces
- **Collision Integration**: Chain links can collide with the environment while maintaining constraint relationships
- **Debug Visualization**: Optional rendering of constraint connections for educational purposes

## Building and Running

```bash
# Build the demo
g++ -std=c++17 -I../../include \
    main.cpp \
    ../../src/Application.cpp ../../src/Graphics.cpp \
    ../../src/Physics/*.cpp ../../src/Math/*.cpp ../../src/SDL2Ext/*.c \
    -lSDL2 -lSDL2_image -o chain_simulation

# Run the demo
./chain_simulation
```

## Educational Value

This demo is perfect for understanding:

- How joint constraint systems work in modern physics engines
- The difference between spring forces and proper rigid constraints
- Multi-body constraint solving and impulse resolution
- Interactive constraint-based physics system design

## Advantages of Joint Constraints vs Spring Forces

**Joint Constraints:**

- ✅ Maintain exact distances (no stretch/compression)
- ✅ More stable and predictable behavior
- ✅ Better performance with many connected bodies
- ✅ No tuning of spring constants required

**Spring Forces (previous approach):**

- ❌ Can be unstable with high spring constants
- ❌ Allow unrealistic stretching/compression
- ❌ Require careful tuning of parameters
- ❌ Can lead to numerical instabilities

Try creating multiple chains and pendulums to see how proper constraint solving creates realistic, stable physics behavior!
