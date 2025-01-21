#pragma once
#include "CircuitElement.h"
#include <memory>

class NMOSElement : public CircuitElement {
private:
  double id;

public:
  NMOSElement() : CircuitElement() {}
  ~NMOSElement() {}
  static std::unique_ptr<NMOSElement> create(double K, double VA, double VT,
                                             int PIN_G, int PIN_D, int PIN_S) {
    std::unique_ptr<NMOSElement> nmosEle = std::make_unique<NMOSElement>();
    nmosEle->PIN_G = PIN_G;
    nmosEle->PIN_D = PIN_D;
    nmosEle->PIN_S = PIN_S;
    nmosEle->K = K;
    nmosEle->VA = VA;
    nmosEle->VT = VT;
    return std::move(nmosEle);
  }

  int getVoltageSourceCount() override { return 0; }

  double getId(Eigen::MatrixXd &vResult) {
    double vg, vd, vs;
    if (PIN_G == -1) {
      vg = 0;
    } else {
      vg = vResult(PIN_G);
    }

    if (PIN_D == -1) {
      vd = 0;
    } else {
      vd = vResult(PIN_D);
    }

    if (PIN_S == -1) {
      vs = 0;
    } else {
      vs = vResult(PIN_S);
    }
    return calid(K, vg - vs, vd - vs, VT, VA);
  }

  void modifyGMatrix(Eigen::MatrixXd &g, Eigen::MatrixXd &v, int MAX_NODE_ID,
                     double t, double deltaT) override {
    // g0
    double vg, vd, vs;
    if (PIN_G == -1) {
      vg = 0;
    } else {
      vg = v(PIN_G);
    }

    if (PIN_D == -1) {
      vd = 0;
    } else {
      vd = v(PIN_D);
    }

    if (PIN_S == -1) {
      vs = 0;
    } else {
      vs = v(PIN_S);
    }

    double g0 = calg0(K, vg - vs, vd - vs, VT, VA);

    if (PIN_D != -1)
      g(PIN_D, PIN_D) += g0;

    if (PIN_S != -1)
      g(PIN_S, PIN_S) += g0;

    if (PIN_D != -1 && PIN_S != -1) {
      g(PIN_D, PIN_S) += -g0;
      g(PIN_S, PIN_D) += -g0;
    }

    double GMIN = pow(10, -9);
    // add gmin
    if (PIN_G != -1 && PIN_S != -1) {
      g(PIN_G, PIN_S) += -GMIN;
      g(PIN_S, PIN_G) += -GMIN;
    }

    if (PIN_G != -1 && PIN_D != -1) {
      g(PIN_G, PIN_D) += -GMIN;
      g(PIN_G, PIN_D) += -GMIN;
    }

    if (PIN_G != -1) {
      g(PIN_G, PIN_G) += 2*GMIN;
    }

    if (PIN_D != -1) {
      g(PIN_D, PIN_D) += GMIN;
    }

    if (PIN_S != -1) {
      g(PIN_S, PIN_S) += GMIN;
    }
  }

  void modifyIMatrix(Eigen::MatrixXd &i, Eigen::MatrixXd &v, int MAX_NODE_ID,
                     double t, double deltaT) override {
    double vg, vd, vs;
    if (PIN_G == -1) {
      vg = 0;
    } else {
      vg = v(PIN_G);
    }

    if (PIN_D == -1) {
      vd = 0;
    } else {
      vd = v(PIN_D);
    }

    if (PIN_S == -1) {
      vs = 0;
    } else {
      vs = v(PIN_S);
    }

    double ideq = calideq(K, vg - vs, vd - vs, VT, VA);
    double issd = calissd(K, vg - vs, vd - vs, VT, VA);

    if (PIN_D != -1) {
      // ideq large DC signal current
      // issd small signal current
      i(PIN_D) += -ideq;
      i(PIN_D) += -issd;
    }

    if (PIN_S != -1) {
      // ideq large DC signal current
      // issd small signal current
      i(PIN_S) += ideq;
      i(PIN_S) += issd;
    }
  }

  void incTime(Circuit *circuit) override {
    
  }

private:
  int PIN_G;
  int PIN_D;
  int PIN_S;
  double K;
  double VA;
  double VT;

public:
  int getPIN_G() { return PIN_G; }

  int getPIN_D() { return PIN_D; }

  int getPIN_S() { return PIN_S; }

private:
  double calg0(double k, double vgs, double vds, double vt, double va) {
    if (vgs - vt < 0)
      return 0;
    if (vds <= vgs - vt)
      return k * ((vgs - vt) - vds);
    return ((k / 2) * pow(vgs - vt, 2)) / va;
  }

  double calgm(double k, double vgs, double vds, double vt, double va) {
    if (vgs - vt < 0)
      return 0;
    if (vds <= vgs - vt)
      return k * vds;
    return k * (vgs - vt) * (1 + (vds - (vgs - vt)) / va);
  }

  double calid(double k, double vgs, double vds, double vt, double va) {
    if (vgs - vt < 0)
      return 0;
    if (vds <= vgs - vt)
      return k * ((vgs - vt) - vds / 2) * vds;
    return (k / 2) * pow(vgs - vt, 2) * (1 + (vds - (vgs - vt)) / va);
  }

  double calissd(double k, double vgs, double vds, double vt, double va) {
    return calgm(k, vgs, vds, vt, va) * vgs;
  }

  double calideq(double k, double vgs, double vds, double vt, double va) {
    return calid(k, vgs, vds, vt, va) - calissd(k, vgs, vds, vt, va) -
           calg0(k, vgs, vds, vt, va) * vds;
  }
};
