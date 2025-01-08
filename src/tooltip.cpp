#include "tooltip.h"
Tooltip::Tooltip(float x, float y, float width, float height,
                 sf::Font& font, const std::string& text,
                 sf::Color backgroundColor, sf::Color textColor)
    : visible(false)
{
    this->shape.setSize(sf::Vector2f(width, height));
    this->shape.setPosition(sf::Vector2f(x, y));
    this->shape.setFillColor(backgroundColor);

    this->text.setFont(font);
    this->text.setString(text);
    this->text.setFillColor(textColor);
    this->text.setCharacterSize(12);
}

void Tooltip::render(sf::RenderTarget& target) {
    if (this->visible) {
        target.draw(this->shape);
        target.draw(this->text);
    }
}

void Tooltip::setPosition(float x, float y) {
    this->shape.setPosition(sf::Vector2f(x, y-175));
    this->text.setPosition(x , y - 150);
}

void Tooltip::setVisible(bool state) {
    this->visible = state;
}

bool Tooltip::isVisible() const {
    return this->visible;
}
