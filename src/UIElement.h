#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include <CircuitElement.h>

class UIElement {
protected:
  
  int xGrid;
  int yGrid;
  std::vector<std::string> connectedLocs;
  std::vector<std::string> groundedLocs;

public:

  UIElement() {};
  std::vector<std::string> getConnectedLocs() {
    return connectedLocs;  
  }

  std::vector<std::string> getGroundedLocs() {
    return groundedLocs;
  }

  virtual ~UIElement() {

  };
  
  virtual void showElement(sf::RenderWindow* window) = 0;

  // if there is no circuit element return nullptr
  virtual CircuitElement* getCircuitElementPointer() = 0;
  // virtual void showUnCalculated() = 0;
};


