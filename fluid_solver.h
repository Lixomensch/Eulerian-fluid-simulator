#pragma once

#include <SFML/Graphics.hpp>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <string_view>

#include "app_constants.h"
#include "fluid.h"

class FluidSolver {
    struct StepProfiler {
        std::chrono::duration<double, std::milli> integrateMs{ 0.0 };
        std::chrono::duration<double, std::milli> projectionMs{ 0.0 };
        std::chrono::duration<double, std::milli> advectMs{ 0.0 };
        std::chrono::duration<double, std::milli> smokeMs{ 0.0 };
        int frames = 0;

        void add(
            std::chrono::duration<double, std::milli> integrate,
            std::chrono::duration<double, std::milli> projection,
            std::chrono::duration<double, std::milli> advect,
            std::chrono::duration<double, std::milli> smoke
        ) {
            integrateMs += integrate;
            projectionMs += projection;
            advectMs += advect;
            smokeMs += smoke;
            frames++;

            if (frames >= AppConstants::PERF_REPORT_EVERY_N_STEPS) {
                const double inv = 1.0 / static_cast<double>(frames);
                std::cerr
                    << "[perf] integrate=" << integrateMs.count() * inv << "ms "
                    << "project=" << projectionMs.count() * inv << "ms "
                    << "advect=" << advectMs.count() * inv << "ms "
                    << "smoke=" << smokeMs.count() * inv << "ms"
                    << std::endl;

                integrateMs = std::chrono::duration<double, std::milli>(0.0);
                projectionMs = std::chrono::duration<double, std::milli>(0.0);
                advectMs = std::chrono::duration<double, std::milli>(0.0);
                smokeMs = std::chrono::duration<double, std::milli>(0.0);
                frames = 0;
            }
        }
    };

    Fluid fluidState;
    float gravity;
    float brushRadius;
    float simulationTime;
    sf::Vector2i previousBrushPosition;
    bool hasPreviousBrushPosition;
    bool useJacobi;
    StepProfiler profiler;

public:
    FluidSolver(int x, int y, int h)
        : fluidState(x, y, h),
          gravity(0.0f),
          brushRadius(AppConstants::DEFAULT_BRUSH_RADIUS),
          simulationTime(0.0f),
          previousBrushPosition(0, 0),
          hasPreviousBrushPosition(false),
#ifdef FLUID_USE_JACOBI
          useJacobi(true) {}
#else
          useJacobi(false) {}
#endif

    Fluid& fluid() {
        return fluidState;
    }

    const Fluid& fluid() const {
        return fluidState;
    }

    float getBrushRadius() const {
        return brushRadius;
    }

    bool isUsingJacobi() const {
        return useJacobi;
    }

    void toggleSolverMode() {
        useJacobi = !useJacobi;
    }

    std::string_view solverModeLabel() const {
        return useJacobi ? "Jacobi" : "RBGS";
    }

    void step(float dt, int numIters = AppConstants::DEFAULT_SOLVER_ITERS) {
        const auto t0 = std::chrono::steady_clock::now();
        fluidState.integrate(dt, gravity);
        fluidState.boundary_velocities();
        const auto t1 = std::chrono::steady_clock::now();

        int iterationBudget = std::max(1, std::min(numIters, AppConstants::PROJECTION_MAX_ITERS));
        int usedIterations = 0;
        float residual = 0.0f;
        fluidState.incompressibility(
            iterationBudget,
            useJacobi,
            AppConstants::PROJECTION_RESIDUAL_EPSILON,
            usedIterations,
            residual
        );
        fluidState.boundary_velocities();
        const auto t2 = std::chrono::steady_clock::now();

        fluidState.advect_velocity(dt);
        fluidState.boundary_velocities();
        const auto t3 = std::chrono::steady_clock::now();

        fluidState.move_smoke(dt);
        fluidState.decay_density(dt, AppConstants::DENSITY_DECAY_PER_SECOND);
        const auto t4 = std::chrono::steady_clock::now();

        if (AppConstants::ENABLE_PERF_LOGS) {
            profiler.add(t1 - t0, t2 - t1, t3 - t2, t4 - t3);
        }

        simulationTime += dt;
    }

    void reset() {
        std::fill(fluidState.pressure.begin(), fluidState.pressure.end(), 0.0f);
        std::fill(fluidState.u.begin(), fluidState.u.end(), 0.0f);
        std::fill(fluidState.v.begin(), fluidState.v.end(), 0.0f);
        std::fill(fluidState.solid.begin(), fluidState.solid.end(), 0.0f);
        std::fill(fluidState.m.begin(), fluidState.m.end(), 0.0f);
        hasPreviousBrushPosition = false;
    }

    void applyBrush(const sf::Vector2i& screenMousePos) {
        const float r = brushRadius;
        const int x = screenMousePos.x - static_cast<int>(r);
        const int y = screenMousePos.y - static_cast<int>(r);
        const int n = fluidState.grid_dimension.y;

        if (!hasPreviousBrushPosition) {
            previousBrushPosition = sf::Vector2i(x, y);
            hasPreviousBrushPosition = true;
        }

        const float vx = static_cast<float>(x - previousBrushPosition.x) * 10.0f;
        const float vy = static_cast<float>(y - previousBrushPosition.y) * 10.0f;
        previousBrushPosition = sf::Vector2i(x, y);

        for (int i = 1; i < fluidState.grid_dimension.x - 2; ++i) {
            const float posX = (i + 0.5f) * fluidState.cell_size;
            for (int j = 1; j < fluidState.grid_dimension.y - 2; ++j) {
                fluidState.solid[i * n + j] = 1.0f;

                const float posY = (j + 0.5f) * fluidState.cell_size;
                const float dx = posX - x;
                const float dy = posY - y;

                if (dx * dx + dy * dy < r * r) {
                    fluidState.m[i * n + j] = 0.5f + 0.2f * std::sin(0.3f * simulationTime);
                    fluidState.u[i * n + j] = vx;
                    fluidState.u[(i + 1) * n + j] = vx;
                    fluidState.v[i * n + j] = vy;
                    fluidState.v[i * n + j + 1] = vy;
                }
            }
        }
    }
};
