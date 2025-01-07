#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include <CircuitElement.h>

class UICircuit;

class UIElement {
protected:
  
  int xGrid;
  int yGrid;
  std::vector<std::string> connectedLocs;
  std::vector<std::string> groundedLocs;
  UICircuit* uiCircuit;

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
  // Called when UICircuit is building himself
  // Will Rewrite uiCircuit
  virtual CircuitElement* getCircuitElementPointer(UICircuit* circuit) = 0;
  // virtual void showUnCalculated() = 0;
};


