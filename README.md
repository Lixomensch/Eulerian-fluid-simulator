# Eulerian Fluid Simulator

Eulerian Fluid Simulator is an educational real-time fluid simulation project based on an Eulerian grid approach. The project focuses on numerical methods, performance optimization, and interactive visualization using SFML.

## Technologies Used

- **Makefile**: Build system used to compile and test the simulator through simple commands such as `make test` and `make release`.

- **SFML**: Utilized for graphical rendering and user interaction, offering a user-friendly and responsive interface for visualizing simulation results.

- **C++**: Primary programming language for implementing the simulator, providing performance and direct control over intensive computational operations.

## Features

- Eulerian grid fluid simulation.
- Semi-Lagrangian advection for velocity and smoke.
- Pressure projection with two modes:
	- **RBGS (Red-Black Gauss-Seidel)**
	- **Jacobi**
- Runtime solver mode switching via clickable top badge.
- Batched rendering path for improved performance.

## Build and Run

Requirements:

- `g++` with C++17 support
- `make`
- SFML 3 (graphics/window/system)

Commands:

```bash
# Debug build validation
make test

# Optimized release build
make release

# Run release binary
./bin/eulerian-fluid-simulator
```

## Controls

- **Left mouse button on simulation area**: inject and move smoke.
- **Reset button**: clears fluid state.
- **Mode badge (top-right)**: switches solver mode RBGS <-> Jacobi at runtime.

## Numerical Notes

- Pressure projection now supports residual-based stopping.
- Advection sampling uses floating-point backtrace coordinates.
- Boundary velocity updates are explicitly applied in the solver pipeline.
- Occupancy semantics are treated as binary in brush injection.

## References

- [Ten Minute Physics](https://matthias-research.github.io/pages/tenMinutePhysics/index.html)
- [YouTube Video](https://www.youtube.com/watch?v=iKAVRgIrUOU)

## License

The code is available under the [MIT license](LICENSE).
