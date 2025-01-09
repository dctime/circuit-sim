#include "button.h"
#include <SFML/Graphics.hpp>

Button::Button(float x, float y, float width, float height,
               sf::Font *font, const std::string &text,
               sf::Color idleColor, sf::Color hoverColor, sf::Color activeColor)
    : tooltip(x, y, width * 2, height * 2, *font, "hello world", sf::Color::Blue, sf::Color::White)
{
    this->shape.setSize(sf::Vector2f(width, height));
    this->shape.setPosition(sf::Vector2f(x, y));

    this->font = font;
    this->text.setFont(*this->font);
    this->text.setString(text);
    this->text.setFillColor(sf::Color::White);
    this->text.setCharacterSize(12);
    this->text.setPosition(
        this->shape.getPosition().x +
            this->shape.getGlobalBounds().width / 2.f -
            this->text.getGlobalBounds().width / 2.f,
        this->shape.getPosition().y +
            this->shape.getGlobalBounds().height / 2.f -
            this->text.getGlobalBounds().height / 2.f);

    this->idleColor = idleColor;
    this->hoverColor = hoverColor;
    this->activeColor = activeColor;
}

Button::~Button() {}

void Button::render(sf::RenderTarget *target)
{
    target->draw(this->shape);
    target->draw(this->text);
    if (tooltip.isVisible())
    {
        tooltip.render(*target);
    }
}

void Button::update(const sf::Vector2f &mousePos)
{
    // Default state: idle
    this->text.setPosition(
        this->shape.getPosition().x +
            this->shape.getGlobalBounds().width / 2.f -
            this->text.getGlobalBounds().width / 2.f,
        this->shape.getPosition().y);
    this->shape.setFillColor(this->idleColor);
    this->text.setFillColor(sf::Color::White);
    tooltip.setVisible(false);

    // Hover state
    if (this->shape.getGlobalBounds().contains(mousePos))
    {
        this->shape.setFillColor(this->hoverColor);
        this->text.setFillColor(sf::Color::Black);

        tooltip.setPosition(this->shape.getPosition().x, this->shape.getPosition().y + this->shape.getSize().y + 5, 800, 600);
        tooltip.setVisible(true);

        // Active state (pressed)
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) || sf::Mouse::isButtonPressed(sf::Mouse::Right))
        {
            this->shape.setFillColor(this->activeColor);
        }
    }
}

void Button::setPosition(float x, float y)
{
    this->shape.setPosition(sf::Vector2f(x + 5, y - 5));
    tooltip.setPosition(x + 5, y + this->shape.getSize().y + 5, 800, 600);
}
