#pragma once

#include "AdjustableVoltageSourceElement.h"
#include "Line.h"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <UICircuit.h>
#include <UIElement.h>
#include <cmath>
#include <iostream>


class AdjustableVoltageSourceUIElement : public UIElement {
public:
  static void showGhostElement(sf::RenderWindow *window, int xGrid, int yGrid) {
    sf::Vector2f loc(xGrid * 50, yGrid * 50);
    AdjustableVoltageSourceUIElement::showElement(window, loc, 0, 0);
  }

private:
  std::unique_ptr<AdjustableVoltageSourceElement> element;

private:
  std::function<double(double)> v;
  UICircuit *uiCircuit;

public:
  AdjustableVoltageSourceUIElement(UICircuit *circuit, int xGrid, int yGrid,
                                   std::function<double(double)> v) {
    this->v = v;
    this->xGrid = xGrid;
    this->yGrid = yGrid;
    uiCircuit = circuit;
    uiElementID = circuit->getUIElementIDForUIElement((UIElement*) this);

    std::string pin1Loc =
        std::to_string(xGrid) + "," + std::to_string(yGrid - 1);
    std::string pin2Loc =
        std::to_string(xGrid) + "," + std::to_string(yGrid + 1);

    this->connectedLocs.push_back(pin1Loc);
    this->connectedLocs.push_back(pin2Loc);

    std::cout << "Adj. Voltage Source Init:" << std::endl;
    std::cout << "  Pin1Loc: " << pin1Loc << std::endl;
    std::cout << "  Pin2Loc: " << pin2Loc << std::endl;
  }

  void showElement(sf::RenderWindow *window) override {
    if (element.get() == nullptr || uiCircuit->getDisplayCircuit() == nullptr) {
      // show Ghost Version
      AdjustableVoltageSourceUIElement::showGhostElement(window, xGrid, yGrid);
    } else {
      double pin1Volt = 0;
      double pin2Volt = 0;
      if (element->getPin1() != -1) {
        pin1Volt =
            *uiCircuit->getDisplayCircuit()->getVoltagePointer(element->getPin1());
      }

      if (element->getPin2() != -1) {
        pin2Volt =
            *uiCircuit->getDisplayCircuit()->getVoltagePointer(element->getPin2());
      }

      showAdjustableVoltageSource(
          window, pin1Volt, pin2Volt,
          *uiCircuit->getDisplayCircuit()->getVoltagePointer(
              uiCircuit->getMaxNodeID() + element->getVoltageSourceID() + 1),
          xGrid, yGrid, uiCircuit->getCurrentScale());
      // show normal ones
    }
    // showAdjustableVoltageSource(window, *vp, *vm, *i, xGrid, yGrid,
    // *currentScale);
  }

  CircuitElement *getCircuitElementPointer() override {
    if (element.get() == nullptr) {
      std::string pin1Loc =
          std::to_string(xGrid) + "," + std::to_string(yGrid - 1);
      std::string pin2Loc =
          std::to_string(xGrid) + "," + std::to_string(yGrid + 1);

      element = AdjustableVoltageSourceElement::create(
          v, uiCircuit->getIDfromLoc(pin1Loc), uiCircuit->getIDfromLoc(pin2Loc),
          uiCircuit->getNextVoltageSourceID());
      std::cout << "Adj. Voltage Source Element Created!" << std::endl;
      std::cout << "Adj. Voltage Source UI Element added to UI Circuit. ID: "
                << uiElementID << std::endl;
    }
    return element.get();
  }

  void resetElement() override { element.reset(); }

public:
  ~AdjustableVoltageSourceUIElement() override {};

private:
  double lastoffsetVolt = 0;
  static void showElement(sf::RenderWindow *window, sf::Vector2f &loc,
                          double vp, double vm) {
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
  }
  void showAdjustableVoltageSource(sf::RenderWindow *window, double vp,
                                   double vm, double i, sf::Vector2f &loc,
                                   double currentScale) {
    showElement(window, loc, vp, vm);
    sf::Vector2f pointP1(loc.x, loc.y - 50);

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
    sf::Vector2f loc(xGrid * 50, yGrid * 50);
    showAdjustableVoltageSource(window, vp, vm, i, loc, currentScale);
  }
};
