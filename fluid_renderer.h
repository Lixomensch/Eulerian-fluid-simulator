#pragma once

#include <SFML/Graphics.hpp>

#include <algorithm>
#include <cmath>

#include "app_constants.h"
#include "fluid.h"

class FluidRenderer {
    sf::VertexArray cells;
    sf::Vector2i gridDimension;
    int cellSize;

    static sf::Color mixColor(const sf::Color& a, const sf::Color& b, float t) {
        t = std::max(0.0f, std::min(1.0f, t));
        return sf::Color(
            static_cast<std::uint8_t>(a.r + (b.r - a.r) * t),
            static_cast<std::uint8_t>(a.g + (b.g - a.g) * t),
            static_cast<std::uint8_t>(a.b + (b.b - a.b) * t),
            255
        );
    }

    static sf::Color densityToColor(float density) {
        density = std::max(0.0f, std::min(1.0f, density));

        const sf::Color empty = AppConstants::BACKGROUND_COLOR;
        const sf::Color smoke = sf::Color(255, 255, 255);
        const float t = std::sqrt(density);

        return mixColor(empty, smoke, t);
    }

public:
    explicit FluidRenderer(const Fluid& fluid)
        : cells(sf::PrimitiveType::Triangles, fluid.grid_dimension.x * fluid.grid_dimension.y * 6),
          gridDimension(fluid.grid_dimension),
          cellSize(fluid.cell_size) {
        int vertexIndex = 0;
        for (int i = 0; i < gridDimension.x; ++i) {
            for (int j = 0; j < gridDimension.y; ++j) {
                const float left = i * cellSize + AppConstants::BORDER_SIZE;
                const float top = j * cellSize + AppConstants::BORDER_SIZE;
                const float right = left + cellSize;
                const float bottom = top + cellSize;

                cells[vertexIndex + 0].position = sf::Vector2f(left, top);
                cells[vertexIndex + 1].position = sf::Vector2f(right, top);
                cells[vertexIndex + 2].position = sf::Vector2f(right, bottom);

                cells[vertexIndex + 3].position = sf::Vector2f(left, top);
                cells[vertexIndex + 4].position = sf::Vector2f(right, bottom);
                cells[vertexIndex + 5].position = sf::Vector2f(left, bottom);
                vertexIndex += 6;
            }
        }
    }

    void updateFromFluid(const Fluid& fluid) {
        const int n = fluid.grid_dimension.y;
        int vertexIndex = 0;

        for (int i = 0; i < gridDimension.x; ++i) {
            for (int j = 0; j < gridDimension.y; ++j) {
                const sf::Color color = densityToColor(fluid.m[i * n + j]);

                cells[vertexIndex + 0].color = color;
                cells[vertexIndex + 1].color = color;
                cells[vertexIndex + 2].color = color;
                cells[vertexIndex + 3].color = color;
                cells[vertexIndex + 4].color = color;
                cells[vertexIndex + 5].color = color;
                vertexIndex += 6;
            }
        }
    }

    void draw(sf::RenderWindow& window) const {
        window.draw(cells);
    }
};
