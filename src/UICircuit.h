#pragma once
#include <Circuit.h>
#include <UIElement.h>
#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <stack>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

class UICircuit {
public:
  UICircuit() {
    bufferWorker = std::thread(&UICircuit::calculatingBufferingCircuits, this);
  }

  ~UICircuit() {
    uiCircuitAlive = false;
    bufferWorker.join();
  }

private:
  std::thread bufferWorker;
  std::mutex circuitLock;
  std::mutex bufferCircuitsLock;
  std::queue<std::unique_ptr<Circuit>> bufferCircuits;
  bool uiCircuitAlive = true;

  void calculatingBufferingCircuits() {
    while (uiCircuitAlive) {
      {
        // std::cout << "Locked at Calculating Buffering Circuits!" <<
        // std::endl; std::cout << "calculating buffering circuit! Buffering
        // Size: " << bufferCircuits.size() << std::endl;
        //

        std::unique_lock<std::mutex> bufferCircuitsUniqueLock(
            bufferCircuitsLock);
        if (bufferCircuits.size() >= 1000) {
          bufferCircuitsUniqueLock.unlock();
          std::this_thread::sleep_for(std::chrono::nanoseconds(1000));
          continue;
        }
        bufferCircuitsUniqueLock.unlock();

        std::unique_lock<std::mutex> circuitUniqueLock(circuitLock);
        if (circuit.get() == nullptr) {
          continue;
        }

        
        bool passed = false;
        bool hasOscillation = false;

        int MAX_ITERATION = 1000;

        int iteration = 1;
        // must be greater than 1
        // the greater the faster iterations slows down the iteration
        // osc -> iteration not doing its job -> drag too much
        // giving too much drag will cause oscillation
        //
        double iterationDrag = 1.1;

        // set pre
        circuit->setPreVAndPreI();
        while (true) {
          circuit->iterate(iteration, iterationDrag, &passed, &hasOscillation);
          if (hasOscillation) {
            // std::cout << "Oscillation occurs" << std::endl;
            iterationDrag = iterationDrag - (iterationDrag - 1.1) / 2;
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

        circuitUniqueLock.unlock();

        bufferCircuitsUniqueLock.lock();
        circuitUniqueLock.lock();

        // Reset may occur to reset circuit
        if (circuit.get() == nullptr) {
          continue;
        }

        // std::cout << "C:" << circuit->getCircuitID() << std::endl;
        std::unique_ptr<Circuit> pastCircuit =
            std::make_unique<Circuit>(*(circuit.get()));
        bufferCircuits.push(std::move(pastCircuit));
        // std::cout << "Buffer Circuit Count: " << bufferCircuits.size()
        //           << std::endl;
        std::cout << "Used Iterations: " << iteration << std::endl;
        // std::cout << "Unlock from calculating buffering circuit" <<
        // std::endl;
      }
      circuit->incTimerByDeltaT();
    }
  }

public:
  Circuit *getDisplayCircuit() { return displayingCircuit.get(); }

  double getCurrentScale() { return currentScale; }

  double getTime() {
    if (displayingCircuit.get() != nullptr) {
      return displayingCircuit->getTime();
    } else {
      return 0;
    }
  }

  int getMaxNodeID() { return nextPinID - 1; }

private:
  std::vector<std::unique_ptr<UIElement>> uiElements;
  std::unique_ptr<Circuit> circuit;
  std::unordered_map<std::string, int> locToPinID;
  std::unique_ptr<Circuit> displayingCircuit;
  int nextPinID = 0;
  double currentScale = 0.1;

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

private:
  bool tryNextTime = false;

public:
  void showCircuit(sf::RenderWindow *window, bool updateCircuit) {
    std::unique_lock<std::mutex> bufferCircuitsUniqueLock(bufferCircuitsLock,
                                                          std::try_to_lock);
    if (updateCircuit || tryNextTime) {
      if (bufferCircuitsUniqueLock.owns_lock() && !bufferCircuits.empty()) {
        displayingCircuit = std::move(bufferCircuits.front());
        bufferCircuits.pop();
        // std::cout << "Popping:" << bufferCircuits.size() << "Left" <<
        // std::endl;
        tryNextTime = false;
      } else if (bufferCircuits.empty()) {
        // std::cout << "Circuits in buffer is empty. Circuit not valid or sim "
        //              "cannot keep up"
        //              ".. Circuit Simulation might slow Down"
        //           << std::endl;
        tryNextTime = true;
      }
    }

    for (std::unique_ptr<UIElement> &uiElement : uiElements) {
      uiElement->showElement(window);
    }
  }

  // run this every frame
  void startCircuit() {
    std::unique_lock<std::mutex> circuitUniqueLock(circuitLock);
    std::cout << "Circuit locked for startCircuit" << std::endl;
    if (circuit.get() == nullptr) {
      if (!buildCircuit()) {
        for (std::unique_ptr<UIElement> &uiElement : uiElements) {
          uiElement->resetElement();
        }

        std::cout << "Circuit unlocked for startCircuit" << std::endl;
        return;
      }
    }
    std::cout << "Circuit unlocked for startCircuit" << std::endl;
  }

private:
  std::unordered_map<int, UIElement *> uiElementIDToUIElement;

public:
  UIElement *getUIElement(int id) { return uiElementIDToUIElement[id]; }
  double getDeltaT() { return deltaT; }

private:
  double deltaT = 0.00000001;
  // TODO:  remember to lock this function outside of calls
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

    circuit = Circuit::create(elements, deltaT, nextPinID - 1);
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
public:
  int getUIElementIDForUIElement(UIElement* elementPtr) {
    uiElementIDToUIElement[nextUIElementID] = elementPtr;
    nextUIElementID++; 
    return nextUIElementID-1;
  }
private:
  void resetCircuit() {
    std::cout << "RESET: trying to obtain bufferCircuits lock" << std::endl;
    std::unique_lock<std::mutex> bufferCircuitsUniqueLock(bufferCircuitsLock);
    std::cout << "RESET: trying to obtain circuit lock" << std::endl;
    std::unique_lock<std::mutex> circuitUniqueLock(circuitLock);
    std::cout << "Locked for resetCircuit" << std::endl;
    while (!bufferCircuits.empty()) {
      bufferCircuits.pop();
    }
    circuit.reset();
    displayingCircuit.reset();
    for (std::unique_ptr<UIElement> &uiElement : uiElements) {
      uiElement->resetElement();
    }
    std::cout << "Circuit unlocked for resetCircuit" << std::endl;
  }

public:
  void addElement(std::unique_ptr<UIElement> &uiElement) {
    resetCircuit();
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
