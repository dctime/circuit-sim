#pragma once
#include "../CircuitElement.h"
#include <memory>

class ResistorElement : public CircuitElement {
public:
  ResistorElement() : CircuitElement() {}
  ~ResistorElement() {}
  static std::unique_ptr<ResistorElement> create(double r, int pin1, int pin2) {
    // current flow from pin1 to pin2, + at pin1 - at pin2
    std::unique_ptr<ResistorElement> rEle = std::make_unique<ResistorElement>();
    rEle->R = r;
    rEle->PIN1 = pin1;
    rEle->PIN2 = pin2;
    return std::move(rEle);
  }

  int getVoltageSourceCount() override { return 0; }

  void modifyGMatrix(Eigen::MatrixXd &g, Eigen::MatrixXd &v, int MAX_NODE_ID,
                     double t, double deltaT) override {
    double g0 = 1.0 / R;
    if (PIN1 != -1)
      g(PIN1, PIN1) += g0;
    if (PIN2 != -1)
      g(PIN2, PIN2) += g0;
    if (PIN1 != -1 && PIN2 != -1) {
      g(PIN1, PIN2) += -g0;
      g(PIN2, PIN1) += -g0;
    }
  }

  void modifyIMatrix(Eigen::MatrixXd &i, Eigen::MatrixXd &v, int MAX_NODE_ID,
                     double t, double deltaT) override {}

  void incTime(Circuit *circuit) override {}

private:
  double R;
  int PIN1;
  int PIN2;
public:
  int getPIN1() {
    return PIN1;
  }

  int getPIN2() {
    return PIN2;
  }

  double getR() {
    return R;
  }
};
