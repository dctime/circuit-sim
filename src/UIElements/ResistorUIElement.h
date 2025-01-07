#pragma once
#include "Line.h"
#include "UIElement.h"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <ResistorElement.h>
#include <iostream>

class ResistorUIElement : public UIElement {
public:
  static void showGhostElement(sf::RenderWindow *window, int xGrid, int yGrid) {
    sf::Vector2f loc(xGrid*50, yGrid*50);
    ResistorUIElement::showResistor(window, 0, 0, loc); 
  }

private:
  double R;
  std::unique_ptr<ResistorElement> resistorElement; 

public:
  ResistorUIElement(int xGrid, int yGrid, double R) {
    this->R = R;
    this->xGrid = xGrid;
    this->yGrid = yGrid;

    std::string pin1Loc = std::to_string(xGrid) + "," + std::to_string(yGrid-1);
    std::string pin2Loc = std::to_string(xGrid) + "," + std::to_string(yGrid+1);

    this->connectedLocs.push_back(pin1Loc);
    this->connectedLocs.push_back(pin2Loc);

    std::cout << "Resistor Added to UI Circuit: " << std::endl;
    std::cout << "  Pin1Loc: " << pin1Loc << std::endl;
    std::cout << "  Pin2Loc: " << pin2Loc << std::endl;
  }

  CircuitElement * getCircuitElementPointer() override {
    return resistorElement.get();
  }

  void showElement(sf::RenderWindow *window) override {
    // showResistor(window, *v1, *v2, xGrid, yGrid, R, *currentScale);
  }

public:
  ~ResistorUIElement() override {};
  double lastoffsetresistor = 0;
  

private:
  // show current using xGrid yGrid
  void showResistor(sf::RenderWindow *window, double v1, double v2, int xGrid,
                    int yGrid, double R, double currentScale) {
    sf::Vector2f loc(xGrid * 50, yGrid * 50);
    showResistor(window, v1, v2, loc, R, currentScale);
  }

  // without current indicator
  static void showResistor(sf::RenderWindow *window, double v1, double v2,
                    sf::Vector2f &loc) {
    // pin1 302.5 300
    // pin2 302.5 400
    // loc 302.5 350
    sf::VertexArray bg(sf::TriangleStrip, 12);
    bg[0].position = sf::Vector2f(loc.x - 2.5, loc.y - 50);
    bg[1].position = sf::Vector2f(loc.x + 2.5, loc.y - 50);
    bg[2].position = sf::Vector2f(loc.x - 2.5, loc.y - 10);
    bg[3].position = sf::Vector2f(loc.x + 2.5, loc.y - 10);
    bg[4].position = sf::Vector2f(loc.x - 7.5, loc.y - 5);
    bg[5].position = sf::Vector2f(loc.x - 2.5, loc.y - 5);
    bg[6].position = sf::Vector2f(loc.x + 2.5, loc.y + 5);
    bg[7].position = sf::Vector2f(loc.x + 7.5, loc.y + 5);
    bg[8].position = sf::Vector2f(loc.x - 2.5, loc.y + 10);
    bg[9].position = sf::Vector2f(loc.x + 2.5, loc.y + 10);
    bg[10].position = sf::Vector2f(loc.x - 2.5, loc.y + 50);
    bg[11].position = sf::Vector2f(loc.x + 2.5, loc.y + 50);

    sf::Color color1;
    voltToColor(v1, color1);
    sf::Color color2;
    voltToColor(v2, color2);
    sf::Color mColor;
    sf::Color m1Color;
    sf::Color m2Color;
    midColor(mColor, color1, color2);
    midColor(m1Color, color1, mColor);
    midColor(m2Color, mColor, color2);

    bg[0].color = color1;
    bg[1].color = color1;
    bg[2].color = color1;
    bg[3].color = color1;
    bg[4].color = m1Color;
    bg[5].color = m1Color;
    bg[6].color = m2Color;
    bg[7].color = m2Color;
    bg[8].color = color2;
    bg[9].color = color2;
    bg[10].color = color2;
    bg[11].color = color2;

    window->draw(bg);
  }

  // with indicator
  void showResistor(sf::RenderWindow *window, double v1, double v2,
                    sf::Vector2f &loc, double R, double currentScale) {
    showResistor(window, v1, v2, loc);

    double current = (v1 - v2) / R;

    lastoffsetresistor += current * currentScale;
    lastoffsetresistor = std::remainder(lastoffsetresistor, 20);
    lastoffsetresistor =
        lastoffsetresistor < 0 ? lastoffsetresistor + 20.0 : lastoffsetresistor;

    for (int i = 0; i < 5; ++i) {
      sf::CircleShape circle(5.0 / 2);
      circle.setPosition(loc.x - circle.getRadius(), loc.y - 50 + 20 * i -
                                                         circle.getRadius() +
                                                         lastoffsetresistor);
      circle.setFillColor(sf::Color(200, 200, 0));
      window->draw(circle);
    }
  }
};
