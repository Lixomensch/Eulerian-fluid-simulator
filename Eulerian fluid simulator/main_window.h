#pragma once
#include "fluid.h"

struct button
{
    sf::RectangleShape shape;
    sf::Text buttonText;

    button(sf::Vector2f size, sf::Vector2f position, sf::Color color) {
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
const float BORDER_SIZE = 50.f;
struct menu {
    sf::RectangleShape rect_up;
    sf::RectangleShape rect_left;
    sf::RectangleShape rect_right;
    sf::RectangleShape rect_down;
    std::vector<button> buttons;

    // Constants for rectangle and button sizes

    const float BUTTON_WIDTH = 100.f;
    const float BUTTON_HEIGHT = 40.f;
    const float BUTTON_MARGIN = 10.f;
    const sf::Color BUTTON_COLORS[3] = { sf::Color::Blue, sf::Color::Green, sf::Color::Red };

    menu(sf::RenderWindow& window) {
        initialize_rectangles(window);
        initialize_buttons(window);
    }

    void initialize_rectangles(sf::RenderWindow& window) {
        sf::Vector2u windowSize = window.getSize();

        rect_up.setSize(sf::Vector2f(windowSize.x, BORDER_SIZE));
        rect_up.setFillColor(sf::Color::White);
        rect_up.setPosition(0.f, 0.f);

        rect_left.setSize(sf::Vector2f(BORDER_SIZE, windowSize.y));
        rect_left.setFillColor(sf::Color::White);
        rect_left.setPosition(0.f, 0.f);

        rect_right.setSize(sf::Vector2f(BORDER_SIZE, windowSize.y));
        rect_right.setFillColor(sf::Color::White);
        rect_right.setPosition(windowSize.x - BORDER_SIZE, 0.f);

        rect_down.setSize(sf::Vector2f(windowSize.x, BORDER_SIZE));
        rect_down.setFillColor(sf::Color::White);
        rect_down.setPosition(0.f, windowSize.y - BORDER_SIZE);
    }

    void initialize_buttons(sf::RenderWindow& window) {
        sf::Vector2f buttonSize(BUTTON_WIDTH, BORDER_SIZE - BUTTON_MARGIN);
        sf::Vector2f buttonPosition(BORDER_SIZE, BORDER_SIZE / 2 - BUTTON_HEIGHT / 2);

        for (const auto& color : BUTTON_COLORS) {
            buttons.emplace_back(button(buttonSize, buttonPosition, color));
            buttonPosition.x += BUTTON_WIDTH + BUTTON_MARGIN;
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(rect_up);
        window.draw(rect_left);
        window.draw(rect_right);
        window.draw(rect_down);

        for (auto& btn : buttons) {
            window.draw(btn.shape);
            window.draw(btn.buttonText);
        }
    }
};

class draw_fluid {
public:
    sf::CircleShape mouse;
    fluid fluid;
    float gravity;
    float dt;
    sf::Clock clock;
    sf::Vector2i mouse_pos;

    draw_fluid(int x, int y, int h) : fluid(x, y, h) {
        mouse.setRadius(50.f);
        mouse.setFillColor(sf::Color::White);
        gravity = 0;
        dt = 0;
    }

    bool isMouseOver(const sf::RenderWindow& window, const menu& main_menu) const {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        return mousePos.x > main_menu.rect_left.getPosition().x + main_menu.rect_left.getSize().x &&
            mousePos.x < main_menu.rect_right.getPosition().x &&
            mousePos.y > main_menu.rect_up.getPosition().y + main_menu.rect_up.getSize().y &&
            mousePos.y < main_menu.rect_down.getPosition().y;
    }

    void draw_mouse(sf::RenderWindow& window) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        mouse.setPosition(static_cast<sf::Vector2f>(mousePos) - sf::Vector2f(mouse.getRadius(), mouse.getRadius()));
        window.draw(mouse);
    }

    void set_mouse() {
        float r = mouse.getRadius();
        int x = mouse.getPosition().x;
        int y = mouse.getPosition().y;
        int n = fluid.grid_dimension.y;

        float vx = (x - mouse_pos.x)*10;
        float vy = (y - mouse_pos.y)*10;

        mouse_pos.x = x;
        mouse_pos.y = y;

        for (int i = 1; i < fluid.grid_dimension.x - 2; i++) {
            float posX = (i + 0.5f) * fluid.tam_cell;
            for (int j = 1; j < fluid.grid_dimension.y - 2; j++) {
                fluid.solid[i * n + j] = 1.0f;

                float posY = (j + 0.5f) * fluid.tam_cell;
                float dx = posX - x;
                float dy = posY - y;

                if (dx * dx + dy * dy < r * r) {
                    fluid.m[i * n + j] = 0.5f + 0.5f * std::sin(0.1f * dt);
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
        //fluid.boundary_velocities();
        fluid.advectVel(dt);
        fluid.move_smoke(dt);
    }

    void draw_window(sf::RenderWindow& window, bool mouse_press) {
        int n = fluid.grid_dimension.y;
        simulate(dt, 10);

        if (mouse_press) {
            draw_mouse(window);
            set_mouse();
        }

        sf::RectangleShape cell(sf::Vector2f(fluid.tam_cell, fluid.tam_cell));
        for (int i = 0; i < fluid.grid_dimension.x; i++) {
            for (int j = 0; j < fluid.grid_dimension.y; j++) {
                float density = fluid.m[i * n + j];
                cell.setPosition(i * fluid.tam_cell + BORDER_SIZE, j * fluid.tam_cell + BORDER_SIZE);
                sf::Uint8 colorValue = static_cast<sf::Uint8>(density * 255);
                cell.setFillColor(sf::Color(colorValue, colorValue, colorValue));
                window.draw(cell);
            }
        }

        dt = clock.restart().asSeconds();
    }
};


class main_window {
    sf::RenderWindow window;
    menu mainMenu;
    draw_fluid fluid_window;
    bool draw_mouse;

    void events() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            for (auto& btn : mainMenu.buttons) {
                if (btn.isMouseOver(window)) {
                    std::cout << "Botão clicado!" << std::endl;
                }
            }

            if (fluid_window.isMouseOver(window, mainMenu)) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                draw_mouse = true;
                std::cout << mousePos.x << "," << mousePos.y << std::endl;
            }
        }
    }

    void draw() {
        window.clear();
        mainMenu.draw(window);
        fluid_window.draw_window(window, draw_mouse);
        window.display();
        draw_mouse = false;
    }

public:
    main_window()
        : window(sf::VideoMode(1366, 738), "Euler fluid simulator"),
        mainMenu(window),
        fluid_window(250, 126, 5),
        draw_mouse(false)
    {}

    void run_window() {
        while (window.isOpen()) {
            events();
            draw();
        }
    }
};

