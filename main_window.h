#pragma once
#include <SFML/Graphics.hpp>

#include <algorithm>
#include <array>
#include <cstdint>
#include <cmath>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "app_constants.h"
#include "fluid_renderer.h"
#include "fluid_solver.h"

struct Button
{
    sf::RectangleShape shape;
    sf::CircleShape iconRing;
    sf::ConvexShape iconArrow;
    sf::Color baseColor;
    sf::Color hoverColor;
    sf::Color currentColor;

    static sf::Color blendColor(const sf::Color& a, const sf::Color& b, float t) {
        t = std::clamp(t, 0.0f, 1.0f);
        return sf::Color(
            static_cast<std::uint8_t>(a.r + (b.r - a.r) * t),
            static_cast<std::uint8_t>(a.g + (b.g - a.g) * t),
            static_cast<std::uint8_t>(a.b + (b.b - a.b) * t),
            static_cast<std::uint8_t>(a.a + (b.a - a.a) * t)
        );
    }

    void configureIcon() {
        const sf::Vector2f pos = shape.getPosition();
        const sf::Vector2f size = shape.getSize();
        const sf::Vector2f center(pos.x + size.x * 0.5f, pos.y + size.y * 0.5f);
        const float radius = size.y * 0.24f;

        iconRing.setRadius(radius);
        iconRing.setPointCount(36);
        iconRing.setOrigin(sf::Vector2f(radius, radius));
        iconRing.setPosition(center);
        iconRing.setFillColor(sf::Color::Transparent);
        iconRing.setOutlineThickness(2.0f);
        iconRing.setOutlineColor(AppConstants::BUTTON_ICON_COLOR);

        iconArrow.setPointCount(3);
        iconArrow.setPoint(0, sf::Vector2f(center.x + radius * 0.95f, center.y));
        iconArrow.setPoint(1, sf::Vector2f(center.x + radius * 0.35f, center.y - radius * 0.55f));
        iconArrow.setPoint(2, sf::Vector2f(center.x + radius * 0.35f, center.y + radius * 0.55f));
        iconArrow.setFillColor(AppConstants::BUTTON_ICON_COLOR);
    }

    Button(sf::Vector2f size, sf::Vector2f position, sf::Color base, sf::Color hover)
        : baseColor(base), hoverColor(hover), currentColor(base) {
        shape.setSize(size);
        shape.setPosition(position);
        shape.setFillColor(currentColor);
        shape.setOutlineThickness(1.0f);
        shape.setOutlineColor(AppConstants::PANEL_EDGE_COLOR);
        configureIcon();
    }

    bool isMouseOver(const sf::RenderWindow& window) const {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f buttonPos = shape.getPosition();
        sf::Vector2f buttonSize = shape.getSize();

        return mousePos.x >= buttonPos.x && mousePos.x <= buttonPos.x + buttonSize.x &&
            mousePos.y >= buttonPos.y && mousePos.y <= buttonPos.y + buttonSize.y;
    }

    void updateVisualState(const sf::RenderWindow& window, float dt) {
        const sf::Color target = isMouseOver(window) ? hoverColor : baseColor;
        const float blend = std::clamp(dt * 9.0f, 0.0f, 1.0f);
        currentColor = blendColor(currentColor, target, blend);
        shape.setFillColor(currentColor);

        const sf::Color iconColor = blendColor(
            AppConstants::BUTTON_ICON_COLOR,
            sf::Color::White,
            isMouseOver(window) ? 0.45f : 0.0f
        );
        iconRing.setOutlineColor(iconColor);
        iconArrow.setFillColor(iconColor);
    }

    void draw(sf::RenderWindow& window) const {
        window.draw(shape);
        window.draw(iconRing);
        window.draw(iconArrow);
    }
};

struct Menu {
    sf::Font uiFont;
    bool hasFont;
    std::optional<sf::Text> titleText;
    sf::RectangleShape modeBadge;
    std::optional<sf::Text> modeBadgeText;
    sf::RectangleShape rectUp;
    sf::RectangleShape rectLeft;
    sf::RectangleShape rectRight;
    sf::RectangleShape rectDown;
    std::vector<Button> buttons;

    void initializeTypography(const sf::RenderWindow& window, std::string_view solverMode) {
        hasFont = false;

        const std::array<const char*, 5> fontCandidates = {
            "/usr/share/fonts/noto/NotoSans-Regular.ttf",
            "/usr/share/fonts/TTF/DejaVuSans.ttf",
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
            "/usr/share/fonts/TTF/LiberationSans-Regular.ttf",
            "/usr/share/fonts/liberation/LiberationSans-Regular.ttf"
        };

        for (const char* path : fontCandidates) {
            if (std::filesystem::exists(path) && uiFont.openFromFile(path)) {
                hasFont = true;
                break;
            }
        }

        modeBadge.setSize(sf::Vector2f(108.0f, 30.0f));
        modeBadge.setFillColor(AppConstants::BADGE_COLOR);
        modeBadge.setOutlineThickness(1.0f);
        modeBadge.setOutlineColor(AppConstants::PANEL_EDGE_COLOR);
        modeBadge.setPosition(sf::Vector2f(window.getSize().x - AppConstants::BORDER_SIZE - 120.0f, 10.0f));

        if (!hasFont) {
            return;
        }

        titleText.emplace(uiFont, "Eulerian Fluid Simulator", 21);
        titleText->setFillColor(AppConstants::TITLE_TEXT_COLOR);
        titleText->setStyle(sf::Text::Bold);
        titleText->setPosition(sf::Vector2f(AppConstants::BORDER_SIZE + 125.0f, 10.0f));

        modeBadgeText.emplace(uiFont, std::string(solverMode), 16);
        modeBadgeText->setFillColor(AppConstants::BADGE_TEXT_COLOR);
        modeBadgeText->setStyle(sf::Text::Bold);
        modeBadgeText->setPosition(sf::Vector2f(window.getSize().x - AppConstants::BORDER_SIZE - 96.0f, 15.0f));
    }

    Menu(sf::RenderWindow& window) {
        initializeRectangles(window);
        initializeButtons();
        initializeTypography(window, "RBGS");
    }

    void initializeRectangles(sf::RenderWindow& window) {
        sf::Vector2u windowSize = window.getSize();

        rectUp.setSize(sf::Vector2f(windowSize.x, AppConstants::BORDER_SIZE));
        rectUp.setFillColor(AppConstants::PANEL_COLOR);
        rectUp.setOutlineThickness(1.0f);
        rectUp.setOutlineColor(AppConstants::PANEL_EDGE_COLOR);
        rectUp.setPosition(sf::Vector2f(0.0f, 0.0f));

        rectLeft.setSize(sf::Vector2f(AppConstants::BORDER_SIZE, windowSize.y));
        rectLeft.setFillColor(AppConstants::PANEL_COLOR);
        rectLeft.setOutlineThickness(1.0f);
        rectLeft.setOutlineColor(AppConstants::PANEL_EDGE_COLOR);
        rectLeft.setPosition(sf::Vector2f(0.0f, 0.0f));

        rectRight.setSize(sf::Vector2f(AppConstants::BORDER_SIZE, windowSize.y));
        rectRight.setFillColor(AppConstants::PANEL_COLOR);
        rectRight.setOutlineThickness(1.0f);
        rectRight.setOutlineColor(AppConstants::PANEL_EDGE_COLOR);
        rectRight.setPosition(sf::Vector2f(windowSize.x - AppConstants::BORDER_SIZE, 0.0f));

        rectDown.setSize(sf::Vector2f(windowSize.x, AppConstants::BORDER_SIZE));
        rectDown.setFillColor(AppConstants::PANEL_COLOR);
        rectDown.setOutlineThickness(1.0f);
        rectDown.setOutlineColor(AppConstants::PANEL_EDGE_COLOR);
        rectDown.setPosition(sf::Vector2f(0.0f, windowSize.y - AppConstants::BORDER_SIZE));
    }

    void initializeButtons() {
        sf::Vector2f buttonSize(AppConstants::BUTTON_WIDTH, AppConstants::BORDER_SIZE - AppConstants::BUTTON_MARGIN);
        sf::Vector2f buttonPosition(
            AppConstants::BORDER_SIZE,
            AppConstants::BORDER_SIZE / 2 - AppConstants::BUTTON_HEIGHT / 2
        );

        buttons.emplace_back(Button(
            buttonSize,
            buttonPosition,
            AppConstants::BUTTON_BASE_COLOR,
            AppConstants::BUTTON_HOVER_COLOR
        ));
    }

    void update(const sf::RenderWindow& window, float dt) {
        for (auto& btn : buttons) {
            btn.updateVisualState(window, dt);
        }

        const sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        const sf::Vector2f badgePos = modeBadge.getPosition();
        const sf::Vector2f badgeSize = modeBadge.getSize();
        const bool hovered =
            mousePos.x >= badgePos.x && mousePos.x <= badgePos.x + badgeSize.x &&
            mousePos.y >= badgePos.y && mousePos.y <= badgePos.y + badgeSize.y;
        modeBadge.setFillColor(hovered ? AppConstants::BUTTON_HOVER_COLOR : AppConstants::BADGE_COLOR);
    }

    bool isModeBadgeOver(const sf::Vector2i& mousePos) const {
        const sf::Vector2f badgePos = modeBadge.getPosition();
        const sf::Vector2f badgeSize = modeBadge.getSize();
        return
            mousePos.x >= badgePos.x && mousePos.x <= badgePos.x + badgeSize.x &&
            mousePos.y >= badgePos.y && mousePos.y <= badgePos.y + badgeSize.y;
    }

    void setModeLabel(std::string_view modeLabel) {
        if (modeBadgeText.has_value()) {
            modeBadgeText->setString(std::string(modeLabel));
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(rectUp);
        window.draw(rectLeft);
        window.draw(rectRight);
        window.draw(rectDown);

        window.draw(modeBadge);
        if (titleText.has_value()) {
            window.draw(*titleText);
        }
        if (modeBadgeText.has_value()) {
            window.draw(*modeBadgeText);
        }

        for (auto& btn : buttons) {
            btn.draw(window);
        }
    }
};

class MainWindow {
    sf::RenderWindow window;
    Menu mainMenu;
    FluidSolver fluidSolver;
    FluidRenderer fluidRenderer;
    sf::CircleShape mouseBrush;
    sf::Clock frameClock;
    sf::Clock pulseClock;
    sf::Clock uiDeltaClock;
    float accumulator;
    const float fixedDt;
    bool brushActive;
    float brushVisualIntensity;
    sf::Vector2i currentMousePosition;

    void initializeBrush() {
        mouseBrush.setRadius(fluidSolver.getBrushRadius());
        mouseBrush.setFillColor(sf::Color(
            AppConstants::BRUSH_FILL_COLOR.r,
            AppConstants::BRUSH_FILL_COLOR.g,
            AppConstants::BRUSH_FILL_COLOR.b,
            0
        ));
        mouseBrush.setOutlineThickness(2.0f);
        mouseBrush.setOutlineColor(sf::Color(
            AppConstants::BRUSH_EDGE_COLOR.r,
            AppConstants::BRUSH_EDGE_COLOR.g,
            AppConstants::BRUSH_EDGE_COLOR.b,
            0
        ));
    }

    void updateBrushVisual(float dt) {
        const float target = brushActive ? 1.0f : 0.0f;
        const float blend = std::clamp(dt * 8.0f, 0.0f, 1.0f);
        brushVisualIntensity += (target - brushVisualIntensity) * blend;
        brushVisualIntensity = std::clamp(brushVisualIntensity, 0.0f, 1.0f);
    }

    bool isSimulationArea(const sf::Vector2i& mousePos) const {
        return mousePos.x > mainMenu.rectLeft.getPosition().x + mainMenu.rectLeft.getSize().x &&
            mousePos.x < mainMenu.rectRight.getPosition().x &&
            mousePos.y > mainMenu.rectUp.getPosition().y + mainMenu.rectUp.getSize().y &&
            mousePos.y < mainMenu.rectDown.getPosition().y;
    }

    void drawMouseOverlay() {
        if (brushVisualIntensity <= 0.01f) {
            return;
        }

        sf::Vector2i currentMousePos = sf::Mouse::getPosition(window);
        const float phase = pulseClock.getElapsedTime().asSeconds() * 5.0f;
        const float glow = 0.5f + 0.5f * std::sin(phase);
        const std::uint8_t edgeAlpha = static_cast<std::uint8_t>((95.0f + glow * 110.0f) * brushVisualIntensity);
        const std::uint8_t fillAlpha = static_cast<std::uint8_t>(52.0f * brushVisualIntensity);

        mouseBrush.setFillColor(sf::Color(
            AppConstants::BRUSH_FILL_COLOR.r,
            AppConstants::BRUSH_FILL_COLOR.g,
            AppConstants::BRUSH_FILL_COLOR.b,
            fillAlpha
        ));
        mouseBrush.setOutlineColor(sf::Color(
            AppConstants::BRUSH_EDGE_COLOR.r,
            AppConstants::BRUSH_EDGE_COLOR.g,
            AppConstants::BRUSH_EDGE_COLOR.b,
            edgeAlpha
        ));
        mouseBrush.setPosition(
            static_cast<sf::Vector2f>(currentMousePos)
            - sf::Vector2f(mouseBrush.getRadius(), mouseBrush.getRadius())
        );
        window.draw(mouseBrush);
    }

    void handleEvents() {
        brushActive = false;
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mousePressed->button == sf::Mouse::Button::Left && mainMenu.isModeBadgeOver(mousePressed->position)) {
                    fluidSolver.toggleSolverMode();
                    mainMenu.setModeLabel(fluidSolver.solverModeLabel());
                }
            }
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
            for (auto& btn : mainMenu.buttons) {
                if (btn.isMouseOver(window)) {
                    fluidSolver.reset();
                }
            }

            currentMousePosition = sf::Mouse::getPosition(window);
            if (isSimulationArea(currentMousePosition)) {
                brushActive = true;
            }
        }
    }

    void updateSimulation(float frameDt) {
        accumulator += frameDt;

        while (accumulator >= fixedDt) {
            fluidSolver.step(fixedDt);
            if (brushActive) {
                fluidSolver.applyBrush(currentMousePosition);
            }
            accumulator -= fixedDt;
        }
    }

    void render() {
        const float uiDt = uiDeltaClock.restart().asSeconds();
        updateBrushVisual(uiDt);
        mainMenu.update(window, uiDt);
        window.clear(AppConstants::BACKGROUND_COLOR);
        mainMenu.draw(window);
        fluidRenderer.updateFromFluid(fluidSolver.fluid());
        fluidRenderer.draw(window);
        drawMouseOverlay();
        window.display();
    }

public:
    MainWindow()
        : window(sf::VideoMode(sf::Vector2u(1366, 738)), "Euler Fluid Simulator"),
        mainMenu(window),
        fluidSolver(251, 125, 5),
        fluidRenderer(fluidSolver.fluid()),
        accumulator(0.0f),
        fixedDt(AppConstants::DEFAULT_FIXED_DT),
        brushActive(false),
        brushVisualIntensity(0.0f),
        currentMousePosition(0, 0)
    {
        initializeBrush();
        mainMenu.setModeLabel(fluidSolver.solverModeLabel());
    }

    void run() {
        while (window.isOpen()) {
            handleEvents();
            updateSimulation(frameClock.restart().asSeconds());
            render();
        }
    }
};


