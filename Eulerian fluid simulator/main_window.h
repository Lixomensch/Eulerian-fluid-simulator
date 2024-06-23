#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

struct button
{
    sf::RectangleShape shape;
    sf::Text buttonText;

    button(sf::Vector2f size, sf::Vector2f position, sf::Color color, sf::Font& font, const std::string& text) {
        shape.setSize(size);
        shape.setPosition(position);
        shape.setFillColor(color);

        buttonText.setString(text);
        buttonText.setFont(font);
        buttonText.setCharacterSize(24);

        // Centralizar o texto no bot�o
        sf::FloatRect textRect = buttonText.getLocalBounds();
        buttonText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        buttonText.setPosition(position.x + size.x / 2.0f, position.y + size.y / 2.0f);
    }
};

struct menu
{
    sf::RectangleShape rect;
    std::vector<button> buttons;

    menu(sf::RenderWindow& window, sf::Font& font) {
        rect.setSize(sf::Vector2f(window.getSize().x, 50.f)); // Ajuste conforme necess�rio
        rect.setFillColor(sf::Color::White);
        rect.setPosition(0.f, 0.f);

        // Posicionamento do bot�o relativo ao ret�ngulo do menu
        sf::Vector2f buttonSize(100.f, rect.getSize().y - 10.f);
        sf::Vector2f buttonPosition(10.f, 5.f); // Ajuste conforme necess�rio
        buttons.emplace_back(buttonSize, buttonPosition, sf::Color::Blue, font, "tank");
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
    sf::Font font;

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
    }

    void draw() {
        menu mainMenu(window, font);
        window.clear();
        mainMenu.draw(window);
        window.display();
    }

public:
    main_window()
        : window(sf::VideoMode(800, 600), "Euler fluid simulator")
    {
        if (!font.loadFromFile("./ARIAL.ttf")) {
            throw std::runtime_error("Failed to load font");
        }

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
