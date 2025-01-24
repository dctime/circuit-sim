#pragma once
#include "CircuitElement.h"
#include <Circuit.h>
#include <memory>

class InductorElement : public CircuitElement {
private:
  double previousI = 0;
  double previousV = 0;
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

public:
  static std::unique_ptr<InductorElement>
  create(double L, double initialI, int PIN_1, int PIN_2, int PIN_M, int voltageSouceID) {
    std::unique_ptr<InductorElement> cap =
        std::make_unique<InductorElement>(L, initialI, PIN_1, PIN_2, PIN_M, voltageSouceID);
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
  // FIXME: Add a 0 volt votlage source to the end so that the current can be
  // got by UIElement
  int getVoltageSourceCount() override { return 1; }

  void modifyGMatrix(Eigen::MatrixXd &g, Eigen::MatrixXd &v, int MAX_NODE_ID,
                     double t, double deltaT) override {
    double g0 = getInnerConductance(deltaT, previousI, previousV, L);
    if (PIN_1 != -1)
      g(PIN_1, PIN_1) += g0;
    if (PIN_2 != -1)
      g(PIN_2, PIN_2) += g0;
    if (PIN_1 != -1 && PIN_2 != -1) {
      g(PIN_1, PIN_2) += -g0;
      g(PIN_2, PIN_1) += -g0;
    }

    if (PIN_2 != -1) {
      g(MAX_NODE_ID + 1 + voltageSourceID, PIN_2) = 1;
      // std::cout << "success1" << std::endl;
      g(PIN_2, MAX_NODE_ID + 1 + voltageSourceID) = 1;
      // std::cout << "success2" << std::endl;
    }

    if (PIN_M != -1) {
      g(MAX_NODE_ID + 1 + voltageSourceID, PIN_M) = -1;
      g(PIN_M, MAX_NODE_ID + 1 + voltageSourceID) = -1;
    }
  }

  void modifyIMatrix(Eigen::MatrixXd &i, Eigen::MatrixXd &v, int MAX_NODE_ID,
                     double t, double deltaT) override {

    if (PIN_1 != -1) {
      i(PIN_1) += -getInnerISource(deltaT, previousI, previousV, L);
    }

    if (PIN_2 != -1) {
      i(PIN_2) += getInnerISource(deltaT, previousI, previousV, L);
    }


    i(MAX_NODE_ID + 1 + voltageSourceID) = 0;
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
    // std::cout << "Pin1Volt: " << pin1Volt << std::endl;
    // std::cout << "Pin2Volt: " << pin2Volt << std::endl;
    // std::cout << "DeltaT: " << circuit->getDeltaT() << std::endl;
    // std::cout << "L: " << L << std::endl;
    circuit->setCircuitID(Circuit::circuitCounter);
    Circuit::circuitCounter++;
    double RI =
        (pin1Volt - pin2Volt) *
        getInnerConductance(circuit->getDeltaT(), previousI, previousV, L);
    previousI =
        RI + getInnerISource(circuit->getDeltaT(), previousI, previousV, L);
    previousV = pin1Volt - pin2Volt;
    // std::cout << "PreviousI: " << previousI << std::endl;
    //
    // std::cout << "CID:" << circuit->getCircuitID() << "updatedP:" <<
    // previousV
    // << std::endl;
  }

public:
  static double getInnerISource(double deltaT, double previousI,
                                double previousV, double L) {
    return previousI + deltaT / (2 * L) * previousV;
  }
  static double getInnerConductance(double deltaT, double previousI,
                                    double previousV, double L) {
    return deltaT / (2 * L);
  }
};
