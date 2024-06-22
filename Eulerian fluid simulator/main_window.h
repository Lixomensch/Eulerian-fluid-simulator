#pragma once

struct menu
{
	sf::RectangleShape rect;

	menu(sf::RenderWindow * window)
	{
		rect.setSize(sf::Vector2f(window->getSize().x, 50.f));
		rect.setFillColor(sf::Color::White);
		rect.setPosition(0,0);
	}
};

class main_window 
{
	sf::RenderWindow * window;
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
		menu menu(window);
		window->clear();
		window->draw(menu.rect);
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