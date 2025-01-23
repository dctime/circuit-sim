#pragma once
#include "CircuitElement.h"
#include <Circuit.h>
#include <memory>

class CapacitorElement : public CircuitElement {
private:
  double previousV = 0;
  double C;
  int PIN_1, PIN_2, PIN_M;
  int voltageSourceID;
public:
  int getVoltageSourceID() {
    return voltageSourceID;
  }

public:
  int getPIN1() { return PIN_1; }
  int getPIN2() { return PIN_2; }

public:
  static std::unique_ptr<CapacitorElement>
  create(double C, double initialV, int PIN_1, int PIN_2, int PIN_M, int voltageSourceID) {
    std::unique_ptr<CapacitorElement> cap =
        std::make_unique<CapacitorElement>(C, initialV, PIN_1, PIN_2, PIN_M, voltageSourceID);
    return std::move(cap);
  }

  CapacitorElement(double C, double initialV, int PIN_1, int PIN_2, int PIN_M, int voltageSourceID) {
    this->C = C;
    previousV = initialV;
    this->PIN_1 = PIN_1;
    this->PIN_2 = PIN_2;
    this->PIN_M = PIN_M;
    this->voltageSourceID = voltageSourceID;
  }

public:
  int getVoltageSourceCount() override { return 1; }

  void modifyGMatrix(Eigen::MatrixXd &g, Eigen::MatrixXd &v, int MAX_NODE_ID,
                     double t, double deltaT) override {
    double g0 = getInnerConductance(deltaT, previousV, C);
    if (PIN_2 != -1)
      g(PIN_2, PIN_2) += g0;
    if (PIN_M != -1)
      g(PIN_M, PIN_M) += g0;
    if (PIN_2 != -1 && PIN_M != -1) {
      g(PIN_2, PIN_M) += -g0;
      g(PIN_M, PIN_2) += -g0;
    }

    if (PIN_1 != -1) {
      g(MAX_NODE_ID + 1 + voltageSourceID, PIN_1) = 1;
      // std::cout << "success1" << std::endl;
      g(PIN_1, MAX_NODE_ID + 1 + voltageSourceID) = 1;
      // std::cout << "success2" << std::endl;
    }

    if (PIN_M != -1) {
      g(MAX_NODE_ID + 1 + voltageSourceID, PIN_M) = -1;
      g(PIN_M, MAX_NODE_ID + 1 + voltageSourceID) = -1;
    }
    
    
  }

  void modifyIMatrix(Eigen::MatrixXd &i, Eigen::MatrixXd &v, int MAX_NODE_ID,
                     double t, double deltaT) override {
    i(MAX_NODE_ID + 1 + voltageSourceID) = getInnerVSource(deltaT, previousV, C);
  }

  void incTime(Circuit *circuit) override {
    double pin1Volt = 0;
    double pin2Volt = 0;
    if (PIN_1 != -1) {
      pin1Volt = *circuit->getVoltagePointer(PIN_1);
    }

    if (PIN_2 != -1) {
      pin2Volt = *circuit->getVoltagePointer(PIN_2);
    }

    circuit->setCircuitID(Circuit::circuitCounter);
    Circuit::circuitCounter++;
    previousV = pin1Volt - pin2Volt;
    // std::cout << "CID:" << circuit->getCircuitID() << "updatedP:" <<
    // previousV
    // << std::endl;
  }

public:
  static double getInnerVSource(double deltaT, double previousV, double C) {
    return previousV;
  }
  static double getInnerConductance(double deltaT, double previousV, double C) {
    return C / deltaT;
  }
};
