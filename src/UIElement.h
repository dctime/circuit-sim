#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
class UIElement {

public:
  virtual ~UIElement() {

  };
  
  virtual void showElement(sf::RenderWindow* window, int xGrid, int yGrid) {};
};


