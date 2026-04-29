# Collision Demo

This demo provides a comprehensive testing environment for collision detection and response systems with different material properties.

## Features

- **5 Material Presets**: Each with unique physical properties
- **Interactive Shape Creation**: Click to add circles and boxes
- **Debug Visualization**: See collision normals, contact points, and velocities
- **Real-time Physics**: Observe different collision behaviors
- **Performance Testing**: Add many bodies to test collision performance

## Controls

| Input           | Action                                |
| --------------- | ------------------------------------- |
| **Left Click**  | Add circle at cursor position         |
| **Right Click** | Add box at cursor position            |
| **1**           | Switch to Bouncy Ball material        |
| **2**           | Switch to Rubber material             |
| **3**           | Switch to Metal material              |
| **4**           | Switch to Wood material               |
| **5**           | Switch to Ice material                |
| **V**           | Toggle velocity visualization         |
| **C**           | Toggle collision normal visualization |
| **R**           | Reset simulation                      |
| **ESC**         | Exit the demo                         |

## Material Properties

### 1. **Bouncy Ball** 🔴

- **Restitution**: 0.9 (very bouncy)
- **Friction**: 0.3 (low friction)
- **Density**: 0.8 (light)
- **Color**: Bright Red
- **Behavior**: High energy retention, lots of bouncing

### 2. **Rubber** 🟠

- **Restitution**: 0.7 (moderately bouncy)
- **Friction**: 0.8 (high friction)
- **Density**: 1.0 (medium)
- **Color**: Orange
- **Behavior**: Good bounce with grip, realistic rubber feel

### 3. **Metal** ⚫

- **Restitution**: 0.2 (low bounce)
- **Friction**: 0.4 (medium friction)
- **Density**: 3.0 (heavy)
- **Color**: Dark Gray
- **Behavior**: Heavy, solid impacts, minimal bouncing

### 4. **Wood** 🟤

- **Restitution**: 0.4 (some bounce)
- **Friction**: 0.7 (high friction)
- **Density**: 0.6 (light)
- **Color**: Brown
- **Behavior**: Natural wood-like collisions, good friction

### 5. **Ice** 🔵

- **Restitution**: 0.3 (low bounce)
- **Friction**: 0.1 (very slippery)
- **Density**: 0.9 (light)
- **Color**: Light Blue
- **Behavior**: Slippery, slides easily, minimal friction

## Visualization Features

### Debug Overlays

- **Velocity Vectors**: Green arrows showing object velocities
- **Collision Normals**: Red lines showing collision directions
- **Contact Points**: Visual markers at collision points

### Visual Feedback

- **Material Colors**: Each material has a distinct color
- **Size Variation**: Boxes and circles of different sizes
- **Motion Trails**: See the paths objects take

## Physics Concepts Demonstrated

### 1. **Collision Detection**

- Circle-Circle collision detection
- Box-Box collision detection (AABB)
- Circle-Box hybrid detection

### 2. **Collision Response**

```cpp
// Impulse-based collision resolution
Vec2 impulse = -(1 + restitution) * relativeVelocity / (invMassA + invMassB);
bodyA->velocity += impulse * bodyA->invMass;
bodyB->velocity -= impulse * bodyB->invMass;
```

### 3. **Material Properties**

- **Restitution**: Controls bounciness (0 = no bounce, 1 = perfect bounce)
- **Friction**: Surface grip coefficient
- **Density**: Affects mass and collision response

### 4. **Momentum Conservation**

Watch how momentum is transferred between colliding objects of different masses.

## Educational Experiments

### Material Comparison

1. Drop identical shapes with different materials
2. Compare bouncing heights and sliding distances
3. Observe energy dissipation rates

### Collision Types

1. **Head-on Collisions**: Maximum momentum transfer
2. **Glancing Blows**: Partial momentum transfer with angular effects
3. **Mass Ratios**: Heavy vs light object collisions

### Performance Testing

1. Add many objects to test collision detection performance
2. Observe frame rate changes with object count
3. Test different material combinations

## Building and Running

```bash
# Build the demo
g++ -std=c++17 -I../../include \
    main.cpp \
    ../../src/Application.cpp ../../src/Graphics.cpp \
    ../../src/Physics/*.cpp ../../src/Math/*.cpp ../../src/SDL2Ext/*.c \
    -lSDL2 -lSDL2_image -o collision_demo

# Run the demo
./collision_demo
```

## Technical Implementation

### Collision Detection Pipeline

1. **Broad Phase**: Quick elimination of non-colliding pairs
2. **Narrow Phase**: Precise collision detection algorithms
3. **Contact Generation**: Calculate contact points and normals
4. **Response**: Apply impulses based on material properties

### Performance Considerations

- Spatial partitioning for large numbers of objects
- Early exit conditions for optimization
- Efficient collision algorithms for each shape pair

## Tips for Testing

- **Mix Materials**: Try collisions between different material types
- **Vary Speeds**: Test both slow and fast collisions
- **Stack Objects**: Create stable and unstable configurations
- **Use Visualization**: Enable debug overlays to understand collision mechanics

This demo is perfect for understanding collision physics, testing material properties, and learning how different physical parameters affect object behavior!
