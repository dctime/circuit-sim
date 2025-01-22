#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include <CircuitElement.h>

class UICircuit;

class UIElement {
friend UICircuit;
protected:
  
  int xGrid;
  int yGrid;
  std::vector<std::string> connectedLocs;
  std::vector<std::string> groundedLocs;
  std::vector<std::string> noCurrentLocs;
  UICircuit* uiCircuit;
public:
  // reset the element stored in UI Element;
  virtual void resetElement() = 0;
public:

  UIElement() {};
  std::vector<std::string> getConnectedLocs() {
    return connectedLocs;  
  }

  std::vector<std::string> getGroundedLocs() {
    return groundedLocs;
  }

  std::vector<std::string> getNoCurrentLocs() {
    return noCurrentLocs;
  }

  virtual ~UIElement() {

  };
  
  virtual void showElement(sf::RenderWindow* window) = 0;

  // if there is no circuit element return nullptr
  // Called when UICircuit is building himself
  // Will Rewrite uiCircuit
  virtual CircuitElement* getCircuitElementPointer() = 0;

protected:
  int uiElementID;
public:
  void setUIElementID(int id) {
    uiElementID = id;
  }
};


