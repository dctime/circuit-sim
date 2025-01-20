#pragma once
#include "CircuitElement.h"
#include <chrono>
#include <cmath>
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Sparse>
#include <iostream>
#include <limits>
#include <memory>
#include <random>
class Circuit {
public:
  static std::unique_ptr<Circuit> create(std::vector<CircuitElement *> elements,
                                         double deltaT, int MAX_NODE_ID) {
    std::unique_ptr<Circuit> circuit = std::make_unique<Circuit>();
    circuit->elements = elements;
    circuit->t = 0;
    circuit->deltaT = deltaT;
    circuit->MAX_NODE_ID = MAX_NODE_ID;

    circuit->MAX_MATRIX_SIZE = MAX_NODE_ID + 1; // node starts from 0
    for (CircuitElement *ele : elements) {
      circuit->MAX_MATRIX_SIZE += ele->getVoltageSourceCount();
    }

    circuit->v.resize(circuit->MAX_MATRIX_SIZE, 1);
    // std::uniform_real_distribution<double> dis(0, 2);
    // std::default_random_engine re;
    // for (int index = 0; index <= MAX_NODE_ID; index++) {
    //   circuit->v(index) = dis(re);
    // }

    circuit->v.setZero();
    circuit->g.resize(circuit->MAX_MATRIX_SIZE, circuit->MAX_MATRIX_SIZE);
    circuit->g.setZero();
    circuit->i.resize(circuit->MAX_MATRIX_SIZE, 1);
    circuit->i.setZero();
    std::cout << "MAX_NODE_ID: " << MAX_NODE_ID << std::endl;
    std::cout << "MAX_MATRIX_SIZE: " << circuit->MAX_MATRIX_SIZE << std::endl;

    for (CircuitElement *ele : elements) {
      // g matrix
      ele->modifyGMatrix(circuit->g, circuit->v, circuit->MAX_NODE_ID, 0, deltaT);
      // i matrix
      ele->modifyIMatrix(circuit->i, circuit->v, circuit->MAX_NODE_ID, 0, deltaT);
      // std::cout << "element success" << std::endl;
    }

    return std::move(circuit);
  }

  void incTimerByDeltaT() { t += deltaT; }
  // keep iterate: false
  // can stop: true
private:
  double lastTimeSumOfIEvaluation = std::numeric_limits<double>::max();
  double floatingRootScale = 10;

public:
  void iterate(int iteration, double iterationDrag, bool *passed,
               bool *hasOscillation) {
    g.resize(MAX_MATRIX_SIZE, MAX_MATRIX_SIZE);
    g.setZero();
    i.resize(MAX_MATRIX_SIZE, 1);
    i.setZero();
    // std::cout << "MAX_MATRIX_SIZE: " << MAX_MATRIX_SIZE << std::endl;

    for (CircuitElement *ele : elements) {
      // g matrix
      ele->modifyGMatrix(g, v, MAX_NODE_ID, t, deltaT);
      // i matrix
      ele->modifyIMatrix(i, v, MAX_NODE_ID, t, deltaT);
      // std::cout << "element success" << std::endl;
    }

    // std::cout << "g: " << std::endl;
    // std::cout << g << std::endl;
    // std::cout << "v: " << std::endl;
    // std::cout << v << std::endl;
    // std::cout << "i: " << std::endl;
    // std::cout << i << std::endl;
    // std::cout << "Start ITERATE: " << std::endl;
    // F matrix
    Eigen::MatrixXd f(MAX_MATRIX_SIZE, 1);
    f = g * v - i;
    // std::cout << "f: " << std::endl;
    // std::cout << f << std::endl;

    // std::cout << "f matrix success" << std::endl;
    // J matrix
    double delta = 0.000001;
    Eigen::MatrixXd dupV = v.replicate(1, MAX_MATRIX_SIZE);
    Eigen::MatrixXd vWithDelta =
        Eigen::MatrixXd::Identity(MAX_MATRIX_SIZE, MAX_MATRIX_SIZE);
    vWithDelta = vWithDelta * delta + dupV;
    // std::cout << "vWithData value: " << vWithDelta << std::endl;
    Eigen::MatrixXd j = (g * vWithDelta - g * dupV) / delta;

    // std::cout << "j" << std::endl;
    // std::cout << j << std::endl;
    //
    // std::cout << "j inverse" << std::endl;
    // std::cout << j.inverse() << std::endl;
    //
    // std::cout << "j matrix success" << std::endl;

    // calculate deltaV
    Eigen::MatrixXd deltaV = -1 * ((j.inverse()) * f);
    // std::cout << "deltaV value:" << std::endl;
    // std::cout << deltaV << std::endl;

    // TODO: Find points deltaV too aggressive
    double maxDeltaV = 1;
    for (int index = 0; index < MAX_MATRIX_SIZE; index++) {
      if (fabs(deltaV(index)) > maxDeltaV) {
        bool negative = false;
        if (deltaV(index) < 0)
          negative = true;

        // std::cout << "Index: " << index << " is too sensitive" << std::endl;
        double fabsDeltaV = std::fabs(deltaV(index));
        double modifiedV = maxDeltaV + 1;
        while (true) {
          modifiedV = pow(fabsDeltaV, 1.0 / floatingRootScale) * (maxDeltaV);
          if (modifiedV < maxDeltaV * 2) {
            break;
          }
          floatingRootScale++;
        }

        if (negative) {
          modifiedV *= -1;
        }

        // std::cout << "Value updated from " << deltaV(index);
        deltaV(index) = modifiedV;
        // std::cout << " to " << deltaV(index) << std::endl;
        // std::cout << "floatingRootScale: " << floatingRootScale << std::endl;
      }
    }
    // calculate new v
    // more drag means more iterations
    //
    double maxDeltaLength = 0.001;
    if (iteration != 1) {
      maxDeltaLength = 10;
    }

    double normDeltaV = deltaV.norm();
    if (normDeltaV >= maxDeltaLength) {
      deltaV = deltaV * (1 / normDeltaV);
    }

    deltaV = deltaV / pow(iteration, 1 / iterationDrag);
    // std::cout << "mod DeltaV:" << std::endl;
    // std::cout << deltaV << std::endl;
    if (deltaV.array().isNaN().any()) {
      std::cout << "NAN occurs" << std::endl;
      std::cout << "G:" << std::endl << g << std::endl;
      std::cout << "V:" << std::endl << v << std::endl;
      std::cout << "I:" << std::endl << i << std::endl;
      std::cout << "J" << std::endl << j << std::endl;
      std::cout << "J inv:" << std::endl << j.inverse() << std::endl;
      std::cout << "F:" << std::endl << f << std::endl;
    } else {
      v += deltaV;
      *passed = true;
    }
    // std::cout << "cal v success" << std::endl;

    // print v
    // std::cout << "V0\nV1\nIx\nIY:" << std::endl;
    // std::cout << "V:" << std::endl;
    // std::cout << v << std::endl;
    Eigen::MatrixXd iEvaluations = g * v - i;
    double sumOfIEvaluation = 0;
    double IEvaluationMax = 0;
    for (int index = 0; index < iEvaluations.size(); index++) {
      double absIEvaluation = fabs(iEvaluations(index));
      if (IEvaluationMax < absIEvaluation) {
        IEvaluationMax = absIEvaluation;
      }
      sumOfIEvaluation += absIEvaluation;
    }

    // std::cout << "===============" << std::endl;
    // std::cout << "evaluation:" << std::endl;
    // std::cout << iEvaluations << std::endl;
    // std::cout << "sum of evaluation: " << std::endl;
    // std::cout << sumOfIEvaluation << std::endl;

    double reltol = 0.001;
    double iabstol = 1 * pow(10, -12);
    double residueCriterion = reltol * IEvaluationMax + iabstol;

    // std::cout << "residueCriterion: " << residueCriterion << std::endl;

    if (iteration == 1) {
      lastTimeSumOfIEvaluation = std::numeric_limits<double>::max();
    }

    if (sumOfIEvaluation > lastTimeSumOfIEvaluation) {
      *hasOscillation = true;
    }

    lastTimeSumOfIEvaluation = sumOfIEvaluation;

    if (sumOfIEvaluation >= residueCriterion) {
      *passed = false;
      // std::cout << "===============" << std::endl;
      return;
    }

    *passed = true;
  }

  double getVoltage(int PIN_ID) { return v(PIN_ID); }
  double *getVoltagePointer(int PIN_ID) { return &v(PIN_ID); }
  Eigen::MatrixXd &getVoltageMatrix() { return v; }
  double getTime() { return t; }

private:
  std::vector<CircuitElement *> elements;
  Eigen::MatrixXd v;
  Eigen::MatrixXd g;
  Eigen::MatrixXd i;
  double t;
  double deltaT;
  int MAX_NODE_ID;
  int MAX_MATRIX_SIZE;
};
