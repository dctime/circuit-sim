#pragma once
#include <Circuit.h>
#include <UIElement.h>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class UICircuit {
public:
  Circuit* getCircuit() {
    return circuit.get();
  }

  double getCurrentScale() {
    return currentScale;
  }
  
  int getMaxNodeID() {
    return nextPinID - 1;
  }
private:
  std::vector<std::unique_ptr<UIElement>> uiElements;
  std::unique_ptr<Circuit> circuit;
  std::unordered_map<std::string, int> locToPinID;
  int nextPinID = 0;
  double currentScale = 5000;

  // infos
public:
  void printOutLocToPinID() {
    for (const auto &pair : locToPinID) {
      std::cout << pair.first << ": " << pair.second << std::endl;
    }
    std::cout << "Max Node ID: " << nextPinID - 1 << std::endl;
    std::cout << "Voltage Source Count: " << getVoltageSourceCount() << std::endl;
  }

public:
  double GROUND = 0;

  void showCircuit(sf::RenderWindow *window) {
    for (std::unique_ptr<UIElement> &uiElement : uiElements) {
      uiElement->showElement(window);
    }
  }

  // run this every frame
  void runCircuit() {
    if (circuit.get() == nullptr)
      buildCircuit();

    circuit->incTimerByDeltaT();
    for (int iteration = 0; iteration < 10; iteration++) {
      // std::cout << "========================" << std::endl;
      // std::cout << "iteration: " << iteration << std::endl;

      circuit->iterate();
    }
  }

private:
  void buildCircuit() {
    // FIXME: Reset every circuit element in UI Elements
    // FIXME: A reset method virtual to reset
    nextPinID = 0;
    for (std::pair<std::string, int> pair : locToPinID) {
      if (pair.second == 0) {
        locToPinID[pair.first] = nextPinID;
        nextPinID++;
      } 
    }
    std::vector<CircuitElement *> elements;
    for (std::unique_ptr<UIElement> &uiElement : uiElements) {
      CircuitElement *circuitElement = uiElement->getCircuitElementPointer(this);
      if (circuitElement == nullptr)
        continue;
      elements.push_back(circuitElement);
    }
    
    circuit = Circuit::create(elements, 0.01, nextPinID-1);
    
    printOutLocToPinID();
  }

public:
  void addElement(std::unique_ptr<UIElement> &uiElement) {
    circuit.reset();
    for (std::string &loc : uiElement.get()->getConnectedLocs()) {
      if (!locToPinID.count(loc)) {
        locToPinID[loc] = 0;
      }
    }

    for (std::string &loc : uiElement.get()->getGroundedLocs()) {
      locToPinID[loc] = -1;
    }

    uiElements.push_back(std::move(uiElement));
  }

public:
  double *getCurrentScalePointer() { return &currentScale; }

  int getIDfromLoc(std::string &loc) { return locToPinID[loc]; }

private:
  int voltageSourceID = 0;

public:
  int getNextVoltageSourceID() {
    int returnID = voltageSourceID;
    voltageSourceID++;
    return returnID;
  }

  int getVoltageSourceCount() {
    return voltageSourceID;
  }
};
