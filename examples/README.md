# Example Projects

This directory contains example applications demonstrating various features of the 2D Physics Engine.

## Available Examples

### 1. Basic Physics Demo (`basic_demo/`)

A simple demonstration showing basic physics concepts:

- Gravity simulation
- Collision detection and response
- Multiple body types (circles, boxes)

### 2. Chain Simulation (`chain_simulation/`)

Demonstrates constraint systems and connected bodies:

- **Chain physics**: Multiple bodies connected with spring forces
- **Pendulum systems**: Realistic swinging motion
- **Interactive creation**: Click to create new chains
- **Spring constraints**: Distance maintenance between links

**Controls:**

- Left Click: Create new chain at cursor
- Right Click: Add heavy ball to scene
- R: Reset simulation
- ESC: Exit

### 3. Force Generators (`forces_demo/`)

Interactive demonstration of different force types:

- **Gravitational attraction**: Bodies attract each other based on mass
- **Drag forces**: Air resistance affecting motion
- **Spring forces**: Oscillating anchor-connected bodies
- **Friction effects**: Surface interaction simulation
- **Custom force fields**: Toggleable force systems

**Controls:**

- Left Click: Add particle at cursor
- Right Click: Add massive planet at cursor
- G: Toggle gravitational forces
- D: Toggle drag forces
- S: Toggle spring forces
- F: Toggle friction
- R: Reset simulation

### 4. Collision Demo (`collision_demo/`)

Comprehensive collision testing playground:

- **Material properties**: 5 different material presets (Bouncy, Rubber, Metal, Wood, Ice)
- **Shape variety**: Circles and boxes with different collision behaviors
- **Interactive testing**: Real-time collision experiments
- **Visual feedback**: Material-based coloring and debug visualization
- **Test environments**: Platforms with different materials and angles

**Controls:**

- Left Click: Create circle with current material
- Right Click: Create box with current material
- 1-5: Select material type (Bouncy/Rubber/Metal/Wood/Ice)
- SPACE: Drop variety of test objects
- D: Toggle debug mode
- R: Reset simulation

### 5. Performance Test (`performance_test/`)

_Coming Soon_ - Benchmarking and stress testing:

- Large numbers of bodies
- Performance profiling
- Memory usage analysis

## Building Examples

### Using Make

```bash
# Build all examples
make examples

# Build specific example
make examples/basic_demo
```

### Using CMake

```bash
mkdir build && cd build
cmake -DBUILD_EXAMPLES=ON ..
make
```

### Manual Build

```bash
# Example: Building basic_demo
g++ -std=c++17 -I../include \
    examples/basic_demo/main.cpp \
    ../src/*.cpp ../src/Physics/*.cpp ../src/Math/*.cpp \
    -lSDL2 -lSDL2_image -o basic_demo
```

## Running Examples

```bash
# From project root
./bin/basic_demo
./bin/chain_demo
./bin/forces_demo
```

Each example includes:

- Source code with comments
- README.md with specific instructions
- Asset files (if needed)
- Expected output description
