#pragma once
#include <eigen3/Eigen/Dense>

class CircuitElement {
public:
  CircuitElement() {}
  virtual ~CircuitElement() {}
  virtual int getVoltageSourceCount() = 0;
  virtual void modifyGMatrix(Eigen::MatrixXd &g, Eigen::MatrixXd &v,
                             int MAX_NODE_ID, double t) = 0;
  virtual void modifyIMatrix(Eigen::MatrixXd &i, Eigen::MatrixXd &v,
                             int MAX_NODE_ID, double t) = 0;
};