#include "button.h"

Button::Button(float x, float y, float width, float height,
               sf::Font* font, std::string text,
               sf::Color idleColor, sf::Color hoverColor, sf::Color activeColor)
{
    this->shape.setSize(sf::Vector2f(width, height));
    this->shape.setPosition(sf::Vector2f(x, y));
    
    this->font=font;
    this->text.setFont(*this->font);
    this->text.setString(text);
    this->text.setFillColor(sf::Color::White);
    this->text.setCharacterSize(12);
    this->text.setPosition(
        this->shape.getPosition().x 
        +
        this->shape.getGlobalBounds().width / 2.f
        -
        this->text.getGlobalBounds().width / 2.f,
        this->shape.getPosition().y
        +
        this->shape.getGlobalBounds().height / 2.f
        -
        this->text.getGlobalBounds().height / 2.f
    );

    this->idleColor=idleColor;
    this->hoverColor=hoverColor;
    this->activeColor=activeColor;
    return;
}

Button::~Button()
{

}

void Button::render(sf::RenderTarget* target)
{
    target->draw(this->shape);
    target->draw(this->text);
}

void Button::update(const sf::Vector2f& mousePos)
{
    /* Update the button's position and size */
    this->text.setPosition(
        this->shape.getPosition().x 
        +
        this->shape.getGlobalBounds().width / 2.f
        -
        this->text.getGlobalBounds().width / 2.f,
        this->shape.getPosition().y
    );

    /* Update the booleans for hover and pressed */

    // Idle
    this->text.setFillColor(sf::Color::White);
    this->shape.setFillColor(this->idleColor);

    // Hover
    if(this->shape.getGlobalBounds().contains(mousePos))
    {
        this->text.setFillColor(sf::Color::Black);
        this->shape.setFillColor(this->hoverColor);

        // Pressed
        if(sf::Mouse::isButtonPressed(sf::Mouse::Left)||sf::Mouse::isButtonPressed(sf::Mouse::Right))
        {
            this->shape.setFillColor(this->activeColor);
        }
    }
}

void Button::setposition(float x, float y)
{
    this->shape.setPosition(sf::Vector2f(x, y));
    return;
}
