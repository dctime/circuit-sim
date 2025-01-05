#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include "Line.h"
#include "UIElement.h"

class AdjustableVoltageSourceUIElement : public UIElement{
private:
  double* vp;
  double* vm;
  double* i;
  double* currentScale;
public:
  AdjustableVoltageSourceUIElement(double* vp, double* vm, double* i, double* currentScale, int xGrid, int yGrid) {
    this->vp = vp;
    this->vm = vm;
    this->i = i;
    this->currentScale = currentScale;
    this->xGrid = xGrid;
    this->yGrid = yGrid;
  }
  
  void showElement(sf::RenderWindow* window) override {
    showAdjustableVoltageSource(window, *vp, *vm, *i, xGrid, yGrid, *currentScale); 
  }

 
public:
  ~AdjustableVoltageSourceUIElement() override {};
private:
  double lastoffsetVolt = 0;
 
  void showAdjustableVoltageSource(sf::RenderWindow *window, double vp,
                                   double vm, double i, sf::Vector2f &loc,
                                   double currentScale) {
    double width = 5;

    sf::CircleShape circle(25);
    circle.setFillColor(sf::Color(0, 0, 0));
    circle.setOutlineThickness(width);
    circle.setOutlineColor(sf::Color(30, 30, 30));

    circle.setPosition(loc.x - circle.getRadius(), loc.y - circle.getRadius());

    sf::Vector2f pointP1(loc.x, loc.y - 50);
    sf::Vector2f pointP2(loc.x, loc.y - 25);
    sf::Vector2f pointM1(loc.x, loc.y + 50);
    sf::Vector2f pointM2(loc.x, loc.y + 25);
    sf::Vector2f pointADD1(loc.x - 10, loc.y - 10);
    sf::Vector2f pointADD2(loc.x + 10, loc.y - 10);
    sf::Vector2f pointADD3(loc.x, loc.y - 10 + 10);
    sf::Vector2f pointADD4(loc.x, loc.y - 10 - 10);
    sf::Vector2f pointMINUS1(loc.x - 10, loc.y + 12.5);
    sf::Vector2f pointMINUS2(loc.x + 10, loc.y + 12.5);

    sf::Color colorP;
    voltToColor(vp, colorP);
    sf::Color colorM;
    voltToColor(vm, colorM);

    showLine(window, pointP1, pointP2, width, colorP, colorP, colorP);
    showLine(window, pointM1, pointM2, width, colorM, colorM, colorM);

    window->draw(circle);

    showLine(window, pointADD1, pointADD2, width, sf::Color(30, 30, 30),
             sf::Color(30, 30, 30), sf::Color(30, 30, 30));
    showLine(window, pointADD3, pointADD4, width, sf::Color(30, 30, 30),
             sf::Color(30, 30, 30), sf::Color(30, 30, 30));
    showLine(window, pointMINUS1, pointMINUS2, width, sf::Color(30, 30, 30),
             sf::Color(30, 30, 30), sf::Color(30, 30, 30));

    double current = i;
    lastoffsetVolt += current * currentScale;
    lastoffsetVolt = std::remainder(lastoffsetVolt, 20);
    lastoffsetVolt =
        lastoffsetVolt < 0 ? lastoffsetVolt + 20.0 : lastoffsetVolt;
    for (int i = 0; i < 5; ++i) {
      sf::CircleShape circle(5.0 / 2);
      circle.setPosition(pointP1.x - circle.getRadius(),
                         pointP1.y + 20 * i - circle.getRadius() +
                             lastoffsetVolt);
      circle.setFillColor(sf::Color(200, 200, 0));
      window->draw(circle);
    }
  }

  void showAdjustableVoltageSource(sf::RenderWindow *window, double vp,
                                   double vm, double i, int xGrid, int yGrid,
                                   double currentScale) {
    sf::Vector2f loc(xGrid*50, yGrid*50);
    showAdjustableVoltageSource(window, vp, vm, i, loc, currentScale);
  }
};
