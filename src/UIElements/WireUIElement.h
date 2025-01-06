#pragma once
#include "../Line.h"
#include "../UIElement.h"
#include <SFML/System/Vector2.hpp>

class WireUIElement : public UIElement {
private:
  int xGrid1, xGrid2, yGrid1, yGrid2;
  double *v;
  double *i;
  double *currentScale;

public:
  ~WireUIElement() override {};
  WireUIElement(double *v, double *i, double *currentScale, int xGrid1,
                int yGrid1, int xGrid2, int yGrid2) {
    this->xGrid1 = xGrid1;
    this->xGrid2 = xGrid2;
    this->yGrid1 = yGrid1;
    this->yGrid2 = yGrid2;
    this->v = v;
    this->i = i;
    this->currentScale = currentScale;
  }

  void showElement(sf::RenderWindow *window) override {
    showWire(window, xGrid1, yGrid1, xGrid2, yGrid2, *v, *i,
             *currentScale);
  }

private:
  double lastoffsetVolt = 0;
  void showWire(sf::RenderWindow *window, sf::Vector2f &loc1,
                sf::Vector2f &loc2, double v, double i, double currentScale) {
    double width = 5;
    sf::Color wireColor;
    voltToColor(v, wireColor);
    showLine(window, loc1, loc2, width, wireColor, wireColor, wireColor);

    double current = i;
    lastoffsetVolt += current * currentScale;
    lastoffsetVolt = std::remainder(lastoffsetVolt, 20);
    lastoffsetVolt =
        lastoffsetVolt < 0 ? lastoffsetVolt + 20.0 : lastoffsetVolt;
    double length =
        std::sqrt(std::pow(loc1.x - loc2.x, 2) + std::pow(loc1.y - loc2.y, 2));
    double unitX = (loc2.x - loc1.x) / length;
    double unitY = (loc2.y - loc1.y) / length;
    int max = (int)(std::sqrt(std::pow(loc1.x - loc2.x, 2) +
                              std::pow(loc1.y - loc2.y, 2)) /
                    20);
    for (int i = 0; i < max; ++i) {
      sf::CircleShape circle(5.0 / 2);
      circle.setPosition(loc1.x + 20 * i * unitX - circle.getRadius() +
                             lastoffsetVolt * unitX,
                         loc1.y + 20 * i * unitY - circle.getRadius() +
                             lastoffsetVolt * unitY);
      circle.setFillColor(sf::Color(200, 200, 0));
      window->draw(circle);
    }
  }
  void showWire(sf::RenderWindow *window, int xGrid1, int yGrid1, int xGrid2,
                int yGrid2, double v, double i, double currentScale) {
    sf::Vector2f loc1(xGrid1 * 50, yGrid1 * 50);
    sf::Vector2f loc2(xGrid2 * 50, yGrid2 * 50);
    showWire(window, loc1, loc2, v, i, currentScale);
  }
};
