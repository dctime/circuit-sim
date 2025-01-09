#pragma once
#include <SFML/Graphics.hpp>
#include "tooltip.h"
#include <string>
#include <functional>
#include <iostream>
#include <time.h>

class Button
{
private:
    sf::RectangleShape shape;      // 按鈕的背景形狀
    sf::Text text;                 // 按鈕的標籤文字
    sf::Font *font;                // 字型指標
    sf::Color idleColor;           // 預設顏色
    sf::Color hoverColor;          // 滑鼠懸停顏色
    sf::Color activeColor;         // 按下時顏色
    Tooltip tooltip;               // 按鈕的工具提示
    bool isPressed = false;        // 按鈕是否被按下
    std::function<void()> callback; // 點擊時執行的回呼函數
    struct timespec ts;

public:
    Button(float x, float y, float width, float height,
           sf::Font *font, const std::string &text,
           sf::Color idleColor, sf::Color hoverColor, sf::Color activeColor,
           const std::string &tooltipText);

    ~Button();

    void render(sf::RenderTarget *target);
    void update_ispress(const sf::Vector2f &mousePos,const std::vector<Button*> &buttons);
    void update(const sf::Vector2f &mousePos);
    void setPosition(float x, float y);
    void connect(std::function<void()> func);
    void click();
};
