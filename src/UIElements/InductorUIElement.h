#pragma once
#include "Line.h"
#include "UIElement.h"
#include <InductorElement.h>
#include <SFML/Graphics/RenderWindow.hpp>
#include <UICircuit.h>

class InductorUIElement : public UIElement {
public:
  void resetElement() override { element.reset(); }

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
    uiElementID = circuit->getUIElementIDForUIElement((UIElement *)this);

    std::string pin1Loc =
        std::to_string(xGrid) + "," + std::to_string(yGrid - 1);
    std::string pinMLoc = "E" + std::to_string(uiElementID) + "PinM";
    std::string pin2Loc =
        std::to_string(xGrid) + "," + std::to_string(yGrid + 1);

    this->connectedLocs.push_back(pin1Loc);
    this->connectedLocs.push_back(pinMLoc);
    this->connectedLocs.push_back(pin2Loc);

    std::cout << "Inductor Init: " << std::endl;
    std::cout << "  Pin1Loc: " << pin1Loc << std::endl;
    std::cout << "  PinMLoc (0 Volt Voltage Source): " << pinMLoc << std::endl;
    std::cout << "  Pin2Loc: " << pin2Loc << std::endl;
  }

  CircuitElement *getCircuitElementPointer() override {
    if (element.get() == nullptr) {
      std::string pin1Loc =
          std::to_string(xGrid) + "," + std::to_string(yGrid - 1);
      std::string pinMLoc = "E" + std::to_string(uiElementID) + "PinM";
      std::string pin2Loc =
          std::to_string(xGrid) + "," + std::to_string(yGrid + 1);

      element = InductorElement::create(L, 0, uiCircuit->getIDfromLoc(pin1Loc),
                                        uiCircuit->getIDfromLoc(pinMLoc),
                                        uiCircuit->getIDfromLoc(pin2Loc),
                                        uiCircuit->getNextVoltageSourceID());

      std::cout << "Inductor Element Created!" << std::endl;
      std::cout << "Inductor UI Element added to UI Circuit. ID: "
                << uiElementID << std::endl;
    }

    return element.get();
  }
  void showElement(sf::RenderWindow *window) override {
    if (element.get() == nullptr || uiCircuit->getDisplayCircuit() == nullptr) {
      InductorUIElement::showGhostElement(window, xGrid, yGrid);
    } else {
      double pin1Volt = 0;
      double pin2Volt = 0;
      double prePin1Volt = 0;
      double prePin2Volt = 0;
      if (element->getPIN1() != -1) {
        pin1Volt = *uiCircuit->getDisplayCircuit()->getVoltagePointer(
            element->getPIN1());
        prePin1Volt = *uiCircuit->getDisplayCircuit()->getPreVoltagePointer(
            element->getPIN1());
      }

      if (element->getPIN2() != -1) {
        pin2Volt = *uiCircuit->getDisplayCircuit()->getVoltagePointer(
            element->getPIN2());
        prePin2Volt = *uiCircuit->getDisplayCircuit()->getPreVoltagePointer(
            element->getPIN2());
      }

      // std::cout << "CID:" << uiCircuit->getDisplayCircuit()->getCircuitID()
      // << "V1-V2:" << pin1Volt - pin2Volt
      // << "element:" << prePin1Volt - prePin2Volt << std::endl;

      double i = *uiCircuit->getDisplayCircuit()->getVoltagePointer(
          uiCircuit->getMaxNodeID() + element->getVoltageSourceID() + 1);

      showInductor(window, pin1Volt, pin2Volt, i, xGrid, yGrid,
                   uiCircuit->getCurrentScale());
    }
  }

private:
  static void showInductor(sf::RenderWindow *window, sf::Vector2f &loc,
                           double v1, double v2) {
    double width = 5;

    sf::Vector2f pointP11(loc.x, loc.y-50);
    sf::Vector2f pointP12(loc.x, loc.y-40);
    sf::Vector2f pointP13(loc.x+20, loc.y-30);
    sf::Vector2f pointPC11(loc.x+20, loc.y-20);
    sf::Vector2f pointPC12(loc.x, loc.y-10);
    sf::Vector2f pointPC13(loc.x-20, loc.y-20);
    sf::Vector2f pointPC14(loc.x, loc.y-30);
    sf::Vector2f pointPC15(loc.x+20, loc.y-10);
    sf::Vector2f pointPC21(loc.x+20, loc.y+10);
    sf::Vector2f pointPC22(loc.x, loc.y+20);
    sf::Vector2f pointPC23(loc.x-20, loc.y+10);
    sf::Vector2f pointPC24(loc.x, loc.y);
    sf::Vector2f pointPC25(loc.x+20, loc.y+20);
    sf::Vector2f pointP23(loc.x+20, loc.y+30);
    sf::Vector2f pointP22(loc.x, loc.y+40);
    sf::Vector2f pointP21(loc.x, loc.y+50);

    sf::Color color1;
    voltToColor(v1, color1);
    sf::Color color2;
    voltToColor(v2, color2);
    sf::Color colorPC1;
    sf::Color colorPC2;
    sf::Color colorPCM;
    midColor(colorPCM, color1, color2);
    midColor(colorPC1, color1, colorPCM);
    midColor(colorPC2, color2, colorPCM);
    
    showLine(window, pointP11, pointP12, width, color1, color1, color1);
    showLine(window, pointP12, pointP13, width, color1, color1, color1);
    showLine(window, pointP13, pointPC11, width, color1, color1, color1);
    showLine(window, pointPC11, pointPC12, width, color1, colorPC1, color1);
    showLine(window, pointPC12, pointPC13, width, colorPC1, colorPC1, colorPC1);
    showLine(window, pointPC13, pointPC14, width, colorPC1, colorPC1, colorPC1);
    showLine(window, pointPC14, pointPC15, width, colorPC1, colorPC1, colorPCM);
    showLine(window, pointPC15, pointPC21, width, colorPCM, colorPC2, colorPC2);
    showLine(window, pointPC21, pointPC22, width, colorPC2, colorPC2, colorPC2);
    showLine(window, pointPC22, pointPC23, width, colorPC2, colorPC2, colorPC2);
    showLine(window, pointPC23, pointPC24, width, colorPC2, colorPC2, colorPC2);
    showLine(window, pointPC24, pointPC25, width, colorPC2, color2, color2);
    showLine(window, pointPC25, pointP23, width, color2, color2, color2);
    showLine(window, pointP23, pointP22, width, color2, color2, color2);
    showLine(window, pointP22, pointP21, width, color2, color2, color2);
  }

  double lastoffset = 0;
  void showInductor(sf::RenderWindow *window, double v1, double v2, double i,
                    sf::Vector2f &loc, double currentScale) {
    InductorUIElement::showInductor(window, loc, v1, v2);

    sf::Vector2f point11(loc.x, loc.y - 50);
    double current = i;
    lastoffset += current * currentScale;
    lastoffset = std::remainder(lastoffset, 20);
    lastoffset = lastoffset < 0 ? lastoffset + 20.0 : lastoffset;
    for (int i = 0; i < 5; ++i) {
      sf::CircleShape circle(5.0 / 2);
      circle.setPosition(point11.x - circle.getRadius(),
                         point11.y + 20 * i - circle.getRadius() + lastoffset);
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
