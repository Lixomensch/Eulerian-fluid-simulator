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

    bool isMouseOver(sf::RenderWindow& window) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f buttonPos = shape.getPosition();
        sf::Vector2f buttonSize = shape.getSize();

        return mousePos.x >= buttonPos.x && mousePos.x <= buttonPos.x + buttonSize.x &&
            mousePos.y >= buttonPos.y && mousePos.y <= buttonPos.y + buttonSize.y;
    }
};

struct menu {
    sf::RectangleShape rect_up;
    sf::RectangleShape rect_left;
    sf::RectangleShape rect_right;
    sf::RectangleShape rect_down;
    std::vector<button> buttons;

    // Constants for rectangle and button sizes
    const float BORDER_SIZE = 50.f;
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
        sf::Vector2f buttonPosition(BUTTON_MARGIN, BUTTON_MARGIN / 2);

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

class main_window
{
    sf::RenderWindow window;
    menu mainMenu;

    sf::Vector2i pos_mouse;
    sf::Vector2f cod_mouse;

    std::vector<std::string> options;
    std::vector<sf::Vector2f> coords;
    std::vector<sf::Text> texts;
    std::vector<std::size_t> sizes;

    void events() {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            for (auto& btn : mainMenu.buttons) {
                if (btn.isMouseOver(window)) {

                    std::cout << "Botao clicado!" << std::endl;
                }
            }
        }
    }

    void draw() {
        window.clear();
        mainMenu.draw(window);
        window.display();
    }

public:
    main_window()
        : window(sf::VideoMode(1366, 738), "Euler fluid simulator"), mainMenu(window)
    {
        // 30 pixels were subtracted from the y-axis to compensate for the standard Windows title bar
        pos_mouse = { 0,0 };
        cod_mouse = { 0,0 };
    }

    void run_window() {
        while (window.isOpen())
        {
            events();
            draw();
        }
    }
};
