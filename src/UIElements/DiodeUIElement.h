#pragma once
#include "Line.h"
#include "UIElement.h"
#include <DiodeElement.h>
#include <SFML/Graphics/RenderWindow.hpp>
#include <UICircuit.h>

class DiodeUIElement : public UIElement {
public:
  void resetElement() override { element.reset(); }

private:
  UICircuit *uiCircuit;
  int xGrid, yGrid;
  double Is, Vt;
  std::unique_ptr<DiodeElement> element;

public:
  DiodeUIElement(UICircuit *circuit, int xGrid, int yGrid, double Is, double Vt) {
    uiCircuit = circuit;
    this->xGrid = xGrid;
    this->yGrid = yGrid;
    this->Is = Is;
    this->Vt = Vt;
    uiElementID = circuit->getUIElementIDForUIElement((UIElement *)this);

    std::string pin1Loc =
        std::to_string(xGrid) + "," + std::to_string(yGrid - 1);
    std::string pinMLoc = "E" + std::to_string(uiElementID) + "PinM";
    std::string pin2Loc =
        std::to_string(xGrid) + "," + std::to_string(yGrid + 1);

    this->connectedLocs.push_back(pin1Loc);
    this->connectedLocs.push_back(pinMLoc);
    this->connectedLocs.push_back(pin2Loc);

    std::cout << "Diode Init: " << std::endl;
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

      element = DiodeElement::create(Is, Vt, uiCircuit->getIDfromLoc(pin1Loc),
                                        uiCircuit->getIDfromLoc(pinMLoc),
                                        uiCircuit->getIDfromLoc(pin2Loc),
                                        uiCircuit->getNextVoltageSourceID());

      std::cout << "Diode Element Created!" << std::endl;
      std::cout << "Diode UI Element added to UI Circuit. ID: "
                << uiElementID << std::endl;
    }

    return element.get();
  }
  void showElement(sf::RenderWindow *window) override {
    if (element.get() == nullptr || uiCircuit->getDisplayCircuit() == nullptr) {
      DiodeUIElement::showGhostElement(window, xGrid, yGrid);
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

      showDiode(window, pin1Volt, pin2Volt, i, xGrid, yGrid,
                   uiCircuit->getCurrentScale());
    }
  }

private:
  static void showDiode(sf::RenderWindow *window, sf::Vector2f &loc,
                           double v1, double v2) {
    double width = 5;

    sf::Vector2f pointP11(loc.x, loc.y-50);
    sf::Vector2f pointP12(loc.x, loc.y-20);
    sf::Vector2f pointP13(loc.x-20, loc.y-20);
    sf::Vector2f pointP14(loc.x+20, loc.y-20);
    sf::Vector2f pointP24(loc.x+20, loc.y+20);
    sf::Vector2f pointP23(loc.x-20, loc.y+20);
    sf::Vector2f pointP22(loc.x, loc.y+20);
    sf::Vector2f pointP21(loc.x, loc.y+50);

    sf::Color color1;
    voltToColor(v1, color1);
    sf::Color color2;
    voltToColor(v2, color2);
    sf::Color colorM;
    midColor(colorM, color1, color2);
    
    showLine(window, pointP11, pointP12, width, color1, color1, color1);
    showLine(window, pointP12, pointP13, width, color1, color1, color1);
    showLine(window, pointP12, pointP14, width, color1, color1, color1);
    showLine(window, pointP21, pointP22, width, color2, color2, color2);
    showLine(window, pointP22, pointP23, width, color2, color2, color2);
    showLine(window, pointP22, pointP24, width, color2, color2, color2);
    showLine(window, pointP13, pointP22, width, color1, color2, colorM);
    showLine(window, pointP14, pointP22, width, color1, color2, colorM);
  }

  double lastoffset = 0;
  void showDiode(sf::RenderWindow *window, double v1, double v2, double i,
                    sf::Vector2f &loc, double currentScale) {
    DiodeUIElement::showDiode(window, loc, v1, v2);

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
  void showDiode(sf::RenderWindow *window, double v1, double v2, double i,
                    int xGrid, int yGrid, double currentScale) {
    sf::Vector2f loc(xGrid * 50, yGrid * 50);
    showDiode(window, v1, v2, i, loc, currentScale);
  }

private:
  static void showGhostElement(sf::RenderWindow *window, int xGrid, int yGrid) {
    sf::Vector2f loc(xGrid * 50, yGrid * 50);
    DiodeUIElement::showDiode(window, loc, 0, 0);
  }
};
