#pragma once
#include <SFML/Graphics.hpp>
#include "tooltip.h"
#include <string>

class Button {
private:
    sf::RectangleShape shape; // Button's background shape
    sf::Text text;            // Button's label text
    sf::Font* font;           // Pointer to the font used by the button

    sf::Color idleColor;      // Button's default color
    sf::Color hoverColor;     // Color when the mouse hovers over the button
    sf::Color activeColor;    // Color when the button is pressed

    Tooltip tooltip;          // Tooltip for the button

public:
    // Constructor
    Button(float x, float y, float width, float height,
           sf::Font* font, const std::string& text,
           sf::Color idleColor, sf::Color hoverColor, sf::Color activeColor);

    // Destructor
    ~Button();

    // Render the button and its tooltip
    void render(sf::RenderTarget* target);

    // Update the button's state based on mouse position
    void update(const sf::Vector2f& mousePos);

    // Set a new position for the button
    void setPosition(float x, float y);
};

