#pragma once
#include "CircuitElement.h"
#include <Circuit.h>
#include <memory>

class InductorElement : public CircuitElement {
private:
  double previousI = 0;
  double L;
  int PIN_1, PIN_2, PIN_M;
  int voltageSourceID;
public:
  int getVoltageSourceID() {
    return voltageSourceID;
  }
public:
  int getPIN1() { return PIN_1; }
  int getPIN2() { return PIN_2; }
  int getPINM() { return PIN_M; }

public:
  static std::unique_ptr<InductorElement> create(double L, double initialI,
                                                  int PIN_1, int PIN_2, int PIN_M, int voltageSourceID) {
    std::unique_ptr<InductorElement> cap =
        std::make_unique<InductorElement>(L, initialI, PIN_1, PIN_2, PIN_M, voltageSourceID);
    return std::move(cap);
  }

  InductorElement(double L, double initialI, int PIN_1, int PIN_2, int PIN_M, int voltageSourceID) {
    this->L = L;
    previousI = initialI;
    this->PIN_1 = PIN_1;
    this->PIN_2 = PIN_2;
    this->PIN_M = PIN_M;
    this->voltageSourceID = voltageSourceID;
  }

public:
  int getVoltageSourceCount() override { return 1; }

  void modifyGMatrix(Eigen::MatrixXd &g, Eigen::MatrixXd &v, int MAX_NODE_ID,
                     double t, double deltaT) override {
    double g0 = 1.0 / getInnerResistance(deltaT, previousI, L);
    if (PIN_1 != -1)
      g(PIN_1, PIN_1) += g0;
    if (PIN_M != -1)
      g(PIN_M, PIN_M) += g0;
    if (PIN_1 != -1 && PIN_M != -1) {
      g(PIN_1, PIN_M) += -g0;
      g(PIN_M, PIN_1) += -g0;
    }

    if (PIN_M != -1) {
      g(MAX_NODE_ID + 1 + voltageSourceID, PIN_M) = 1;
      // std::cout << "success1" << std::endl;
      g(PIN_M, MAX_NODE_ID + 1 + voltageSourceID) = 1;
      // std::cout << "success2" << std::endl;
    }

    if (PIN_2 != -1) {
      g(MAX_NODE_ID + 1 + voltageSourceID, PIN_2) = -1;
      g(PIN_2, MAX_NODE_ID + 1 + voltageSourceID) = -1;
    }
  }

  void modifyIMatrix(Eigen::MatrixXd &i, Eigen::MatrixXd &v, int MAX_NODE_ID,
                     double t, double deltaT) override {
    i(MAX_NODE_ID + 1 + voltageSourceID) = getInnerVSource(deltaT, previousI, L);
    
  }

  void incTime(Circuit *circuit) override {
    circuit->setCircuitID(Circuit::circuitCounter);
    Circuit::circuitCounter++;
    previousI = *circuit->getVoltagePointer(circuit->getMaxNodeID() +
              voltageSourceID + 1);
    std::cout << "PreviousI: " << previousI << std::endl;
    // std::cout << "CID:" << circuit->getCircuitID() << "updatedP:" << previousV
              // << std::endl;
  }

public:
  static double getInnerVSource(double deltaT, double previousI, double L) {
    return -previousI * InductorElement::getInnerResistance(deltaT, previousI, L);
  }
  static double getInnerResistance(double deltaT, double previousI, double L) { return L / deltaT; }
};
