#pragma once
#include "Line.h"
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include "UIElement.h"
#include <VoltageSourceElement.h>
#include <memory>
#include <iostream>

class VoltageSourceUIElement : public UIElement{
private:
  std::unique_ptr<VoltageSourceElement> element;
public:
  ~VoltageSourceUIElement() override {};
  VoltageSourceUIElement(int xGrid, int yGrid, double v) {
    this->xGrid = xGrid;
    this->yGrid = yGrid;

    std::string pin1Loc = std::to_string(xGrid) + "," + std::to_string(yGrid-1);

    std::string pin2Loc = std::to_string(xGrid) + "," + std::to_string(yGrid+1);

    this->connectedLocs.push_back(pin1Loc);
    this->connectedLocs.push_back(pin2Loc);

    std::cout << "Voltage Source Added to UI Circuit: " << std::endl;
    std::cout << "  Pin1Loc: " << pin1Loc << std::endl;
    std::cout << "  Pin2Loc: " << pin2Loc << std::endl;
  }

  void showElement(sf::RenderWindow *window) override {
    // showVoltageSource(window, *vp, *vm, *i, xGrid, yGrid, *currentScale);
  }

  CircuitElement * getCircuitElementPointer() override {
    return element.get();
  }

private:
  double lastoffsetVolt = 0;
  void showVoltageSource(sf::RenderWindow *window, double vp, double vm,
                         double i, sf::Vector2f &loc, double currentScale) {
    double width = 5;

    sf::Vector2f pointP1(loc.x, loc.y - 50);
    sf::Vector2f pointP2(loc.x, loc.y - 10);
    sf::Vector2f pointP3(loc.x - 30, loc.y - 10);
    sf::Vector2f pointP4(loc.x + 30, loc.y - 10);
    sf::Vector2f pointM1(loc.x, loc.y + 50);
    sf::Vector2f pointM2(loc.x, loc.y + 10);
    sf::Vector2f pointM3(loc.x - 20, loc.y + 10);
    sf::Vector2f pointM4(loc.x + 20, loc.y + 10);

    sf::Color colorP;
    voltToColor(vp, colorP);
    sf::Color colorM;
    voltToColor(vm, colorM);

    showLine(window, pointP1, pointP2, width, colorP, colorP, colorP);
    showLine(window, pointP3, pointP4, width, colorP, colorP, colorP);
    showLine(window, pointM1, pointM2, width, colorM, colorM, colorM);
    showLine(window, pointM3, pointM4, width, colorM, colorM, colorM);

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

  void showVoltageSource(sf::RenderWindow *window, double vp, double vm,
                         double i, int xGrid, int yGrid, double currentScale) {
    sf::Vector2f loc(xGrid*50, yGrid*50);
    showVoltageSource(window, vp, vm, i, loc, currentScale);
  }
};
