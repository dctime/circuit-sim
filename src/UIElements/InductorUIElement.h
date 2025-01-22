#pragma once
#include "Line.h"
#include "UIElement.h"
#include <InductorElement.h>
#include <SFML/Graphics/RenderWindow.hpp>
#include <UICircuit.h>

class InductorUIElement : public UIElement {
public:
  void resetElement() override {}

private:
  UICircuit *uiCircuit;
  int xGrid, yGrid;
  double L;
  std::unique_ptr<InductorElement> element;

public:
  InductorUIElement(UICircuit *circuit, int xGrid, int yGrid, double L) {
    uiCircuit = circuit;
    this->xGrid = xGrid;
    this->yGrid = yGrid;
    this->L = L;

    std::string pin1Loc =
        std::to_string(xGrid) + "," + std::to_string(yGrid - 1);
    std::string pin2Loc =
        std::to_string(xGrid) + "," + std::to_string(yGrid + 1);
    std::string pinMLoc =
        "E" + std::to_string(uiElementID) + "PinM";

    this->connectedLocs.push_back(pin1Loc);
    this->connectedLocs.push_back(pin2Loc);
    this->connectedLocs.push_back(pinMLoc);

    std::cout << "Inductor Init: " << std::endl;
    std::cout << "  Pin1Loc: " << pin1Loc << std::endl;
    std::cout << "  Pin2Loc: " << pin2Loc << std::endl;
    std::cout << "  PinMLoc: " << pinMLoc << std::endl;

  }

  CircuitElement *getCircuitElementPointer() override {
    if (element.get() == nullptr) {
      std::string pin1Loc =
          std::to_string(xGrid) + "," + std::to_string(yGrid - 1);
      std::string pin2Loc =
          std::to_string(xGrid) + "," + std::to_string(yGrid + 1);
      std::string pinMLoc =
        "E" + std::to_string(uiElementID) + "PinM";

      element = InductorElement::create(L, 0, uiCircuit->getIDfromLoc(pin1Loc),
                                         uiCircuit->getIDfromLoc(pin2Loc), uiCircuit->getIDfromLoc(pinMLoc), uiCircuit->getNextVoltageSourceID());
      std::cout << "Inductor Element Created!" << std::endl;
      std::cout << "Inductor UI Element added to UI Circuit. ID: "
                << uiElementID << std::endl;
    }

    return element.get();
  }
  void showElement(sf::RenderWindow *window) override {
    // FIXME: element is the newest version. to get the rendered version use
    if (element.get() == nullptr || uiCircuit->getDisplayCircuit() == nullptr) {
      InductorUIElement::showGhostElement(window, xGrid, yGrid);
    } else {
      double pin1Volt = 0;
      double pin2Volt = 0;
      if (element->getPIN1() != -1) {
        pin1Volt = *uiCircuit->getDisplayCircuit()->getVoltagePointer(
            element->getPIN1());
      }

      if (element->getPIN2() != -1) {
        pin2Volt = *uiCircuit->getDisplayCircuit()->getVoltagePointer(
            element->getPIN2());
      }

      // std::cout << "CID:" << uiCircuit->getDisplayCircuit()->getCircuitID()
                // << "V1-V2:" << pin1Volt - pin2Volt
                // << "element:" << prePin1Volt - prePin2Volt << std::endl;
      double i =  *uiCircuit->getDisplayCircuit()->getVoltagePointer(
              uiCircuit->getMaxNodeID() +
              element->getVoltageSourceID() + 1); 
      showInductor(window, pin1Volt, pin2Volt, i, xGrid, yGrid,
                    uiCircuit->getCurrentScale());
    }
  }

private:
  static void showInductor(sf::RenderWindow *window, sf::Vector2f &loc,
                            double v1, double v2) {
    double width = 5;
    sf::Vector2f point11(loc.x, loc.y - 50);
    sf::Vector2f point12(loc.x, loc.y - 10);
    // sf::Vector2f pointP3(loc.x - 30, loc.y - 10);
    // sf::Vector2f pointP4(loc.x + 30, loc.y - 10);
    sf::Vector2f point21(loc.x, loc.y + 50);
    sf::Vector2f point22(loc.x, loc.y + 10);
    // sf::Vector2f pointM3(loc.x - 30, loc.y + 10);
    // sf::Vector2f pointM4(loc.x + 30, loc.y + 10);

    sf::Color color1;
    voltToColor(v1, color1);
    sf::Color color2;
    voltToColor(v2, color2);

    showLine(window, point11, point12, width, color1, color1, color1);
    // showLine(window, pointP3, pointP4, width, color1, color1, color1);
    showLine(window, point21, point22, width, color2, color2, color2);
    // showLine(window, pointM3, pointM4, width, color2, color2, color2);
  }

  double lastoffset = 0;
  void showInductor(sf::RenderWindow *window, double v1, double v2, double i,
                     sf::Vector2f &loc, double currentScale) {
    InductorUIElement::showInductor(window, loc, v1, v2);

    sf::Vector2f point11(loc.x, loc.y - 50);
    double current = i;
    lastoffset += current * currentScale;
    lastoffset = std::remainder(lastoffset, 20);
    lastoffset =
        lastoffset < 0 ? lastoffset + 20.0 : lastoffset;
    for (int i = 0; i < 5; ++i) {
      sf::CircleShape circle(5.0 / 2);
      circle.setPosition(point11.x - circle.getRadius(),
                         point11.y + 20 * i - circle.getRadius() +
                             lastoffset);
      circle.setFillColor(sf::Color(200, 200, 0));
      window->draw(circle);
    }
  }
  void showInductor(sf::RenderWindow *window, double v1, double v2, double i,
                     int xGrid, int yGrid, double currentScale) {
    sf::Vector2f loc(xGrid * 50, yGrid * 50);
    showInductor(window, v1, v2, i, loc, currentScale);
  }

private:
  static void showGhostElement(sf::RenderWindow *window, int xGrid, int yGrid) {
    sf::Vector2f loc(xGrid * 50, yGrid * 50);
    InductorUIElement::showInductor(window, loc, 0, 0);
  }
};
