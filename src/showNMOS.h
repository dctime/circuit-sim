#pragma once
#include <SFML/Graphics.hpp>
#include "Line.h"
double lastoffsetNMOS = 0;
void showNMOS(sf::RenderWindow *window, double vg, double vd, double vs,
              double id, sf::Vector2f &loc, double currentScale) {
  // pin_d 100 100
  // pin_g 0 150
  // pin_s 100 200
  // loc 100 150
  double width = 5;
  sf::Vector2f pointD1(loc.x, loc.y - 50);
  sf::Vector2f pointD2(loc.x, loc.y - 25);
  sf::Vector2f pointB1(loc.x - 50 + 2.5, loc.y - 25);
  sf::Vector2f pointB2(loc.x - 50 + 2.5, loc.y + 25);
  sf::Vector2f pointBG1(loc.x - 50 - 7.5, loc.y - 25);
  sf::Vector2f pointBG2(loc.x - 50 - 7.5, loc.y + 25);
  sf::Vector2f pointG1(loc.x - 50 - 7.5, loc.y);
  sf::Vector2f pointG2(loc.x - 100, loc.y);
  sf::Vector2f pointS1(loc.x, loc.y + 25);
  sf::Vector2f pointS11(loc.x - 10, loc.y + 25 + 10);
  sf::Vector2f pointS12(loc.x - 10, loc.y + 25 - 10);
  sf::Vector2f pointS2(loc.x, loc.y + 50);

  sf::Color gColor;
  voltToColor(vg, gColor);
  sf::Color dColor;
  voltToColor(vd, dColor);
  sf::Color sColor;
  voltToColor(vs, sColor);
  sf::Color bColor;
  midColor(bColor, dColor, sColor);

  showLine(window, pointD1, pointD2, 5, dColor, dColor, dColor);
  showLine(window, pointB1, pointB2, width, dColor, sColor, bColor);
  showLine(window, pointBG1, pointBG2, width, gColor, gColor, gColor);
  showLine(window, pointG1, pointG2, 5, gColor, gColor, gColor);
  showLine(window, pointS1, pointS2, 5, sColor, sColor, sColor);
  showLine(window, pointD2, pointB1, 5, dColor, dColor, dColor);
  showLine(window, pointB2, pointS1, 5, sColor, sColor, sColor);
  showLine(window, pointS1, pointS11, width, sColor, sColor, sColor);
  showLine(window, pointS1, pointS12, width, sColor, sColor, sColor);

  double &current = id;
  // std::cout << "id:" << id << std::endl;
  lastoffsetNMOS += current * currentScale;
  lastoffsetNMOS = std::remainder(lastoffsetNMOS, 20);
  lastoffsetNMOS = lastoffsetNMOS < 0 ? lastoffsetNMOS + 20.0 : lastoffsetNMOS;

  sf::CircleShape circle(width / 2);
  for (int i = 0; i < 2; ++i) {
    circle.setPosition(pointG1.x - circle.getRadius() - 20 * i,
                       pointG1.y - circle.getRadius());
    circle.setFillColor(sf::Color(200, 200, 0));
    window->draw(circle);
  }

  for (int i = 0; i < 20; ++i) {
    if (i < 1)
      circle.setPosition(pointD1.x - circle.getRadius(),
                         pointD1.y - circle.getRadius() + lastoffsetNMOS);
    else if (i < 2) {
      if (lastoffsetNMOS < 5)
        circle.setPosition(pointD1.x - circle.getRadius(),
                           pointD1.y + 20 - circle.getRadius() +
                               lastoffsetNMOS);
      else
        circle.setPosition(pointD2.x - circle.getRadius() - lastoffsetNMOS + 5,
                           pointD2.y - circle.getRadius());
    } else if (i < 3) {
      circle.setPosition(pointD2.x - circle.getRadius() - lastoffsetNMOS - 15,
                         pointD2.y - circle.getRadius());
    } else if (i < 4) {
      if (lastoffsetNMOS < 15)
        circle.setPosition(pointD2.x - circle.getRadius() - lastoffsetNMOS - 35,
                           pointD2.y - circle.getRadius());
      else
        circle.setPosition(pointB1.x - circle.getRadius(),
                           pointB1.y - circle.getRadius() + lastoffsetNMOS -
                               15);
    } else if (i < 6) {
      circle.setPosition(pointB1.x - circle.getRadius(),
                         pointB1.y - circle.getRadius() + lastoffsetNMOS + 5 +
                             (i - 4) * 20);
    } else if (i < 7) {
      if (lastoffsetNMOS < 5)
        circle.setPosition(pointB1.x - circle.getRadius(),
                           pointB1.y - circle.getRadius() + lastoffsetNMOS +
                               45);
      else
        circle.setPosition(pointB2.x - circle.getRadius() + lastoffsetNMOS - 5,
                           pointB2.y - circle.getRadius());
    } else if (i < 8) {
      circle.setPosition(pointB2.x - circle.getRadius() + lastoffsetNMOS + 15,
                         pointB2.y - circle.getRadius());
    } else if (i < 9) {
      if (lastoffsetNMOS < 15)
        circle.setPosition(pointB2.x - circle.getRadius() + lastoffsetNMOS + 35,
                           pointB2.y - circle.getRadius());
      else
        circle.setPosition(pointS1.x - circle.getRadius(),
                           pointS1.y - circle.getRadius() - 15 +
                               lastoffsetNMOS);
    } else {
      circle.setPosition(pointS1.x - circle.getRadius(),
                         pointS1.y - circle.getRadius() + lastoffsetNMOS + 5);
    }
    circle.setFillColor(sf::Color(200, 200, 0));
    window->draw(circle);
  }
}
