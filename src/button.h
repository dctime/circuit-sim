#pragma once

#include <iostream>
#include <ctime>
#include <cstdlib>
#include <sstream>

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
class Button
{
    private:
        sf::RectangleShape shape;
        sf::Font* font;
        sf::Text text;

        sf::Color idleColor;
        sf::Color hoverColor;
        sf::Color activeColor;
    public:
        Button(float x, float y, float width, float height,
               sf::Font* font, std::string text,
               sf::Color idleColor, sf::Color hoverColor, sf::Color activeColor);
        ~Button();
       // void ispressed();
        void render(sf::RenderTarget* target);
        void update(const sf::Vector2f& mousePos);
        void setposition(float x, float y);
};