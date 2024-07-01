#pragma once

class Fluid
{
public:
    float density;
    sf::Vector2i grid_dimension;
    int cell_size;
    int num_cells;
    std::vector<float> u, v, new_u, new_v, pressure, solid, m, new_m;
    int grid;

    Fluid(int x, int y, int h)
        : grid_dimension(x + 2, y + 2), cell_size(h), density(1000.0f), num_cells(grid_dimension.x* grid_dimension.y) {
        u.resize(num_cells);
        v.resize(num_cells);
        new_u.resize(num_cells);
        new_v.resize(num_cells);
        pressure.resize(num_cells);
        solid.resize(num_cells);
        m.resize(num_cells);
        new_m.resize(num_cells);
        grid = grid_dimension.x * grid_dimension.y;
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

    void incompressibility(int num_it, float dt) {
        int n = grid_dimension.y;
        float cp = density * cell_size / dt;

        for (int it = 0; it < num_it; it++) {
            for (int i = 1; i < grid_dimension.x - 1; i++) {
                for (int j = 1; j < grid_dimension.y - 1; j++) {

                    float s_up = solid[(i - 1) * n + j];
                    float s_down = solid[(i + 1) * n + j];
                    float s_left = solid[i * n + j - 1];
                    float s_right = solid[i * n + j + 1];
                    float s = s_up + s_down + s_left + s_right;

                    if (s == 0.0f)
                        continue;

                    float divergence = u[(i + 1) * n + j] - u[i * n + j] +
                        v[i * n + j + 1] - v[i * n + j];

                    float p = -divergence / s;

                    pressure[i * n + j] += p * cp;

                    u[i * n + j] -= s_up * p;
                    u[(i + 1) * n + j] += s_down * p;
                    v[i * n + j] -= s_left * p;
                    v[i * n + j + 1] += s_right * p;
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

    float sample_field(int x, int y, int field) const {
        int n = grid_dimension.y;
        int h = cell_size;
        float h1 = 1.0f / h;
        float h2 = 0.5f * h;

        x = std::max(std::min(x, grid_dimension.x * h), h);
        y = std::max(std::min(y, grid_dimension.y * h), h);

        int dx = 0;
        int dy = 0;
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
        new_u = u;
        new_v = v;

        int n = grid_dimension.y;
        int h = cell_size;
        float h2 = 0.5f * h;

        for (int i = 1; i < grid_dimension.x; i++) {
            for (int j = 1; j < grid_dimension.y; j++) {
                // u component
                if (solid[i * n + j] != 0.0f && solid[(i - 1) * n + j] != 0.0f && j < grid_dimension.y - 1) {
                    int x = i * h;
                    int y = j * h + h2;
                    float u_l = u[i * n + j];
                    float v_l = average_v(i, j);
                    x = x - dt * u_l;
                    y = y - dt * v_l;
                    u_l = sample_field(x, y, 0);
                    new_u[i * n + j] = u_l;
                }
                // v component
                if (solid[i * n + j] != 0.0f && solid[i * n + j - 1] != 0.0f && i < grid_dimension.x - 1) {
                    int x = i * h + h2;
                    int y = j * h;
                    float u_l = average_u(i, j);
                    float v_l = v[i * n + j];
                    x = x - dt * u_l;
                    y = y - dt * v_l;
                    v_l = sample_field(x, y, 1);
                    new_v[i * n + j] = v_l;
                }
            }
        }

        u = new_u;
        v = new_v;
    }

    void move_smoke(float dt) {
        new_m = m;

        int n = grid_dimension.y;
        int h = cell_size;
        float h2 = 0.5f * h;

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

        m = new_m;
    }
};