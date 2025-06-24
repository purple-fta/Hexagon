// src/Palette.h
#pragma once
#include <SFML/Graphics.hpp>

namespace Palette {
    // Base Colors
    const sf::Color Teal(148, 226, 213);
    const sf::Color Green(166, 227, 161);
    const sf::Color Mauve(203, 166, 247);
    const sf::Color Sky(137, 220, 235);
    const sf::Color Red(243, 139, 168);
    const sf::Color Yellow(249, 226, 175);

    // UI Colors
    const sf::Color Background(30, 30, 46);
    const sf::Color Surface0(49, 50, 68);
    const sf::Color Surface1(69, 71, 90);

    // Board Colors
    const sf::Color emptyColor = Palette::Surface0;
    const sf::Color p1Color = Palette::Green;
    const sf::Color p2Color = sf::Color(255, 85, 85);
    const sf::Color emptyHoverColor = Palette::Surface1;
    const sf::Color p1HoverColor = Palette::Teal;
    const sf::Color p2HoverColor = Palette::Red;
    
    const sf::Color selectedCell = Palette::Mauve; 

}