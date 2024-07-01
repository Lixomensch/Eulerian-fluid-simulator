#pragma once
#include "fluid.h"

// Constants for button colors and border size
const float BORDER_SIZE = 50.f;
const float BUTTON_WIDTH = 100.f;
const float BUTTON_HEIGHT = 40.f;
const float BUTTON_MARGIN = 10.f;
const sf::Color BUTTON_COLORS[1] = { sf::Color::Blue };

struct Button
{
    sf::RectangleShape shape;
    sf::Text buttonText;

    Button(sf::Vector2f size, sf::Vector2f position, sf::Color color) {
        shape.setSize(size);
        shape.setPosition(position);
        shape.setFillColor(color);
    }

    bool isMouseOver(const sf::RenderWindow& window) const {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f buttonPos = shape.getPosition();
        sf::Vector2f buttonSize = shape.getSize();

        return mousePos.x >= buttonPos.x && mousePos.x <= buttonPos.x + buttonSize.x &&
            mousePos.y >= buttonPos.y && mousePos.y <= buttonPos.y + buttonSize.y;
    }
};

struct Menu {
    sf::RectangleShape rectUp;
    sf::RectangleShape rectLeft;
    sf::RectangleShape rectRight;
    sf::RectangleShape rectDown;
    std::vector<Button> buttons;

    Menu(sf::RenderWindow& window) {
        initializeRectangles(window);
        initializeButtons(window);
    }

    void initializeRectangles(sf::RenderWindow& window) {
        sf::Vector2u windowSize = window.getSize();

        rectUp.setSize(sf::Vector2f(windowSize.x, BORDER_SIZE));
        rectUp.setFillColor(sf::Color::White);
        rectUp.setPosition(0.f, 0.f);

        rectLeft.setSize(sf::Vector2f(BORDER_SIZE, windowSize.y));
        rectLeft.setFillColor(sf::Color::White);
        rectLeft.setPosition(0.f, 0.f);

        rectRight.setSize(sf::Vector2f(BORDER_SIZE, windowSize.y));
        rectRight.setFillColor(sf::Color::White);
        rectRight.setPosition(windowSize.x - BORDER_SIZE, 0.f);

        rectDown.setSize(sf::Vector2f(windowSize.x, BORDER_SIZE));
        rectDown.setFillColor(sf::Color::White);
        rectDown.setPosition(0.f, windowSize.y - BORDER_SIZE);
    }

    void initializeButtons(sf::RenderWindow& window) {
        sf::Vector2f buttonSize(BUTTON_WIDTH, BORDER_SIZE - BUTTON_MARGIN);
        sf::Vector2f buttonPosition(BORDER_SIZE, BORDER_SIZE / 2 - BUTTON_HEIGHT / 2);

        for (const auto& color : BUTTON_COLORS) {
            buttons.emplace_back(Button(buttonSize, buttonPosition, color));
            buttonPosition.x += BUTTON_WIDTH + BUTTON_MARGIN;
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(rectUp);
        window.draw(rectLeft);
        window.draw(rectRight);
        window.draw(rectDown);

        for (auto& btn : buttons) {
            window.draw(btn.shape);
            window.draw(btn.buttonText);
        }
    }
};

class DrawFluid {
public:
    sf::CircleShape mouse;
    Fluid fluid;
    float gravity;
    float dt;
    sf::Clock clock;
    sf::Vector2i mousePos;

    DrawFluid(int x, int y, int h) : fluid(x, y, h) {
        mouse.setRadius(50.f);
        mouse.setFillColor(sf::Color::White);
        gravity = 0;
        dt = 0;
    }

    bool isMouseOver(const sf::RenderWindow& window, const Menu& mainMenu) const {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        return mousePos.x > mainMenu.rectLeft.getPosition().x + mainMenu.rectLeft.getSize().x &&
            mousePos.x < mainMenu.rectRight.getPosition().x &&
            mousePos.y > mainMenu.rectUp.getPosition().y + mainMenu.rectUp.getSize().y &&
            mousePos.y < mainMenu.rectDown.getPosition().y;
    }

    void drawMouse(sf::RenderWindow& window) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        mouse.setPosition(static_cast<sf::Vector2f>(mousePos) - sf::Vector2f(mouse.getRadius(), mouse.getRadius()));
        window.draw(mouse);
    }

    void setMouse() {
        float r = mouse.getRadius();
        int x = mouse.getPosition().x;
        int y = mouse.getPosition().y;
        int n = fluid.grid_dimension.y;

        float vx = (x - mousePos.x) * 10;
        float vy = (y - mousePos.y) * 10;

        mousePos.x = x;
        mousePos.y = y;

        for (int i = 1; i < fluid.grid_dimension.x - 2; i++) {
            float posX = (i + 0.5f) * fluid.cell_size;
            for (int j = 1; j < fluid.grid_dimension.y - 2; j++) {
                fluid.solid[i * n + j] = 10.0f;

                float posY = (j + 0.5f) * fluid.cell_size;
                float dx = posX - x;
                float dy = posY - y;

                if (dx * dx + dy * dy < r * r) {
                    fluid.m[i * n + j] = 0.5f + 0.2f * std::sin(0.3f * dt);
                    fluid.u[i * n + j] = vx;
                    fluid.u[(i + 1) * n + j] = vx;
                    fluid.v[i * n + j] = vy;
                    fluid.v[i * n + j + 1] = vy;
                }
            }
        }
    }

    void simulate(float dt, int numIters) {
        fluid.integrate(dt, gravity);
        std::fill(fluid.pressure.begin(), fluid.pressure.end(), 0.0f);
        fluid.incompressibility(numIters, dt);
        fluid.advect_velocity(dt);
        fluid.move_smoke(dt);
    }

    void reset() {
        std::fill(fluid.pressure.begin(), fluid.pressure.end(), 0.0f);
        std::fill(fluid.u.begin(), fluid.u.end(), 0.0f);
        std::fill(fluid.v.begin(), fluid.v.end(), 0.0f);
        std::fill(fluid.solid.begin(), fluid.solid.end(), 0.0f);
        std::fill(fluid.m.begin(), fluid.m.end(), 0.0f);
    }

    void drawWindow(sf::RenderWindow& window, bool mousePress) {
        int n = fluid.grid_dimension.y;
        simulate(dt, 10);

        if (mousePress) {
            drawMouse(window);
            setMouse();
        }

        sf::RectangleShape cell(sf::Vector2f(fluid.cell_size, fluid.cell_size));
        for (int i = 0; i < fluid.grid_dimension.x; i++) {
            for (int j = 0; j < fluid.grid_dimension.y; j++) {
                float density = fluid.m[i * n + j];
                cell.setPosition(i * fluid.cell_size + BORDER_SIZE, j * fluid.cell_size + BORDER_SIZE);
                sf::Uint8 colorValue = static_cast<sf::Uint8>(density * 255);
                cell.setFillColor(sf::Color(colorValue, colorValue, colorValue));
                window.draw(cell);
            }
        }

        dt = clock.restart().asSeconds();
    }
};

class MainWindow {
    sf::RenderWindow window;
    Menu mainMenu;
    DrawFluid fluidWindow;
    bool drawMouse;

    void handleEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            for (auto& btn : mainMenu.buttons) {
                if (btn.isMouseOver(window)) {
                    fluidWindow.reset();
                }
            }

            if (fluidWindow.isMouseOver(window, mainMenu)) {
                drawMouse = true;
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                std::cout << mousePos.x << "," << mousePos.y << std::endl;
            }
        }
    }

    void render() {
        window.clear();
        mainMenu.draw(window);
        fluidWindow.drawWindow(window, drawMouse);
        window.display();
        drawMouse = false;
    }

public:
    MainWindow()
        : window(sf::VideoMode(1366, 738), "Euler Fluid Simulator"),
        mainMenu(window),
        fluidWindow(251, 125, 5),
        drawMouse(false)
    {}

    void run() {
        while (window.isOpen()) {
            handleEvents();
            render();
        }
    }
};


