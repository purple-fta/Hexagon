#include <SFML/Graphics.hpp>
#include <iostream>
#include "Menu.h"
#include "Game.h"


int main() {
    auto window = sf::RenderWindow(
        sf::VideoMode({1280, 720}), "Hexagon",
        sf::Style::Default, sf::State::Windowed,
        sf::ContextSettings{.antiAliasingLevel = 8}
    );
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.openFromFile("assets/JetBrainsMono-SemiBold.ttf")) {
        return -1;
    }

    sf::Clock clock;
    float dt;

    // Создание кнопок
    std::vector<sf::Color> colors = {
        Palette::Sky,
        Palette::Green,
        Palette::Yellow,
        Palette::Red
    };
    std::vector<std::string> labels = {
        " Player\n   vs\nComputer",
        "Player\n  vs\nPlayer",
        "Load\nGame",
        "Exit"
    };
    Menu startMenu(font, window, colors, labels, "Hexagon", Palette::Sky);

    std::unique_ptr<Game> game;

    while (window.isOpen()) {
        dt = clock.restart().asSeconds();

        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            startMenu.handleEvent(*event);
        }

        startMenu.update(dt);

        startMenu.draw();

        if (startMenu.getSelected() != -1) {
            int selected = startMenu.getSelected();
            startMenu.resetSelected();
            if (selected == 0) {
                game = std::make_unique<Game>(window, true, font);
                game->run(false);
            } else if (selected == 1 ) {
                game = std::make_unique<Game>(window, false, font);
                game->run(false);
            } else if (selected == 2) {
                game = std::make_unique<Game>(window, false, font);
                game->run(true);
            }
             else if (selected == 3) {
                window.close();
            }
        }
    }
}
