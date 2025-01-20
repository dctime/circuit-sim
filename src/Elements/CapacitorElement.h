#include "CircuitElement.h"
#include <memory>

class CapacitorElement : public CircuitElement {
private:
  double previousV = 0;
  double C;
  double deltaT;
  int PIN_1, PIN_2;
public:

  int getPIN1() { return PIN_1; }
  int getPIN2() { return PIN_2; }
  double getDeltaT() { return deltaT; }

public:
  static std::unique_ptr<CapacitorElement>
  create(double C, double initialV, int PIN_1, int PIN_2) {
    std::unique_ptr<CapacitorElement> cap =
        std::make_unique<CapacitorElement>(C, initialV, PIN_1, PIN_2);
    return std::move(cap);
  }

  CapacitorElement(double C, double initialV, int PIN_1,
                   int PIN_2) {
    this->C = C;
    previousV = initialV;
    this->PIN_1 = PIN_1;
    this->PIN_2 = PIN_2;
  }

public:
  int getVoltageSourceCount() override { return 0; }

  void modifyGMatrix(Eigen::MatrixXd &g, Eigen::MatrixXd &v, int MAX_NODE_ID,
                     double t, double deltaT) override {
    this->deltaT = deltaT;
    double g0 = 1.0 / getInnerResistance(deltaT);
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
    this->deltaT = deltaT;
    if (PIN_1 != -1) {
      // ideq large DC signal current
      // issd small signal current
      i(PIN_1) += -getInnerISource(deltaT);
    }

    if (PIN_2 != -1) {
      // ideq large DC signal current
      // issd small signal current
      i(PIN_2) += getInnerISource(deltaT);
    }
  }

public:
  double getInnerISource(double deltaT) { return previousV / getInnerResistance(deltaT); }

  double getInnerResistance(double deltaT) { return deltaT / C; }
};
