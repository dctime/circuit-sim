#pragma once
#include <Circuit.h>
#include <UIElement.h>
#include <iostream>
#include <memory>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

class UICircuit {
public:
  Circuit *getCircuit() { return circuit.get(); }

  double getCurrentScale() { return currentScale; }

  double getTime() {
    if (circuit.get() != nullptr) {
      return circuit->getTime();
    } else {
      return 0;
    }
  }

  int getMaxNodeID() { return nextPinID - 1; }

private:
  std::vector<std::unique_ptr<UIElement>> uiElements;
  std::unique_ptr<Circuit> circuit;
  std::unordered_map<std::string, int> locToPinID;
  int nextPinID = 0;
  double currentScale = 1000;

  // infos
public:
  void printOutLocToPinID() {
    std::cout << "xGrid, yGrid : PIN ID" << std::endl;
    for (const auto &pair : locToPinID) {
      std::cout << pair.first << ": " << pair.second << std::endl;
    }
    std::cout << "Max Node ID: " << nextPinID - 1 << std::endl;
    std::cout << "Max UIElement ID: " << nextUIElementID - 1 << std::endl;
    std::cout << "Voltage Source Count: " << getVoltageSourceCount()
              << std::endl;
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
    if (circuit.get() == nullptr) {
      if (!buildCircuit()) {
        for (std::unique_ptr<UIElement> &uiElement : uiElements) {
          uiElement->resetElement();
        }
        return;
      }
    }

    circuit->incTimerByDeltaT();

    bool passed = false;
    bool hasOscillation = false;

    int MAX_ITERATION = 50;

    int iteration = 1;
    // must be greater than 1
    // the greater the faster iterations slows down the iteration
    // osc -> iteration not doing its job -> drag too much
    // giving too much drag will cause oscillation
    //
    double iterationDrag = 1.1;
    while (true) {
      circuit->iterate(iteration, iterationDrag, &passed, &hasOscillation);

      if (hasOscillation) {
        // std::cout << "Oscillation occurs" << std::endl;
        iterationDrag = 1.1;
        hasOscillation = false;
      } else {
        // std::cout << "adding Drag to: " << iterationDrag << std::endl;
        iterationDrag += 0.1;
      }

      if (passed) {
        break;
      }

      if (iteration >= MAX_ITERATION) {
        break;
      }

      iteration++;
    }

    // std::cout << "Used Iterations: " << iteration << std::endl;
  }

private:
  std::unordered_map<int, UIElement *> uiElementIDToUIElement;

public:
  UIElement *getUIElement(int id) { return uiElementIDToUIElement[id]; }

private:
  bool buildCircuit() {
    std::cout << "Rebuild Circuit" << std::endl;
    std::unordered_map<std::string, int> locToPinID;
    for (std::unique_ptr<UIElement> &uiElement : uiElements) {
      for (std::string &loc : uiElement.get()->getConnectedLocs()) {
        if (!locToPinID.count(loc)) {
          locToPinID[loc] = 0;
        }
      }

      for (std::string &loc : uiElement.get()->getGroundedLocs()) {
        locToPinID[loc] = -1;
      }
    }

    nextPinID = 0;
    for (std::pair<std::string, int> pair : locToPinID) {
      if (pair.second == 0) {
        locToPinID[pair.first] = nextPinID;
        nextPinID++;
      }
    }
    this->locToPinID = locToPinID;

    std::vector<CircuitElement *> elements;
    voltageSourceID = 0;
    for (std::unique_ptr<UIElement> &uiElement : uiElements) {
      // creates circuit element
      // voltage source ID wil inc
      CircuitElement *circuitElement = uiElement->getCircuitElementPointer();
      if (circuitElement == nullptr)
        continue;
      elements.push_back(circuitElement);
    }

    printOutLocToPinID();

    if (!checkIfAllGoToGND(nextPinID - 1)) {
      return false;
    }

    circuit = Circuit::create(elements, 0.01, nextPinID - 1);
    return true;
  }

private:
  bool checkIfAllGoToGND(int MAX_NODE_ID) {
    std::cout << "Checking Circuit: No Floating Nodes Test!" << std::endl;

    std::unordered_map<int, std::string> pinIDToLoc;
    for (std::pair<std::string, int> pair : locToPinID) {
      pinIDToLoc[pair.second] = pair.first;
    }
    // locToPinID and pinIDToLoc ready. is it valid?

    struct Node {
      int id;
      std::vector<int> connectedID;
      Node(int id) { this->id = id; }
    };

    struct Graph {
      std::vector<std::unique_ptr<Node>> nodes;
      void addNode(std::unique_ptr<Node> &node) {
        nodes.push_back(std::move(node));
      }

      Node *getNodeFromID(int id) {
        Node *returnNode = nullptr;
        for (std::unique_ptr<Node> &node : nodes) {
          if (node->id == id) {
            returnNode = node.get();
          }
        }
        return returnNode;
      }

      void addNeighborToNode(int targetID, int neighborID) {
        Node *targetNode = getNodeFromID(targetID);
        if (targetNode == nullptr) {
          std::cout << "TargetID:" << targetID << " Doesnt Exist!" << std::endl;
          return;
        }

        Node *neighborNode = getNodeFromID(neighborID);
        if (targetNode == nullptr) {
          std::cout << "NeighborID:" << neighborID << " Doesnt Exist!"
                    << std::endl;
          return;
        }

        for (int &id : targetNode->connectedID) {
          if (id == neighborID) {
            std::cout << "NeighborID: " << neighborID
                      << "is already a neighbor!" << std::endl;
            return;
          }
        }

        targetNode->connectedID.push_back(neighborID);
      }
    };

    Graph graph;

    // init nodes in graph
    for (std::pair<std::string, int> pair : locToPinID) {
      std::unique_ptr<Node> node = std::make_unique<Node>(pair.second);
      graph.addNode(node);
    }

    // turning uiElements getConnectedLocs and getNoCurrentLocs into neighbors
    // ID
    for (std::unique_ptr<UIElement> &uiElement : uiElements) {
      std::vector<int> allConnectsID;
      std::vector<int> allConnectsID2;
      for (std::string &loc : uiElement->getConnectedLocs()) {
        bool isNoCurrentLoc = false;
        std::vector<std::string> noCurrentLocs = uiElement->getNoCurrentLocs();
        if (noCurrentLocs.size() >= 1 && loc == noCurrentLocs.at(0)) {
          isNoCurrentLoc = true;
        }
        if (isNoCurrentLoc) {
          allConnectsID2.push_back(locToPinID[loc]);
        } else {
          allConnectsID.push_back(locToPinID[loc]);
        }
      }

      // allConnects Ready
      for (int targetId : allConnectsID) {
        for (int neighborId : allConnectsID) {
          if (targetId == neighborId)
            continue;
          graph.addNeighborToNode(targetId, neighborId);
        }
      }

      for (int targetId : allConnectsID2) {
        for (int neighborId : allConnectsID2) {
          if (targetId == neighborId)
            continue;
          graph.addNeighborToNode(targetId, neighborId);
        }
      }
    }

    // graph is ready
    std::stack<int> stack;
    std::vector<int> IDVisitedStatus;

    for (int i = 0; i <= MAX_NODE_ID; i++) {
      IDVisitedStatus.push_back(0);
    }

    // check if circuit is just full of ground
    if (MAX_NODE_ID == 0) {
      std::cout << "Only Grounds exists in this circuit! Passed" << std::endl;
      return true;
    }

    while (true) {
      // decided which id should be pushed
      int pushedID = -1;
      for (int id = 0; id < IDVisitedStatus.size(); id++) {
        if (IDVisitedStatus.at(id) == 0) {
          pushedID = id;
          break;
        }
      }

      // check if there is nothing to push
      if (pushedID == -1) {
        std::cout << "Nothing To Push! All elements are not floating! Passed!"
                  << std::endl;
        return true;
      }

      std::cout << "Pushed ID: " << pushedID << std::endl;
      stack.push(pushedID);

      bool metGround = false;
      while (!stack.empty()) {
        int targetID = stack.top();
        stack.pop();

        // check if met ground
        if (targetID == -1) {
          metGround = true;
          continue;
        }

        // check if visited
        if (IDVisitedStatus.at(targetID) == 1) {
          continue;
        }

        // add into visited
        IDVisitedStatus.at(targetID) = 1;

        Node *targetNode = graph.getNodeFromID(targetID);
        for (int aConnectedID : targetNode->connectedID) {
          stack.push(aConnectedID);
        }
      }

      // if not met ground
      if (!metGround) {
        std::cout << "Error! Some Circuit Elements Does not have a path to "
                     "reach ground."
                  << std::endl;
        return false;
      }
    }
  }

private:
  int nextUIElementID = 0;

  void resetCircuit() {
    std::cout << "Circuit Reset!" << std::endl;
    circuit.reset();
    for (std::unique_ptr<UIElement> &uiElement : uiElements) {
      uiElement->resetElement();
    }
  }

public:
  void addElement(std::unique_ptr<UIElement> &uiElement) {
    resetCircuit();
    uiElement->setUIElementID(nextUIElementID);
    uiElementIDToUIElement[nextUIElementID] = uiElement.get();
    nextUIElementID++;

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

  int getVoltageSourceCount() { return voltageSourceID; }
};
