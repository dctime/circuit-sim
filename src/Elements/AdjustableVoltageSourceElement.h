#pragma once
#include <CircuitElement.h>
#include <memory>

class AdjustableVoltageSourceElement : public CircuitElement {
public:
  AdjustableVoltageSourceElement() : CircuitElement() {};
  ~AdjustableVoltageSourceElement() {};
  static std::unique_ptr<AdjustableVoltageSourceElement>
  create(std::function<double(double)> v, int pin1, int pin2,
         int voltageSourceID) {
    // current flow from pin1 to pin2, + at pin1 - at pin2
    std::unique_ptr<AdjustableVoltageSourceElement> vsrc =
        std::make_unique<AdjustableVoltageSourceElement>();
    vsrc->v = v;
    vsrc->pin1 = pin1;
    vsrc->pin2 = pin2;
    vsrc->voltageSourceID = voltageSourceID;
    return std::move(vsrc);
  }

  double getV(double t) { return v(t); }
  int getVoltageSourceCount() override { return 1; }

  void modifyGMatrix(Eigen::MatrixXd &g, Eigen::MatrixXd &v, int MAX_NODE_ID,
                     double t) override {
    if (pin1 != -1) {
      g(MAX_NODE_ID + 1 + voltageSourceID, pin1) = 1;
      g(pin1, MAX_NODE_ID + 1 + voltageSourceID) = 1;
    }

    if (pin2 != -1) {
      g(MAX_NODE_ID + 1 + voltageSourceID, pin2) = -1;
      g(pin2, MAX_NODE_ID + 1 + voltageSourceID) = -1;
    }

    // std::cout << "adjVoltageSourceGSuccess" << std::endl;
  }

  void modifyIMatrix(Eigen::MatrixXd &i, Eigen::MatrixXd &v, int MAX_NODE_ID,
                     double t) override {
    i(MAX_NODE_ID + 1 + voltageSourceID) = this->v(t);
    // std::cout << "adjVoltageSourceISuccess" << std::endl;
  }

private:
  std::function<double(double)> v;
  int pin1;
  int pin2;
  int voltageSourceID;
public:
  int getPin1() {
    return pin1;
  }

  int getPin2() {
    return pin2;
  }

  int getVoltageSourceID() {
    return voltageSourceID;
  }
};
