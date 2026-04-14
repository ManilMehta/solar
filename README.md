# Solar System Simulator

A real-time 3D N-body gravitational simulation of the solar system, written in C++ with OpenGL.

Simulates the Sun, all 8 planets, and major moons (Earth's Moon, Phobos, Deimos, the four Galilean moons, Titan, and Triton) using real astronomical data from NASA JPL Horizons (J2000 epoch).

## Features

- **N-body gravity** — full pairwise gravitational interaction between all 18 bodies
- **Velocity Verlet integration** — symplectic integrator with excellent long-term energy conservation
- **Real data** — initial positions and velocities from NASA JPL Horizons
- **3D rendering** — OpenGL 3.3 core profile with Phong shading
- **Orbit trails** — fading trail lines showing orbital paths
- **Interactive camera** — orbit, zoom, and focus on any body
- **Time controls** — pause, reverse, speed up/slow down simulation

## Building

Requires CMake 3.20+ and a C++17 compiler. All other dependencies (GLFW, GLAD, GLM) are fetched automatically.

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
```

On macOS, replace `$(nproc)` with `$(sysctl -n hw.ncpu)`.

## Running

```bash
cd build
./solar
```

The executable must be run from the `build/` directory so it can find the `shaders/` folder.

## Controls

| Key              | Action                              |
| ---------------- | ----------------------------------- |
| **Left drag**    | Rotate camera                       |
| **Scroll**       | Zoom in/out                         |
| **0–8**          | Focus on Sun / planets              |
| **Tab**          | Cycle focus through all bodies      |
| **Space**        | Pause / resume                      |
| **R**            | Reverse time                        |
| **+** / **Up**   | Double simulation speed             |
| **-** / **Down** | Halve simulation speed              |
| **Esc**          | Quit                                |

## Architecture

```
src/
├── main.cpp                        — window, main loop, input callbacks
├── simulation/
│   ├── body.h                      — CelestialBody struct
│   ├── physics.h/cpp               — N-body gravity + Verlet integrator
│   └── solar_system_data.h         — J2000 initial conditions for all bodies
├── rendering/
│   ├── renderer.h/cpp              — draws bodies and trails
│   ├── camera.h/cpp                — arcball orbit camera
│   ├── shader.h/cpp                — GLSL shader compilation
│   ├── sphere_mesh.h/cpp           — UV-sphere mesh generation
│   └── trail.h/cpp                 — orbit trail ring buffer
└── shaders/
    ├── body.vert/frag              — Phong-lit planet shader
    └── trail.vert/frag             — colored line shader
```

## Physics

- **Units**: AU (distance), days (time), solar masses (mass)
- **G** = 2.9591×10⁻⁴ AU³/(M☉·day²)
- **Integration**: Velocity Verlet (symplectic, 2nd order)
- **Default timestep**: 0.5 days, with 10 substeps per render frame
