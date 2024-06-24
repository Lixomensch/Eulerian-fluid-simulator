#pragma once

class fluid
{
	float density;
	sf::Vector2i grid_dimension;
	int tam_cell;
	int num_cells;
	std::vector<float> u;
	std::vector<float> v;
	std::vector<float> new_u;
	std::vector<float> new_v;
	std::vector<float> pressure;
	std::vector<float> solid;
	std::vector<float> m;
	std::vector<float> new_m;
	int grid;

	fluid(int x, int y, int h) 
	{	
		density = 1000.0;
		grid_dimension.x = x+2;
		grid_dimension.y = y+2;
		int tam_cell = h;
		num_cells = grid_dimension.x* grid_dimension.y;
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
				if (solid[i * n + j] != 0.0 && solid[i * n + j - 1] != 0.0)
					v[i * n + j] += gravity * dt;
			}
		}
	}

	void incompressibility(int num_it, float dt) {
		int n = grid_dimension.y;
		float cp = density * tam_cell / dt;

		for (int it = 0; it < num_it; it++) {
			for (int i = 1; i < grid_dimension.x - 1; i++) {
				for (int j = 1; j < grid_dimension.y - 1; j++) {
					if (solid[i * n + j] == 0.0)
						continue;

					float s = solid[i * n + j];
					float s_up = solid[(i - 1) * n + j];
					float s_down = solid[(i + 1) * n + j];
					float s_left = solid[i * n + j - 1];
					float s_right = solid[i * n + j + 1];
					float s = s_up + s_down + s_left + s_right;

					if (s == 0.0)
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
			u[i * n + 0] = u[i * n + 1];
			u[i * n + grid_dimension.y - 1] = u[i * n + grid_dimension.y - 2];
		}

		for (int j = 0; j < grid_dimension.y; j++) {
			v[0 * n + j] = v[1 * n + j];
			v[(grid_dimension.x - 1) * n + j] = v[(grid_dimension.x - 2) * n + j];
		}
	}
};

