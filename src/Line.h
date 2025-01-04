#pragma once
#include <SFML/Graphics.hpp>
#include <cmath> 
void showLine(sf::RenderWindow *window, sf::Vector2f &point1,
              sf::Vector2f &point2, double width, sf::Color color1,
              sf::Color color2, sf::Color colorm) {
  sf::VertexArray background(sf::TriangleStrip, 6);
  float angle = std::atan2(point2.y - point1.y, point2.x - point1.x);
  // std::cout << "angle:" << angle << std::endl;
  background[0].position =
      sf::Vector2f(point1.x + (width / 2.0) * std::sin(angle),
                   point1.y - (width / 2.0) * std::cos(angle));
  background[1].position =
      sf::Vector2f(point1.x - (width / 2.0) * std::sin(angle),
                   point1.y + (width / 2.0) * std::cos(angle));
  background[2].position = sf::Vector2f(
      (point1.x + point2.x) / 2.0 + (width / 2.0) * std::sin(angle),
      (point1.y + point2.y) / 2.0 - (width / 2.0) * std::cos(angle));
  background[3].position = sf::Vector2f(
      (point1.x + point2.x) / 2.0 - (width / 2.0) * std::sin(angle),
      (point1.y + point2.y) / 2.0 + (width / 2.0) * std::cos(angle));
  background[4].position =
      sf::Vector2f(point2.x + (width / 2.0) * std::sin(angle),
                   point2.y - (width / 2.0) * std::cos(angle));
  background[5].position =
      sf::Vector2f(point2.x - (width / 2.0) * std::sin(angle),
                   point2.y + (width / 2.0) * std::cos(angle));

  background[0].color = color1;
  background[1].color = color1;
  background[2].color = colorm;
  background[3].color = colorm;
  background[4].color = color2;
  background[5].color = color2;

  window->draw(background);
}

void voltToColor(double voltage, sf::Color &color) {
  if (voltage >= 0) {
    if (voltage >= 1)
      voltage = 1;
    color.a = 255;
    color.r = 30;
    color.g = 30 + (255 - 30) * voltage;
  } else {
    if (voltage <= -1)
      voltage = -1;
    color.a = 255;
    color.r = 30 + (255 - 30) * voltage * (-1);
    color.g = 30;
    color.b = 30;
  }
}

void midColor(sf::Color &outputColor, sf::Color &color1, sf::Color &color2) {
  int color1Value = (color1.r - 30) - (color1.g - 30);
  int color2Value = (color2.r - 30) - (color2.g - 30);
  int midValue = (color1Value + color2Value) / 2;
  outputColor.a = 255;
  outputColor.r = 30;
  outputColor.g = 30;
  outputColor.b = 30;
  if (midValue >= 0) {
    outputColor.r += midValue;
  } else {
    outputColor.g += -midValue;
  }
}

void showGround(sf::RenderWindow *window, sf::Vector2f &loc) {
  double width = 5;
  sf::Vector2f pointG1(loc.x - 25, loc.y);
  sf::Vector2f pointG2(loc.x + 25, loc.y);
  sf::Vector2f pointG3(loc.x - 10, loc.y + 10);
  sf::Vector2f pointG4(loc.x + 10, loc.y + 10);

  showLine(window, pointG1, pointG2, width, sf::Color(30, 30, 30),
           sf::Color(30, 30, 30), sf::Color(30, 30, 30));
  showLine(window, pointG3, pointG4, width, sf::Color(30, 30, 30),
           sf::Color(30, 30, 30), sf::Color(30, 30, 30));
}

void showGround(sf::RenderWindow *window, int xGrid, int yGrid) {
  sf::Vector2f loc(xGrid*50, yGrid*50);
  showGround(window, loc);
}
