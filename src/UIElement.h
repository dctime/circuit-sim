#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
class UIElement {
protected:
  int xGrid;
  int yGrid;
public:
  virtual ~UIElement() {

  };
  
  virtual void showElement(sf::RenderWindow* window) = 0;
};


