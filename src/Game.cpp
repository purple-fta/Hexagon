#include <SFML/Graphics.hpp>
#include <iostream>

#include "Game.h"
#include "pallete.h"
#include "Serialization.h"

Game::Game(sf::RenderWindow& window, bool singleGame, sf::Font& font) : window(window), score(font) {
    this->font = font;

    std::vector<sf::Color> colors = {
        Palette::Green,
        Palette::Sky,
        Palette::Yellow
    };
    std::vector<std::string> labels = {
        "Save\nGame",
        "Load\nGame",
        "Exit\n To \nMenu"
    };
    escMenu = std::make_unique<Menu>(font, window, colors, labels, "Hexagon", Palette::Yellow);

    board = std::make_unique<Board>(window, singleGame);

    this->singleGame = singleGame;
}

void Game::processEvents() {
    while (auto event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        } else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->scancode == sf::Keyboard::Scan::Escape && !board->isGameOver) {
                window.clear(Palette::Background);
                escMenuActive = !escMenuActive;
            }
        }

        if (escMenuActive) {
            escMenu->handleEvent(*event);
        } else {
            if (!board->isGameOver) {
                board->handleEvent(*event);
            } else {
                if (resultMenu != nullptr) {
                    resultMenu->handleEvent(*event);
                }
            }
        }
    }
}

void Game::run(bool _loadGame) {
    float showResultsDelay = 0;

    if (_loadGame) {
        this->loadGame();
    }

    window.clear(Palette::Background);

    score.draw(window);

    while (window.isOpen()) {
        dt = clock.restart().asSeconds();

        processEvents();

        if (escMenuActive) {
            escMenu->update(dt);
            escMenu->draw();

            if (escMenu->getSelected() != -1) {
                int selected = escMenu->getSelected();
                escMenu->resetSelected();
                switch (selected) {
                    case 0:
                        save(*board);
                        escMenuActive = false;
                        window.clear(Palette::Background);
                        break;
                    case 1:
                        loadGame();
                        break;
                    case 2:
                        return;
                        break;
                }
            }
        } else if (!board->isGameOver) {
            board->update(dt);
            board->draw();

            if (oldIsPlayer1Turn != board->isPlayer1Turn) {
                score.change();
                oldIsPlayer1Turn = board->isPlayer1Turn;
            }
            score.update(dt);
            score.draw(window);

            score.setScore(
                board->getCellsWithState(CellState::Player1).size(),
                board->getCellsWithState(CellState::Player2).size()
            );
        } else if (showResultsDelay > 1.5) {
            if (resultMenu == nullptr) {
                initResulltMenu(font, board->isPlayer1Win);
            }
            showResults();

            if (resultMenu->getSelected() != -1) {
                int selected = resultMenu->getSelected();
                resultMenu->resetSelected();
                switch (selected) {
                    case 0:
                        score = Score(font);
                        window.clear(Palette::Background);
                        board = std::make_unique<Board>(window, singleGame);
                        isPlayer1Turn = true;
                        oldIsPlayer1Turn = true;
                        resultMenu = nullptr;
                        score.draw(window);
                        board->update(10);
                        board->draw();
                        escMenuActive = false;
                        showResultsDelay = 0;
                        break;
                    case 1:
                        loadGame();
                        break;
                    case 2:
                        return;
                        break;
                }
            }       
        } else {
            showResultsDelay += dt;
            board->update(dt);
            board->draw();
        }

        window.display();
    }

}

void Game::showResults() {
    resultMenu->update(dt);
    resultMenu->draw();
}

void Game::initResulltMenu(sf::Font& font, bool isPlayer1Win) {
    std::cout << isPlayer1Win << std::endl;

    std::vector<sf::Color> colors = {
        Palette::Red,
        Palette::Sky,
        Palette::Yellow
    };
    std::vector<std::string> labels = {
        "Reload",
        "Load\nGame",
        "Exit\n To \nMenu"
    };

    sf::Color titleColor = isPlayer1Win ? Palette::Green : Palette::Red;
    std::string titleLabel = isPlayer1Win ? "Green Wins!" : "Red Wins!";

    resultMenu = std::make_unique<Menu>(font, window, colors, labels, titleLabel, titleColor);

}

void Game::loadGame() {
    std::cout << "Load game" << std::endl;
    score = Score(font);
    window.clear(Palette::Background);
    board = std::make_unique<Board>(load(window));
    isPlayer1Turn = board->isPlayer1Turn;
    oldIsPlayer1Turn = isPlayer1Turn;
    resultMenu = nullptr;
    escMenuActive = false;
    score.setScore(
        board->getCellsWithState(CellState::Player1).size(),
        board->getCellsWithState(CellState::Player2).size()
    );
    if (!board->isPlayer1Turn) {
        score.change();
    }
    score.draw(window);
}
