#pragma once
#include "Line.h"
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include "UIElement.h"

class ResistorUIElement : public UIElement {
private:
  double* v1;
  double* v2;
  double R;
  double* currentScale;
public:
  ResistorUIElement() {};
  ResistorUIElement(double* v1, double* v2, double R, double* currentScale) {
    this->v1 = v1;
    this->v2 = v2;
    this->R = R;
    this->currentScale = currentScale;
  }

  void showElement(sf::RenderWindow *window, int xGrid, int yGrid) override {
    showResistor(window, *v1, *v2, xGrid, yGrid, R, *currentScale);
  }
public:
  ~ResistorUIElement() override {};
  double lastoffsetresistor = 0; 
  void showResistor(sf::RenderWindow *window, int xGrid, int yGrid) {
    sf::Vector2f loc(xGrid * 50, yGrid * 50);
    showResistor(window, 0, 0, loc, 1000, 0, false);
  }
private:

  void showResistor(sf::RenderWindow *window, double v1, double v2, int xGrid,
                    int yGrid, double R, double currentScale) {
    sf::Vector2f loc(xGrid * 50, yGrid * 50);
    showResistor(window, v1, v2, loc, R, currentScale, true);
  }

  void showResistor(sf::RenderWindow *window, double v1, double v2,
                    sf::Vector2f &loc, double R, double currentScale,
                    bool showCurrent) {
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

    double current = (v1 - v2) / R;

    lastoffsetresistor += current * currentScale;
    lastoffsetresistor = std::remainder(lastoffsetresistor, 20);
    lastoffsetresistor =
        lastoffsetresistor < 0 ? lastoffsetresistor + 20.0 : lastoffsetresistor;

    if (showCurrent) {
      for (int i = 0; i < 5; ++i) {
        sf::CircleShape circle(5.0 / 2);
        circle.setPosition(loc.x - circle.getRadius(), loc.y - 50 + 20 * i -
                                                           circle.getRadius() +
                                                           lastoffsetresistor);
        circle.setFillColor(sf::Color(200, 200, 0));
        window->draw(circle);
      }
    }
  }
};
