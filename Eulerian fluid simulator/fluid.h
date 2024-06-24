#pragma once

class fluid
{
	float density;
	sf::Vector2i grid_dimension;
	int num_cells;
	std::vector<float> u;
	std::vector<float> v;
	std::vector<float> new_u;
	std::vector<float> new_v;
	std::vector<float> pressure;
	std::vector<float> s;
	std::vector<float> m;
	std::vector<float> new_m;
	int grid;

	fluid() 
	{	
		density = 1000.0;
		grid_dimension.x = 1366;
		grid_dimension.y = 718;
		num_cells = grid_dimension.x* grid_dimension.y;
		u.resize(num_cells);
		v.resize(num_cells);
		new_u.resize(num_cells);
		new_v.resize(num_cells);
		pressure.resize(num_cells);
		s.resize(num_cells);
		m.resize(num_cells);
		new_m.resize(num_cells);
		grid = grid_dimension.x * grid_dimension.y;
	}
};

