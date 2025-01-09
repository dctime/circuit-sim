#include "tooltip.h"
Tooltip::Tooltip(float x, float y, float width, float height,
                 sf::Font &font, const std::string &text,
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

void Tooltip::render(sf::RenderTarget &target)
{
    if (this->visible)
    {
        target.draw(this->shape);
        target.draw(this->text);
    }
}

void Tooltip::setVisible(bool state)
{
    this->visible = state;
}

bool Tooltip::isVisible() const
{
    return this->visible;
}
void Tooltip::setPosition(float x, float y, float windowWidth, float windowHeight)
{
    float tooltipWidth = this->shape.getSize().x;
    float tooltipHeight = this->shape.getSize().y;

    // Adjust x position if tooltip goes beyond the window width
    if (x + tooltipWidth > windowWidth)
    {
        x = windowWidth - tooltipWidth-65;
    }
    // Adjust y position if tooltip goes beyond the window height
    if (y + tooltipHeight > windowHeight)
    {
        y = windowHeight - tooltipHeight;
    }
    if (y < 0)
    {
        y = 0;
    }

    this->shape.setPosition(sf::Vector2f(x, y-75));
    this->text.setPosition(x + 10, y + 10); // Adjust text position with some padding
}