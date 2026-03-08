## Overview
Implementation of steering behaviors for autonomous agents, progressing from basic behaviors to flocking, optimized with spatial partitioning.

## Implemented Features

### Basic Steering Behaviors
- **Seek/Flee** - Move towards/away from target
- **Arrival** - Smooth deceleration
- **Wander** - Random steering
- **Pursuit/Evasion** - Predict target movement

### Blended Behaviors
- Weighted combination of multiple behaviors
- Priority-based steering

### Flocking
- **Separation** - Avoid crowding neighbors
- **Velocity match** - Match neighbor heading
- **Cohesion** - Move toward group center

### Spatial Partitioning
Grid-based optimization for more efficient neighbor detection

