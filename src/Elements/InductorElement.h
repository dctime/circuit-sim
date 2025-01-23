#pragma once
#include "CircuitElement.h"
#include <Circuit.h>
#include <memory>

class InductorElement : public CircuitElement {
private:
  double previousI = 0;
  double L;
  int PIN_1, PIN_2;
public:
  int getPIN1() { return PIN_1; }
  int getPIN2() { return PIN_2; }

public:
  static std::unique_ptr<InductorElement> create(double L, double initialI,
                                                  int PIN_1, int PIN_2) {
    std::unique_ptr<InductorElement> cap =
        std::make_unique<InductorElement>(L, initialI, PIN_1, PIN_2);
    return std::move(cap);
  }

  InductorElement(double L, double initialI, int PIN_1, int PIN_2) {
    this->L = L;
    previousI = initialI;
    this->PIN_1 = PIN_1;
    this->PIN_2 = PIN_2;
  }

public:
  // FIXME: Add a 0 volt votlage source to the end so that the current can be got by UIElement
  int getVoltageSourceCount() override { return 0; }

  void modifyGMatrix(Eigen::MatrixXd &g, Eigen::MatrixXd &v, int MAX_NODE_ID,
                     double t, double deltaT) override {
    double g0 = getInnerConductance(deltaT, previousI, L);
    if (PIN_1 != -1)
      g(PIN_1, PIN_1) += g0;
    if (PIN_2 != -1)
      g(PIN_2, PIN_2) += g0;
    if (PIN_1 != -1 && PIN_2 != -1) {
      g(PIN_1, PIN_2) += -g0;
      g(PIN_2, PIN_1) += -g0;
    }
  }

  void modifyIMatrix(Eigen::MatrixXd &i, Eigen::MatrixXd &v, int MAX_NODE_ID,
                     double t, double deltaT) override {

    if (PIN_1 != -1) {
      i(PIN_1) += -getInnerISource(deltaT, previousI, L);
    }

    if (PIN_2 != -1) {
      i(PIN_2) += getInnerISource(deltaT, previousI, L);
    }
    
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
    std::cout << "Pin1Volt: " << pin1Volt << std::endl;
    std::cout << "Pin2Volt: " << pin2Volt << std::endl;
    std::cout << "DeltaT: " << circuit->getDeltaT() << std::endl;
    std::cout << "L: " << L << std::endl;
    circuit->setCircuitID(Circuit::circuitCounter);
    Circuit::circuitCounter++;
    double RI = (pin1Volt - pin2Volt) * getInnerConductance(circuit->getDeltaT(), previousI, L);
    previousI = RI + getInnerISource(circuit->getDeltaT(), previousI, L);    
    std::cout << "PreviousI: " << previousI << std::endl;
    //
    // std::cout << "CID:" << circuit->getCircuitID() << "updatedP:" << previousV
              // << std::endl;
  }

public:
  static double getInnerISource(double deltaT, double previousI, double L) {
    return previousI;
  }
  static double getInnerConductance(double deltaT, double previousI, double L) { return deltaT / L; }
};
