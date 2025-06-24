#pragma once

#include <SFML/Graphics.hpp>
#include "Menu.h"
#include "Board.h"
#include "pallete.h"

class Score {
public:
    Score(sf::Font& font) : redScore(font, "3", 56), greenScore(font, "3", 56) {
        initShapes();
        centerText();

        redScore.setFillColor(Palette::p2Color);
        greenScore.setFillColor(Palette::p1Color);

        greenHex.setFillColor(greenSelectedColor);
    };

    void setScore(int score_1, int score_2) {
        if (score_1 != score_green) {
            greenScore.setString(std::to_string(score_1));
        } 
        if (score_2 != score_red) {
            redScore.setString(std::to_string(score_2));
        }
        centerText();
    };
    
    void draw(sf::RenderWindow& window) {
        window.draw(redHex);
        window.draw(greenHex);
        window.draw(redScore);
        window.draw(greenScore);
    };

    void change() {
        isGreenSelected = !isGreenSelected;
        animationTime = 0.0f;
    }

    void update(float dt) {
        animationTime += dt;
        float t = animationTime / 0.2f;
        if (t > 1.0f) {
            t = 1.0f;
        }

        sf::Color greenOld = isGreenSelected ? Palette::Background : redSelectedColor;
        sf::Color redOld = isGreenSelected ? redSelectedColor : Palette::Background;

        sf::Color greenTarget = isGreenSelected ? greenSelectedColor : Palette::Background;
        sf::Color redTarget = isGreenSelected ? Palette::Background : redSelectedColor;
    
        greenHex.setFillColor(sf::Color(
            (int)((1 - t) * greenOld.r + t * greenTarget.r),
            (int)((1 - t) * greenOld.g + t * greenTarget.g),
            (int)((1 - t) * greenOld.b + t * greenTarget.b)
        ));
        redHex.setFillColor(sf::Color(
            (int)((1 - t) * redOld.r + t * redTarget.r),
            (int)((1 - t) * redOld.g + t * redTarget.g),
            (int)((1 - t) * redOld.b + t * redTarget.b)
        ));
    }

private:
    int score_green = 3;
    int score_red = 3;

    float animationTime = 1.0f;

    bool isGreenSelected = true;

    sf::Color greenSelectedColor = sf::Color(70, 87, 79);
    sf::Color redSelectedColor = sf::Color(113, 50, 60);

    sf::ConvexShape redHex;
    sf::ConvexShape greenHex;

    sf::Text redScore;
    sf::Text greenScore;

    int redHexX = 1280-200;
    int redHexY = 360;
    int greenHexX = 200;
    int greenHexY = 360;

    void initShapes() {
        redHex.setPointCount(6);
        greenHex.setPointCount(6);

        for (int i = 0; i < 6; ++i) {
            float angle = i * 2 * 3.14159f / 6;
            redHex.setPoint(i,   {redHexX   + 80 * static_cast<float>(cos(angle)), redHexY + 80 * static_cast<float>(sin(angle))});
            greenHex.setPoint(i, {greenHexX   + 80 * static_cast<float>(cos(angle)), greenHexY + 80 * static_cast<float>(sin(angle))});
        }

        redHex.setOutlineThickness(3);
        greenHex.setOutlineThickness(3);
        redHex.setOutlineColor(Palette::p2Color);
        greenHex.setOutlineColor(Palette::p1Color);
        redHex.setFillColor(Palette::Background);
        greenHex.setFillColor(Palette::Background);
    }

    void centerText() {
        redScore.setPosition({
            redHexX - redScore.getGlobalBounds().size.x / 2 - 3,
            redHexY - redScore.getGlobalBounds().size.y / 2 - 15
        });
        greenScore.setPosition({
            greenHexX - greenScore.getGlobalBounds().size.x / 2 - 3,
            greenHexY - greenScore.getGlobalBounds().size.y / 2 - 15
        });
    }
};

class Game {

private:
    sf::Font font;

    Score score;

    sf::RenderWindow& window;
    std::unique_ptr<Menu> escMenu;
    bool escMenuActive = false;
    sf::Clock clock;
    float dt;
    bool isPlayer1Turn = true;
    bool oldIsPlayer1Turn = true;
    std::unique_ptr<Menu> resultMenu = nullptr;

    bool singleGame = false;


    void showResults();

    void initResulltMenu(sf::Font& font, bool isPlayer1Win);

public:
    std::unique_ptr<Board> board;
    
    Game(sf::RenderWindow& window, bool singleGame, sf::Font& font);

    void run(bool _loadGame);
    
    void processEvents();

    void loadGame();
    
};