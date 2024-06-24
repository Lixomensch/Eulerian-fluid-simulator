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

struct menu
{
    sf::RectangleShape rect;
    std::vector<button> buttons;

    menu(sf::RenderWindow& window) {
        rect.setSize(sf::Vector2f(window.getSize().x, 50.f));
        rect.setFillColor(sf::Color::White);
        rect.setPosition(0.f, 0.f);


        sf::Vector2f buttonSize(100.f, rect.getSize().y - 10.f);
        sf::Vector2f buttonPosition(10.f, 5.f);

        buttons.emplace_back(button(buttonSize, buttonPosition, sf::Color::Blue));
        buttonPosition.x += 110.f;
        buttons.emplace_back(button(buttonSize, buttonPosition, sf::Color::Green));
        buttonPosition.x += 110.f;
        buttons.emplace_back(button(buttonSize, buttonPosition, sf::Color::Red));
    }

    void draw(sf::RenderWindow& window) {
        window.draw(rect);
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
        : window(sf::VideoMode(1366, 768), "Euler fluid simulator"), mainMenu(window)
    {
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
