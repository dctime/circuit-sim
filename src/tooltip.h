#ifndef TOOLTIP_H
#define TOOLTIP_H

#include <SFML/Graphics.hpp>
#include <string>

class Tooltip {
private:
    sf::RectangleShape shape;
    sf::Text text;
    bool visible;

public:
    Tooltip(float x, float y, float width, float height,
            sf::Font& font, const std::string& text,
            sf::Color backgroundColor, sf::Color textColor);

    ~Tooltip() = default;

    void render(sf::RenderTarget& target);
    void setPosition(float x, float y, float windowWidth, float windowHeight);  
    void setVisible(bool state);
    bool isVisible() const;
};

#endif // TOOLTIP_H
