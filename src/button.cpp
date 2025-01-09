#include "button.h"

Button::Button(float x, float y, float width, float height,
               sf::Font *font, const std::string &text,
               sf::Color idleColor, sf::Color hoverColor, sf::Color activeColor,
               const std::string &tooltipText)
    : tooltip(x, y, width * 2, height * 2, *font, tooltipText, sf::Color::Blue, sf::Color::White)
{
    ts.tv_sec = 0;
    ts.tv_nsec = 100000000L; // 100毫秒

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
    if(isPressed)
    {
        this->shape.setFillColor(this->activeColor);
    }
    else
    {
    this->shape.setFillColor(this->idleColor);
    }    
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

void Button::update_ispress(const sf::Vector2f &mousePos, const std::vector<Button *> &buttons)
{
    // 默认状态：空闲
    this->text.setPosition(
        this->shape.getPosition().x +
            this->shape.getGlobalBounds().width / 2.f -
            this->text.getGlobalBounds().width / 2.f,
        this->shape.getPosition().y);
    
    if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
    {
        this->isPressed = false;
    }
    
    if (!this->isPressed)
    {
        this->shape.setFillColor(this->idleColor);
    }
    else
    {
        this->shape.setFillColor(this->activeColor);
    }
    
    this->text.setFillColor(sf::Color::White);
    this->tooltip.setVisible(false);

    // 检查是否鼠标悬停在按钮上
    if (this->shape.getGlobalBounds().contains(mousePos))
    {
        this->shape.setFillColor(this->hoverColor);
        this->text.setFillColor(sf::Color::Black);

        this->tooltip.setPosition(this->shape.getPosition().x, this->shape.getPosition().y + this->shape.getSize().y + 5, 800, 600);
        this->tooltip.setVisible(true);

        // 激活状态（按下）
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            if (!this->isPressed)
            {
                // 重置其他按钮状态
                for (auto &button : buttons)
                {
                    if (button != this)
                    {
                        button->isPressed = false;
                    }
                }
                this->isPressed = true; // 激活当前按钮
                nanosleep(&this->ts, NULL); // 等待 100 毫秒
                this->click(); // 调用回调函数
            }
            else
            {
                this->isPressed = false; // 再次按下时取消激活
                nanosleep(&this->ts, NULL); // 等待 100 毫秒
            }
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
        {
            this->isPressed = false; // 右键按下时取消激活
        }
    }
}
void Button::update(const sf::Vector2f &mousePos)
{
    // 默认状态：空闲
    this->text.setPosition(
        this->shape.getPosition().x +
            this->shape.getGlobalBounds().width / 2.f -
            this->text.getGlobalBounds().width / 2.f,
        this->shape.getPosition().y);
    
    if (this->shape.getGlobalBounds().contains(mousePos))
    {
        this->shape.setFillColor(this->hoverColor);
        this->text.setFillColor(sf::Color::Black);

        this->tooltip.setPosition(this->shape.getPosition().x, this->shape.getPosition().y + this->shape.getSize().y + 5, 800, 600);
        this->tooltip.setVisible(true);
    }
    else
    {
        this->shape.setFillColor(this->idleColor);
        this->text.setFillColor(sf::Color::White);
        this->tooltip.setVisible(false);
    }
}
void Button::setPosition(float x, float y)
{
    this->shape.setPosition(sf::Vector2f(x, y));
    tooltip.setPosition(x, y + this->shape.getSize().y + 5, 800, 600);
}

void Button::connect(std::function<void()> func)
{
    callback = func;
}

void Button::click()
{
    if (callback)
    {
        callback();
    }
}
