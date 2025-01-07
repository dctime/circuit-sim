#pragma once
#include "CircuitElement.h"
#include <memory>

class VoltageSourceElement : public CircuitElement {
public:
  VoltageSourceElement() : CircuitElement() {}
  ~VoltageSourceElement() {}
  static std::unique_ptr<VoltageSourceElement>
  create(double v, int pin1, int pin2, int voltageSourceID) {
    // current flow from pin1 to pin2, + at pin1 - at pin2
    std::unique_ptr<VoltageSourceElement> vsrc =
        std::make_unique<VoltageSourceElement>();
    vsrc->v = v;
    vsrc->pin1 = pin1;
    vsrc->pin2 = pin2;
    vsrc->voltageSourceID = voltageSourceID;
    return std::move(vsrc);
  }

  void modifyGMatrix(Eigen::MatrixXd &g, Eigen::MatrixXd &v, int MAX_NODE_ID,
                     double t) override {
    // std::cout << "modifying" << std::endl;
    // std::cout << "pin1" << pin1 << std::endl;
    // std::cout << "pin2" << pin2 << std::endl;
    // std::cout << "MAX_NODE_ID" << MAX_NODE_ID << std::endl;
    // std::cout << "voltageSourceID" << voltageSourceID << std::endl;
    if (pin1 != -1) {
      g(MAX_NODE_ID + 1 + voltageSourceID, pin1) = 1;
      // std::cout << "success1" << std::endl;
      g(pin1, MAX_NODE_ID + 1 + voltageSourceID) = 1;
      // std::cout << "success2" << std::endl;
    }

    if (pin2 != -1) {
      g(MAX_NODE_ID + 1 + voltageSourceID, pin2) = -1;
      g(pin2, MAX_NODE_ID + 1 + voltageSourceID) = -1;
    }

    // std::cout << "voltageSourceGSuccess" << std::endl;
  }

  void modifyIMatrix(Eigen::MatrixXd &i, Eigen::MatrixXd &v, int MAX_NODE_ID,
                     double t) override {
    i(MAX_NODE_ID + 1 + voltageSourceID) = this->v;

    // std::cout << "voltageSourceISuccess" << std::endl;
  }

  int getVoltageSourceCount() override { return 1; }

private:
  double v;
  int pin1;
  int pin2;
  int voltageSourceID;
};
