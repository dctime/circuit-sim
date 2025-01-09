#pragma once
#include "CircuitElement.h"
#include <eigen3/Eigen/Dense>
#include <iostream>
#include <memory>
#include <eigen3/Eigen/Sparse>
#include <chrono>
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
    circuit->v.setZero();
    circuit->g.resize(circuit->MAX_MATRIX_SIZE, circuit->MAX_MATRIX_SIZE);
    circuit->g.setZero();
    circuit->i.resize(circuit->MAX_MATRIX_SIZE, 1);
    circuit->i.setZero();
    std::cout << "MAX_NODE_ID: " << MAX_NODE_ID << std::endl;
    std::cout << "MAX_MATRIX_SIZE: " << circuit->MAX_MATRIX_SIZE << std::endl;

    for (CircuitElement *ele : elements) {
      // g matrix
      ele->modifyGMatrix(circuit->g, circuit->v, circuit->MAX_NODE_ID, 0);
      // i matrix
      ele->modifyIMatrix(circuit->i, circuit->v, circuit->MAX_NODE_ID, 0);
      // std::cout << "element success" << std::endl;
    }

    return std::move(circuit);
  }

  void incTimerByDeltaT() { t += deltaT; }
  void iterate(int iteration) {
    g.resize(MAX_MATRIX_SIZE, MAX_MATRIX_SIZE);
    g.setZero();
    i.resize(MAX_MATRIX_SIZE, 1);
    i.setZero();
    // std::cout << "MAX_MATRIX_SIZE: " << MAX_MATRIX_SIZE << std::endl;


    for (CircuitElement *ele : elements) {
      // g matrix
      ele->modifyGMatrix(g, v, MAX_NODE_ID, t);
      // i matrix
      ele->modifyIMatrix(i, v, MAX_NODE_ID, t);
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

    // std::cout << "f matrix success" << std::endl;
    // J matrix
    double delta = 0.00000001;
    Eigen::MatrixXd dupV = v.replicate(1, MAX_MATRIX_SIZE);
    Eigen::MatrixXd vWithDelta =
        Eigen::MatrixXd::Identity(MAX_MATRIX_SIZE, MAX_MATRIX_SIZE);
    vWithDelta = vWithDelta * delta + dupV;
    // std::cout << "vWithData value: " << vWithDelta << std::endl;
    Eigen::MatrixXd j = (g * vWithDelta - g * dupV) / delta;
    // std::cout << "j" << std::endl;
    // std::cout << j << std::endl;
    //
    // std::cout << "j matrix success" << std::endl;

    // calculate deltaV
    Eigen::MatrixXd deltaV = -1 * ((j.inverse()) * f);
    // std::cout << "deltaV value:" << std::endl;
    // std::cout << deltaV << std::endl;
    // calculate new v
    v += deltaV / iteration;
    // std::cout << "cal v success" << std::endl;

    // print v
    // std::cout << "iteration: " << "idk" << std::endl;
    // std::cout << "V0\nV1\nIx\nIY:" << std::endl;
    // std::cout << v << std::endl;
    // std::cout << "===============" << std::endl;
    // std::cout << "evaluation:" << std::endl;
    // std::cout << g * v - i << std::endl;
    // std::cout << "===============" << std::endl;
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
