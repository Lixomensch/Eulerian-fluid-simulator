#pragma once

#include <SFML/Graphics.hpp>

namespace AppConstants {
constexpr float BORDER_SIZE = 50.0f;
constexpr float BUTTON_WIDTH = 100.0f;
constexpr float BUTTON_HEIGHT = 40.0f;
constexpr float BUTTON_MARGIN = 10.0f;
constexpr sf::Color BACKGROUND_COLOR = sf::Color(6, 6, 8);
constexpr sf::Color PANEL_COLOR = sf::Color(20, 20, 24, 235);
constexpr sf::Color PANEL_EDGE_COLOR = sf::Color(58, 58, 66, 255);
constexpr sf::Color BUTTON_BASE_COLOR = sf::Color(42, 42, 48, 240);
constexpr sf::Color BUTTON_HOVER_COLOR = sf::Color(70, 70, 80, 245);
constexpr sf::Color BUTTON_ICON_COLOR = sf::Color(230, 230, 235, 245);
constexpr sf::Color TITLE_TEXT_COLOR = sf::Color(235, 235, 240, 255);
constexpr sf::Color BADGE_COLOR = sf::Color(36, 36, 42, 245);
constexpr sf::Color BADGE_TEXT_COLOR = sf::Color(228, 228, 232, 255);
constexpr sf::Color BRUSH_FILL_COLOR = sf::Color(255, 255, 255, 26);
constexpr sf::Color BRUSH_EDGE_COLOR = sf::Color(255, 255, 255, 220);
constexpr float DEFAULT_BRUSH_RADIUS = 50.0f;
constexpr float DEFAULT_FIXED_DT = 1.0f / 60.0f;
constexpr float DENSITY_DECAY_PER_SECOND = 0.45f;
constexpr int DEFAULT_SOLVER_ITERS = 10;
constexpr int PROJECTION_MAX_ITERS = 40;
constexpr float PROJECTION_RESIDUAL_EPSILON = 1.0e-3f;
constexpr bool ENABLE_PERF_LOGS = false;
constexpr int PERF_REPORT_EVERY_N_STEPS = 120;
} // namespace AppConstants
