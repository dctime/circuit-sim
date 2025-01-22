#pragma once

#include "Line.h"
#include <CurrentSourceElement.h>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <UICircuit.h>
#include <UIElement.h>
#include <cmath>
#include <iostream>

class UICircuit;

class CurrentSourceUIElement : public UIElement {
public:
  static void showGhostElement(sf::RenderWindow *window, int xGrid, int yGrid) {
    sf::Vector2f loc(xGrid * 50, yGrid * 50);
    CurrentSourceUIElement::showElement(window, loc, 0, 0);
  }

private:
  std::unique_ptr<CurrentSourceElement> element;

private:
  double i;
  UICircuit *uiCircuit;

public:
  CurrentSourceUIElement(UICircuit *circuit, int xGrid, int yGrid, double i) {
    this->i = i;
    this->xGrid = xGrid;
    this->yGrid = yGrid;
    uiCircuit = circuit;

    std::string pin1Loc =
        std::to_string(xGrid) + "," + std::to_string(yGrid - 1);
    std::string pin2Loc =
        std::to_string(xGrid) + "," + std::to_string(yGrid + 1);

    this->connectedLocs.push_back(pin1Loc);
    this->connectedLocs.push_back(pin2Loc);

    std::cout << "Current Source Init:" << std::endl;
    std::cout << "  Pin1Loc: " << pin1Loc << std::endl;
    std::cout << "  Pin2Loc: " << pin2Loc << std::endl;
  }

  void showElement(sf::RenderWindow *window) override {
    if (element.get() == nullptr || uiCircuit->getDisplayCircuit() == nullptr) {
      // show Ghost Version
      CurrentSourceUIElement::showGhostElement(window, xGrid, yGrid);
    } else {
      double pin1Volt = 0;
      double pin2Volt = 0;
      if (element->getPin1() != -1) {
        pin1Volt = *uiCircuit->getDisplayCircuit()->getVoltagePointer(
            element->getPin1());
      }

      if (element->getPin2() != -1) {
        pin2Volt = *uiCircuit->getDisplayCircuit()->getVoltagePointer(
            element->getPin2());
      }

      showCurrentSource(window, pin1Volt, pin2Volt, i, xGrid, yGrid,
                        uiCircuit->getCurrentScale());
    }
  }

  CircuitElement *getCircuitElementPointer() override {
    if (element.get() == nullptr) {
      std::string pin1Loc =
          std::to_string(xGrid) + "," + std::to_string(yGrid - 1);
      std::string pin2Loc =
          std::to_string(xGrid) + "," + std::to_string(yGrid + 1);

      element =
          CurrentSourceElement::create(i, uiCircuit->getIDfromLoc(pin1Loc),
                                       uiCircuit->getIDfromLoc(pin2Loc));
      std::cout << "Current Source Element Created!" << std::endl;
      std::cout << "Current Source UI Element added to UI Circuit. ID: "
                << uiElementID << std::endl;
    }
    return element.get();
  }

  void resetElement() override { element.reset(); }

public:
  ~CurrentSourceUIElement() override {};

private:
  double lastoffset = 0;
  static void showElement(sf::RenderWindow *window, sf::Vector2f &loc,
                          double v1, double v2) {
    double width = 5;

    sf::CircleShape circle(25);
    circle.setFillColor(sf::Color(0, 0, 0));
    circle.setOutlineThickness(width);
    circle.setOutlineColor(sf::Color(30, 30, 30));

    circle.setPosition(loc.x - circle.getRadius(), loc.y - circle.getRadius());

    sf::Vector2f point11(loc.x, loc.y - 50);
    sf::Vector2f point12(loc.x, loc.y - 25);
    sf::Vector2f point21(loc.x, loc.y + 50);
    sf::Vector2f point22(loc.x, loc.y + 25);
    sf::Vector2f pointC1(loc.x, loc.y - 20);
    sf::Vector2f pointC2(loc.x, loc.y + 20);
    sf::Vector2f pointC11(loc.x + 10, loc.y - 10);
    sf::Vector2f pointC12(loc.x - 10, loc.y - 10);
    // sf::Vector2f pointADD1(loc.x - 10, loc.y - 10);
    // sf::Vector2f pointADD2(loc.x + 10, loc.y - 10);
    // sf::Vector2f pointADD3(loc.x, loc.y - 10 + 10);
    // sf::Vector2f pointADD4(loc.x, loc.y - 10 - 10);
    // sf::Vector2f pointMINUS1(loc.x - 10, loc.y + 12.5);
    // sf::Vector2f pointMINUS2(loc.x + 10, loc.y + 12.5);

    sf::Color color1;
    voltToColor(v1, color1);
    sf::Color color2;
    voltToColor(v2, color2);

    showLine(window, point11, point12, width, color1, color1, color1);
    showLine(window, point21, point22, width, color2, color2, color2);

    window->draw(circle);

    showLine(window, pointC1, pointC2, width, sf::Color(30, 30, 30),
             sf::Color(30, 30, 30), sf::Color(30, 30, 30));
    showLine(window, pointC1, pointC11, width, sf::Color(30, 30, 30),
             sf::Color(30, 30, 30), sf::Color(30, 30, 30));
    showLine(window, pointC1, pointC12, width, sf::Color(30, 30, 30),
             sf::Color(30, 30, 30), sf::Color(30, 30, 30));
  }
  void showCurrentSource(sf::RenderWindow *window, double v1, double v2,
                         double i, sf::Vector2f &loc, double currentScale) {
    showElement(window, loc, v1, v2);
    sf::Vector2f pointP1(loc.x, loc.y - 50);

    double current = -i;
    lastoffset += current * currentScale;
    lastoffset = std::remainder(lastoffset, 20);
    lastoffset = lastoffset < 0 ? lastoffset + 20.0 : lastoffset;
    for (int index = 0; index < 5; ++index) {
      sf::CircleShape circle(5.0 / 2);
      circle.setPosition(pointP1.x - circle.getRadius(),
                         pointP1.y + 20 * index - circle.getRadius() + lastoffset);
      circle.setFillColor(sf::Color(200, 200, 0));
      window->draw(circle);
    }
  }

  void showCurrentSource(sf::RenderWindow *window, double v1, double v2,
                         double i, int xGrid, int yGrid, double currentScale) {
    sf::Vector2f loc(xGrid * 50, yGrid * 50);
    showCurrentSource(window, v1, v2, i, loc, currentScale);
  }
};
