#pragma once
#include "CircuitElement.h"
#include <Circuit.h>
#include <cmath>
#include <memory>

class DiodeElement : public CircuitElement {
private:
  double Is;
  double Vt;
  int PIN_1, PIN_2, PIN_M;
  int voltageSourceID;

public:
  int getVoltageSourceID() { return voltageSourceID; }

public:
  int getPIN1() { return PIN_1; }
  int getPIN2() { return PIN_2; }

public:
  static std::unique_ptr<DiodeElement> create(double Is, double Vt, int PIN_1,
                                              int PIN_2, int PIN_M,
                                              int voltageSouceID) {
    std::unique_ptr<DiodeElement> cap = std::make_unique<DiodeElement>(
        Is, Vt, PIN_1, PIN_2, PIN_M, voltageSouceID);
    return std::move(cap);
  }

  DiodeElement(double Is, double Vt, int PIN_1, int PIN_2, int PIN_M,
               int voltageSourceID) {
    this->Is = Is;
    this->Vt = Vt;
    this->PIN_1 = PIN_1;
    this->PIN_2 = PIN_2;
    this->PIN_M = PIN_M;
    this->voltageSourceID = voltageSourceID;
  }

public:
  int getVoltageSourceCount() override { return 1; }

  void modifyGMatrix(Eigen::MatrixXd &g, Eigen::MatrixXd &v, int MAX_NODE_ID,
                     double t, double deltaT) override {
    double v1, v2;
    if (PIN_1 == -1) {
      v1 = 0;
    } else {
      v1 = v(PIN_1);
    }

    if (PIN_2 == -1) {
      v2 = 0;
    } else {
      v2 = v(PIN_2);
    }
    double lastIterateV = v1 - v2;

    double g0 = getInnerConductance(deltaT, Is, Vt, v1 - v2);
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

    double GMIN = pow(10, -9);
    // add gmin
    if (PIN_1 != -1 && PIN_2 != -1) {
      g(PIN_1, PIN_2) += -GMIN;
      g(PIN_2, PIN_1) += -GMIN;
    }

    if (PIN_1 != -1) {
      g(PIN_1, PIN_1) += GMIN;
    }

    if (PIN_2 != -1) {
      g(PIN_2, PIN_2) += GMIN;
    }
  }

  void modifyIMatrix(Eigen::MatrixXd &i, Eigen::MatrixXd &v, int MAX_NODE_ID,
                     double t, double deltaT) override {

    double v1, v2;
    if (PIN_1 == -1) {
      v1 = 0;
    } else {
      v1 = v(PIN_1);
    }

    if (PIN_2 == -1) {
      v2 = 0;
    } else {
      v2 = v(PIN_2);
    }

    double lastIterateV = v1 - v2;
    if (PIN_1 != -1) {
      i(PIN_1) += -getInnerISource(deltaT, Is, Vt, lastIterateV);
    }

    if (PIN_2 != -1) {
      i(PIN_2) += getInnerISource(deltaT, Is, Vt, lastIterateV);
    }

    i(MAX_NODE_ID + 1 + voltageSourceID) = 0;
  }

  void incTime(Circuit *circuit) override {}

public:
  static double getInnerISource(double deltaT, double Is, double Vt,
                                double lastIterateV) {
    return Is * (exp(lastIterateV / Vt) - 1) -
           getInnerConductance(deltaT, Is, Vt, lastIterateV) * lastIterateV;
  }
  static double getInnerConductance(double deltaT, double Is, double Vt,
                                    double lastIterateV) {
    return Is * (exp(lastIterateV / Vt) - 1) / Vt;
  }
};
