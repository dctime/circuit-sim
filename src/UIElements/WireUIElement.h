#pragma once
#include "../Line.h"
#include "../UIElement.h"
#include "VoltageSourceElement.h"
#include <SFML/System/Vector2.hpp>
#include <UICircuit.h>
#include <iostream>

class WireUIElement : public UIElement {
public:
  static void showGhostElement(sf::RenderWindow *window, int xGrid1, int yGrid1,
                               int xGrid2, int yGrid2) {
    sf::Vector2f loc1(xGrid1 * 50, yGrid1 * 50);
    sf::Vector2f loc2(xGrid2 * 50, yGrid2 * 50);
    WireUIElement::showWire(window, 0, loc1, loc2);
  }

private:
  int xGrid1, xGrid2, yGrid1, yGrid2;
  std::unique_ptr<VoltageSourceElement> element;

public:
  ~WireUIElement() override {};
  WireUIElement(UICircuit *circuit, int xGrid1, int yGrid1, int xGrid2,
                int yGrid2) {
    uiCircuit = circuit;
    this->xGrid = -1;
    this->yGrid = -1;
    this->xGrid1 = xGrid1;
    this->xGrid2 = xGrid2;
    this->yGrid1 = yGrid1;
    this->yGrid2 = yGrid2;

    std::string pin1Loc = std::to_string(xGrid1) + "," + std::to_string(yGrid1);
    std::string pin2Loc = std::to_string(xGrid2) + "," + std::to_string(yGrid2);

    connectedLocs.push_back(pin1Loc);
    connectedLocs.push_back(pin2Loc);

    std::cout << "Wire Init: " << std::endl;
    std::cout << "  Pin1Loc: " << pin1Loc << std::endl;
    std::cout << "  Pin2Loc: " << pin2Loc << std::endl;
  }

  void showElement(sf::RenderWindow *window) override {
    if (element.get() == nullptr) {
      WireUIElement::showGhostElement(window, xGrid1, yGrid1, xGrid2, yGrid2);
    } else {
      int pin1Volt = 0;

      if (element->getPin1() != -1) {
        pin1Volt = *uiCircuit->getCircuit()->getVoltagePointer(element->getPin1());
      }


      showWire(
          window, xGrid1, yGrid1, xGrid2, yGrid2,
          pin1Volt,
          *uiCircuit->getCircuit()->getVoltagePointer(
              uiCircuit->getMaxNodeID() + 1 + element->getVoltageSourceID()),
          uiCircuit->getCurrentScale());
    }
  }

  CircuitElement *getCircuitElementPointer() override {
    if (element.get() == nullptr) {
      std::string pin1Loc =
          std::to_string(xGrid1) + "," + std::to_string(yGrid1);
      std::string pin2Loc =
          std::to_string(xGrid2) + "," + std::to_string(yGrid2);

      element = VoltageSourceElement::create(
          0, uiCircuit->getIDfromLoc(pin1Loc), uiCircuit->getIDfromLoc(pin2Loc),
          uiCircuit->getNextVoltageSourceID());
      std::cout << "Wire Element Created!" << std::endl;
      std::cout << "Wire UI Element added to UI Circuit. ID: " << uiElementID << std::endl;
    }
    return element.get();
  }

private:
  double lastoffsetVolt = 0;
  static void showWire(sf::RenderWindow *window, double v, sf::Vector2f &loc1,
                       sf::Vector2f &loc2) {
    double width = 5;
    sf::Color wireColor;
    voltToColor(v, wireColor);
    showLine(window, loc1, loc2, width, wireColor, wireColor, wireColor);
  }

  void showWire(sf::RenderWindow *window, sf::Vector2f &loc1,
                sf::Vector2f &loc2, double v, double i, double currentScale) {
    WireUIElement::showWire(window, v, loc1, loc2);
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
