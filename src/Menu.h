#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <cmath>
#include "pallete.h"

struct HexButton {
    sf::ConvexShape shape;
    sf::Text text;
    sf::Color color;
    sf::Color hoverColor = Palette::Surface1;
    sf::Color normalColor = Palette::Surface0;
    sf::Color currentColor = normalColor;
    sf::Color oldColor = normalColor;
    sf::Color targetColor = normalColor;

    bool hovered = false;
    bool oldHoveredState = false;    
    float animationTime = 0.0f;

    HexButton(float x, float y, float size, int font_size, 
              const std::string& label, const sf::Font& font, sf::Color outlineColor);
    
    void draw(sf::RenderWindow& window);

    void ifHovered(const sf::Vector2f& mousePos);

    void updateColor(float dt);
    
    bool isHovered(const sf::Vector2f& mousePos);
};


class Menu {
public:
    Menu(sf::Font& font, sf::RenderWindow& window, 
         std::vector<sf::Color>& colors, std::vector<std::string>& labels, std::string titleLabel, sf::Color titleColor);

    // Обработка событий (мышь, клавиатура)
    void handleEvent(const sf::Event& event);

    // Отрисовка меню
    void draw();

    void update(float dt);

    int getSelected();
    void resetSelected();

private:
    sf::RenderWindow& window;
    std::vector<HexButton> buttons;
    sf::Text title;
    int selected = -1;
};