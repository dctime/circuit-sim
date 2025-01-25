#pragma once
#include "Circuit.h"
#include <CircuitElement.h>
#include <memory>

class CurrentSourceElement : public CircuitElement {
public:
  CurrentSourceElement() : CircuitElement() {};
  ~CurrentSourceElement() {};
  static std::unique_ptr<CurrentSourceElement> create(double i, int pin1,
                                                      int pin2) {
    // current flow from pin1 to pin2, + at pin1 - at pin2
    std::unique_ptr<CurrentSourceElement> vsrc =
        std::make_unique<CurrentSourceElement>();
    vsrc->i = i;
    vsrc->pin1 = pin1;
    vsrc->pin2 = pin2;
    return std::move(vsrc);
  }

  int getVoltageSourceCount() override { return 0; }

  void modifyGMatrix(Eigen::MatrixXd &g, Eigen::MatrixXd &v, int MAX_NODE_ID,
                     double t, double deltaT) override {}

  void modifyIMatrix(Eigen::MatrixXd &i, Eigen::MatrixXd &v, int MAX_NODE_ID,
                     double t, double deltaT) override {
    if (pin1 != -1) {
      i(pin1) += this->i;
    }

    if (pin2 != -1) {
      i(pin2) += -this->i;
    }
  }

  void incTime(Circuit *circuit) override {}

private:
  double i;
  int pin1;
  int pin2;

public:
  int getPin1() { return pin1; }

  int getPin2() { return pin2; }
};
