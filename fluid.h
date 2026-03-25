#pragma once

#include <SFML/Graphics.hpp>

#include <algorithm>
#include <cmath>
#include <vector>

class Fluid
{
public:
    float density;
    sf::Vector2i grid_dimension;
    int cell_size;
    int num_cells;
    std::vector<float> u, v, new_u, new_v, pressure, pressure_new, divergence, solid, m, new_m;
    int grid;

    Fluid(int x, int y, int h)
        : density(1000.0f), grid_dimension(x + 2, y + 2), cell_size(h), num_cells(grid_dimension.x * grid_dimension.y) {
        u.resize(num_cells);
        v.resize(num_cells);
        new_u.resize(num_cells);
        new_v.resize(num_cells);
        pressure.resize(num_cells);
        pressure_new.resize(num_cells);
        divergence.resize(num_cells);
        solid.resize(num_cells);
        m.resize(num_cells);
        new_m.resize(num_cells);
        grid = grid_dimension.x * grid_dimension.y;
    }

    int idx(int i, int j) const {
        return i * grid_dimension.y + j;
    }

    void integrate(float dt, float gravity) {
        int n = grid_dimension.y;
        for (int i = 1; i < grid_dimension.x - 1; i++) {
            for (int j = 1; j < grid_dimension.y - 1; j++) {
                if (solid[i * n + j] != 0.0f && solid[i * n + j - 1] != 0.0f) {
                    v[i * n + j] += gravity * dt;
                }
            }
        }
    }

    void incompressibility(
        int maxIterations,
        bool useJacobi,
        float residualEpsilon,
        int& outUsedIterations,
        float& outResidual
    ) {
        std::fill(pressure.begin(), pressure.end(), 0.0f);
        std::fill(pressure_new.begin(), pressure_new.end(), 0.0f);

#ifdef _OPENMP
#pragma omp parallel for collapse(2)
#endif
        for (int i = 1; i < grid_dimension.x - 1; i++) {
            for (int j = 1; j < grid_dimension.y - 1; j++) {
                int c = idx(i, j);
                if (solid[c] <= 0.0f) {
                    divergence[c] = 0.0f;
                    continue;
                }

                divergence[c] =
                    u[idx(i + 1, j)] - u[idx(i, j)] +
                    v[idx(i, j + 1)] - v[idx(i, j)];
            }
        }

        outUsedIterations = 0;
        outResidual = 0.0f;

        for (int it = 0; it < maxIterations; it++) {
            float maxDelta = 0.0f;

            if (useJacobi) {
#ifdef _OPENMP
#pragma omp parallel for collapse(2) reduction(max:maxDelta)
#endif
                for (int i = 1; i < grid_dimension.x - 1; i++) {
                    for (int j = 1; j < grid_dimension.y - 1; j++) {
                        int c = idx(i, j);
                        if (solid[c] <= 0.0f) {
                            pressure_new[c] = 0.0f;
                            continue;
                        }

                        float s_up = solid[idx(i - 1, j)] > 0.0f ? 1.0f : 0.0f;
                        float s_down = solid[idx(i + 1, j)] > 0.0f ? 1.0f : 0.0f;
                        float s_left = solid[idx(i, j - 1)] > 0.0f ? 1.0f : 0.0f;
                        float s_right = solid[idx(i, j + 1)] > 0.0f ? 1.0f : 0.0f;
                        float s = s_up + s_down + s_left + s_right;

                        if (s <= 0.0f) {
                            pressure_new[c] = 0.0f;
                            continue;
                        }

                        float neighborSum =
                            pressure[idx(i - 1, j)] * s_up +
                            pressure[idx(i + 1, j)] * s_down +
                            pressure[idx(i, j - 1)] * s_left +
                            pressure[idx(i, j + 1)] * s_right;

                        float nextPressure = (neighborSum - divergence[c]) / s;
                        maxDelta = std::max(maxDelta, std::fabs(nextPressure - pressure[c]));
                        pressure_new[c] = nextPressure;
                    }
                }

                pressure.swap(pressure_new);
            }
            else {
                for (int color = 0; color < 2; color++) {
#ifdef _OPENMP
#pragma omp parallel for collapse(2) reduction(max:maxDelta)
#endif
                    for (int i = 1; i < grid_dimension.x - 1; i++) {
                        for (int j = 1; j < grid_dimension.y - 1; j++) {
                            if (((i + j) & 1) != color) {
                                continue;
                            }

                            int c = idx(i, j);
                            if (solid[c] <= 0.0f) {
                                pressure[c] = 0.0f;
                                continue;
                            }

                            float s_up = solid[idx(i - 1, j)] > 0.0f ? 1.0f : 0.0f;
                            float s_down = solid[idx(i + 1, j)] > 0.0f ? 1.0f : 0.0f;
                            float s_left = solid[idx(i, j - 1)] > 0.0f ? 1.0f : 0.0f;
                            float s_right = solid[idx(i, j + 1)] > 0.0f ? 1.0f : 0.0f;
                            float s = s_up + s_down + s_left + s_right;

                            if (s <= 0.0f) {
                                pressure[c] = 0.0f;
                                continue;
                            }

                            float neighborSum =
                                pressure[idx(i - 1, j)] * s_up +
                                pressure[idx(i + 1, j)] * s_down +
                                pressure[idx(i, j - 1)] * s_left +
                                pressure[idx(i, j + 1)] * s_right;

                            float oldPressure = pressure[c];
                            pressure[c] = (neighborSum - divergence[c]) / s;
                            maxDelta = std::max(maxDelta, std::fabs(pressure[c] - oldPressure));
                        }
                    }
                }
            }

            outUsedIterations = it + 1;
            outResidual = maxDelta;
            if (maxDelta <= residualEpsilon) {
                break;
            }
        }

#ifdef _OPENMP
#pragma omp parallel for collapse(2)
#endif
        for (int i = 1; i < grid_dimension.x - 1; i++) {
            for (int j = 1; j < grid_dimension.y - 1; j++) {
                int c = idx(i, j);
                if (solid[c] <= 0.0f) {
                    continue;
                }

                if (solid[idx(i - 1, j)] > 0.0f) {
                    u[idx(i, j)] -= pressure[c] - pressure[idx(i - 1, j)];
                }
                if (solid[idx(i, j - 1)] > 0.0f) {
                    v[idx(i, j)] -= pressure[c] - pressure[idx(i, j - 1)];
                }
            }
        }
    }

    void boundary_velocities() {
        int n = grid_dimension.y;
        for (int i = 0; i < grid_dimension.x; i++) {
            u[i * n] = u[i * n + 1];
            u[i * n + grid_dimension.y - 1] = u[i * n + grid_dimension.y - 2];
        }

        for (int j = 0; j < grid_dimension.y; j++) {
            v[j] = v[n + j];
            v[(grid_dimension.x - 1) * n + j] = v[(grid_dimension.x - 2) * n + j];
        }
    }

    float sample_field(float x, float y, int field) const {
        int n = grid_dimension.y;
        float h = static_cast<float>(cell_size);
        float h1 = 1.0f / h;
        float h2 = 0.5f * h;

        x = std::max(std::min(x, grid_dimension.x * h), h);
        y = std::max(std::min(y, grid_dimension.y * h), h);

        float dx = 0.0f;
        float dy = 0.0f;
        const std::vector<float>* f;

        switch (field) {
        case 0: f = &u; dy = h2; break;
        case 1: f = &v; dx = h2; break;
        case 2: f = &m; dx = h2; dy = h2; break;
        default: return 0.0f;
        }

        int left = std::min(static_cast<int>(std::floor((x - dx) * h1)), grid_dimension.x - 1);
        float tx = ((x - dx) - left * h) * h1;
        int right = std::min(left + 1, grid_dimension.x - 1);

        int down = std::min(static_cast<int>(std::floor((y - dy) * h1)), grid_dimension.y - 1);
        float ty = ((y - dy) - down * h) * h1;
        int up = std::min(down + 1, grid_dimension.y - 1);

        float sx = 1.0f - tx;
        float sy = 1.0f - ty;

        return sx * sy * (*f)[left * n + down] +
            tx * sy * (*f)[right * n + down] +
            tx * ty * (*f)[right * n + up] +
            sx * ty * (*f)[left * n + up];
    }

    float average_u(int i, int j) const {
        int n = grid_dimension.y;
        return (u[i * n + j - 1] + u[i * n + j] + u[(i + 1) * n + j - 1] + u[(i + 1) * n + j]) * 0.25f;
    }

    float average_v(int i, int j) const {
        int n = grid_dimension.y;
        return (v[i * n + j - 1] + v[i * n + j] + v[(i + 1) * n + j - 1] + v[(i + 1) * n + j]) * 0.25f;
    }

    void advect_velocity(float dt) {
        std::copy(u.begin(), u.end(), new_u.begin());
        std::copy(v.begin(), v.end(), new_v.begin());

        int n = grid_dimension.y;
        float h = static_cast<float>(cell_size);
        float h2 = 0.5f * h;

#ifdef _OPENMP
#pragma omp parallel for collapse(2)
#endif
        for (int i = 1; i < grid_dimension.x; i++) {
            for (int j = 1; j < grid_dimension.y; j++) {
                // u component
                if (solid[i * n + j] != 0.0f && solid[(i - 1) * n + j] != 0.0f && j < grid_dimension.y - 1) {
                    float x = i * h;
                    float y = j * h + h2;
                    float u_l = u[i * n + j];
                    float v_l = average_v(i, j);
                    x -= dt * u_l;
                    y -= dt * v_l;
                    u_l = sample_field(x, y, 0);
                    new_u[i * n + j] = u_l;
                }
                // v component
                if (solid[i * n + j] != 0.0f && solid[i * n + j - 1] != 0.0f && i < grid_dimension.x - 1) {
                    float x = i * h + h2;
                    float y = j * h;
                    float u_l = average_u(i, j);
                    float v_l = v[i * n + j];
                    x -= dt * u_l;
                    y -= dt * v_l;
                    v_l = sample_field(x, y, 1);
                    new_v[i * n + j] = v_l;
                }
            }
        }

        u.swap(new_u);
        v.swap(new_v);
    }

    void move_smoke(float dt) {
        std::copy(m.begin(), m.end(), new_m.begin());

        int n = grid_dimension.y;
        float h = static_cast<float>(cell_size);
        float h2 = 0.5f * h;

#ifdef _OPENMP
#pragma omp parallel for collapse(2)
#endif
        for (int i = 1; i < grid_dimension.x - 1; i++) {
            for (int j = 1; j < grid_dimension.y - 1; j++) {
                if (solid[i * n + j] != 0.0f) {
                    float u_l = (u[i * n + j] + u[(i + 1) * n + j]) * 0.5f;
                    float v_l = (v[i * n + j] + v[i * n + j + 1]) * 0.5f;
                    float x = i * h + h2 - dt * u_l;
                    float y = j * h + h2 - dt * v_l;

                    new_m[i * n + j] = sample_field(x, y, 2);
                }
            }
        }

        m.swap(new_m);
    }

    void decay_density(float dt, float decayPerSecond) {
        const int n = grid_dimension.y;
        const float factor = std::exp(-decayPerSecond * dt);

#ifdef _OPENMP
#pragma omp parallel for collapse(2)
#endif
        for (int i = 1; i < grid_dimension.x - 1; i++) {
            for (int j = 1; j < grid_dimension.y - 1; j++) {
                if (solid[i * n + j] != 0.0f) {
                    float& densityCell = m[i * n + j];
                    densityCell *= factor;
                    if (densityCell < 0.0005f) {
                        densityCell = 0.0f;
                    }
                }
            }
        }
    }
};