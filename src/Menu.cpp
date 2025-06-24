#include "Menu.h"
#include <iostream>

namespace 
{
    int button_size = 90;
    int button_font_size = 24;
    int button_offset = 35;

    float animation_duration = 0.2f;
}


HexButton::HexButton(float x, float y, float size, int font_size, const std::string& label, const sf::Font& font, sf::Color outlineColor)
    : text(font, label, font_size) 
{
    shape.setPointCount(6);
    for (int i = 0; i < 6; ++i) {
        float angle = i * 2 * 3.14159f / 6;
        shape.setPoint(i, {x + size * static_cast<float>(cos(angle)), y + size * static_cast<float>(sin(angle))});
    }
    
    shape.setFillColor(normalColor);
    shape.setOutlineThickness(3);
    shape.setOutlineColor(outlineColor);

    text.setFillColor(sf::Color::White);
    text.setPosition({x - text.getGlobalBounds().size.x / 2, y - text.getGlobalBounds().size.y / 2 - 5});
}

void HexButton::draw(sf::RenderWindow& window) {
    window.draw(shape);
    window.draw(text);
}

void HexButton::ifHovered(const sf::Vector2f& mousePos)  {
    hovered = shape.getGlobalBounds().contains(mousePos);
    if (hovered != oldHoveredState) {
        oldColor = currentColor;
        animationTime = 0.0f;
        targetColor = hovered ? hoverColor : normalColor;
        oldHoveredState = hovered;
    }
}

void HexButton::updateColor(float dt) {
    animationTime += dt;
    float t = animationTime / animation_duration;

    // std::cout << t << std::endl;
    if (t > 1.0f) {
        t = 1.0f;
    }

    currentColor.r = (int)((1 - t) * oldColor.r + t * targetColor.r);
    currentColor.g = (int)((1 - t) * oldColor.g + t * targetColor.g);
    currentColor.b = (int)((1 - t) * oldColor.b + t * targetColor.b);

    shape.setFillColor(currentColor);
}

bool HexButton::isHovered(const sf::Vector2f& mousePos) {
    return shape.getGlobalBounds().contains(mousePos);
}


Menu::Menu(sf::Font& font, sf::RenderWindow& window, std::vector<sf::Color>& colors, std::vector<std::string>& labels, std::string titleLabel, sf::Color titleColor) : window(window), title(font, titleLabel, 120) {
    // Создание заголовка
    title.setFillColor(titleColor);
    title.setPosition({
        (window.getSize().x - title.getGlobalBounds().size.x) / 2,
        (window.getSize().y - title.getGlobalBounds().size.y) / 2 - 200
    });


    // Установка начальных координат и размеров кнопок
    float start_x = window.getSize().x / 2.0f - ((button_size*2 + button_offset) * (labels.size()-1)) / 2.0f;
    int offset = (button_size*2 + button_offset);
    for (size_t i = 0; i < labels.size(); i++) {
        buttons.emplace_back(
            start_x + i * offset,
            400,
            button_size,
            button_font_size,
            labels.at(i),
            font,
            colors.at(i)
        );
    }
}

void Menu::draw() {
    window.clear(Palette::Background);
    
    window.draw(title);

    for (auto& button : buttons) {
        button.draw(window);
    }
    
    window.display();
}

void Menu::handleEvent(const sf::Event& event) {
    if (const auto* mouseMoved = event.getIf<sf::Event::MouseMoved>())
    {
        for (auto& button : buttons) {
            button.ifHovered({static_cast<float>(mouseMoved->position.x), 
                              static_cast<float>(mouseMoved->position.y)});
        }
    } else if (const auto* mouseButton = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mouseButton->button == sf::Mouse::Button::Left) {
            for (size_t i = 0; i < buttons.size(); ++i) {
                if (buttons.at(i).isHovered({static_cast<float>(mouseButton->position.x), 
                                      static_cast<float>(mouseButton->position.y)})) {
                    selected = i;
                }
            }
        }
    }
}

void Menu::update(float dt) {
    for (auto& button : buttons) {
        button.updateColor(dt); // Примерный dt, можно заменить на реальный
    }
}

int Menu::getSelected() {
    return selected;
}

void Menu::resetSelected() {
    selected = -1;
}

