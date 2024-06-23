#pragma once

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

		sf::FloatRect textRect = buttonText.getLocalBounds();
		buttonText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
		buttonText.setPosition(position.x + size.x / 2.0f, position.y + size.y / 2.0f);
	}
};

struct menu
{
	sf::RectangleShape rect;
	std::vector<button> buttons;

	menu(sf::RenderWindow* window, sf::Font font)
	{
		rect.setSize(sf::Vector2f(window->getSize().x, 50.f));
		rect.setFillColor(sf::Color::White);
		rect.setPosition(0,0);

		buttons.emplace_back(sf::Vector2f(50.f, rect.getSize().y - 5), sf::Vector2f(0, 0), sf::Color::Blue, font, "tank");

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
	sf::RenderWindow* window;
	sf::Font* font;

	sf::Vector2i pos_mouse;
	sf::Vector2f cod_mouse;

	std::vector<std::string> options;
	std::vector<sf::Vector2f> coords;
	std::vector<sf::Text> texts;
	std::vector<std::size_t> sizes;

	void events() {
		sf::Event event;
		while (window->pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window->close();
		}
	}

	void draw() {
		menu mainMenu(window, *font);
		window->clear();
		mainMenu.draw(*window);
		window->display();
	}
public:

	main_window() {
		window = new sf::RenderWindow(sf::VideoMode(800, 600), "Euler fluid simulator");
		font = new sf::Font;
		font->loadFromFile("Roboto-Black.ttf");
		pos_mouse = { 0,0 };
		cod_mouse = { 0,0 };

	}

	void run_window() {
		while (window->isOpen())
		{
			events();
			draw();
		}
	}

	~main_window() {
		delete window;
		delete font;
	}
};