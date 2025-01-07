#pragma once
#include <UIElement.h>
#include <unordered_map>
#include <vector>
#include <memory>
#include <Circuit.h>
#include <string>
#include <iostream>

class UICircuit {
private:
  std::vector<std::unique_ptr<UIElement>> uiElements;
  std::unique_ptr<Circuit> circuit;
  std::unordered_map<std::string, int> locToPinID;
  int nextPinID = 0;

  double currentScale = 5000;

  void buildCircuit() {
    std::vector<CircuitElement*> elements;
    for (std::unique_ptr<UIElement>& uiElement : uiElements) {
      CircuitElement* circuitElement = uiElement->getCircuitElementPointer();
      if (circuitElement == nullptr) continue;
      elements.push_back(circuitElement);
    }
    circuit = Circuit::create(elements, 0.01, nextPinID-1);
  }
// infos
public:
  void printOutLocToPinID() {
    for (const auto& pair : locToPinID) { std::cout << pair.first << ": " << pair.second << std::endl; }
  }
public:
  double GROUND = 0;
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

  void addElement(std::unique_ptr<UIElement>& uiElement) {
    circuit.reset();
    for (std::string& loc : uiElement.get()->getConnectedLocs()) {
      if (!locToPinID.count(loc)) {
        locToPinID[loc] = nextPinID;
        nextPinID++;
      }
    }

    for (std::string& loc : uiElement.get()->getGroundedLocs()) {
      locToPinID[loc] = -1;
    }

    uiElements.push_back(std::move(uiElement));
  }

  double* getCurrentScalePointer() {
    return &currentScale;
  }

  int getIDfromLoc(std::string& loc) {
    return locToPinID[loc];
  }
private:
  int voltageSourceID = 0;
public:
  int getNextVoltageSourceID() {
    int returnID = voltageSourceID;
    voltageSourceID++;
    return returnID;
  }
};
