#pragma once
#include "CircuitElement.h"
#include <memory>

class PMOSElement : public CircuitElement {
private:
  double id;

public:
  PMOSElement() : CircuitElement() {}
  ~PMOSElement() {}
  static std::unique_ptr<PMOSElement> create(double K, double VA, double VT,
                                             int PIN_G, int PIN_D, int PIN_S) {
    std::unique_ptr<PMOSElement> pmosEle = std::make_unique<PMOSElement>();
    pmosEle->PIN_G = PIN_G;
    pmosEle->PIN_D = PIN_D;
    pmosEle->PIN_S = PIN_S;
    pmosEle->K = K;
    pmosEle->VA = VA;
    pmosEle->VT = VT;
    return std::move(pmosEle);
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
    return calid(K, vs - vg, vs - vd, VT, VA);
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

    double g0 = calg0(K, vs - vg, vs - vd, VT, VA);

    if (PIN_D != -1)
      g(PIN_D, PIN_D) += g0;

    if (PIN_S != -1)
      g(PIN_S, PIN_S) += g0;

    if (PIN_D != -1 && PIN_S != -1) {
      g(PIN_D, PIN_S) += -g0;
      g(PIN_S, PIN_D) += -g0;
    }

    double GMIN = pow(10, -6);
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

    double ideq = calideq(K, vs - vg, vs - vd, VT, VA);
    double issd = calissd(K, vs - vg, vs - vd, VT, VA);

    if (PIN_D != -1) {
      // ideq large DC signal current
      // issd small signal current
      i(PIN_D) += ideq;
      i(PIN_D) += issd;
    }

    if (PIN_S != -1) {
      // ideq large DC signal current
      // issd small signal current
      i(PIN_S) += -ideq;
      i(PIN_S) += -issd;
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
  double calg0(double k, double vsg, double vsd, double vt, double va) {
    if (vsg - vt < 0)
      return 0;
    if (-vsd <= -vsg - vt)
      return k * ((vsg - vt) - vsd);
    return calid(k, vsg, vsd, vt, va)/va;
  }

  double calgm(double k, double vsg, double vsd, double vt, double va) {
    if (vsg - vt < 0)
      return 0;
    if (-vsd <= -vsg - vt)
      return k * vsd;
    return pow(2*k*calid(k, vsg, vsd, vt, va), 1/2.0);
  }

  double calid(double k, double vsg, double vsd, double vt, double va) {
    if (vsg - vt < 0)
      return 0;
    if (-vsd <= -vsg - vt)
      return k * ((vsg - vt) - vsd / 2) * vsd;
    return (k / 2) * pow(vsg - vt, 2) * (1 + (vsd - (vsd - vt)) / va);
  }

  double calissd(double k, double vsg, double vsd, double vt, double va) {
    return calgm(k, vsg, vsd, vt, va) * vsg;
  }

  double calideq(double k, double vsg, double vsd, double vt, double va) {
    return calid(k, vsg, vsd, vt, va) - calissd(k, vsg, vsd, vt, va) -
           calg0(k, vsg, vsd, vt, va) * vsd;
  }
};
