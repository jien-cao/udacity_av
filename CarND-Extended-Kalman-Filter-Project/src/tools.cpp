#include "tools.h"
#include <iostream>

using Eigen::VectorXd;
using Eigen::MatrixXd;
using std::vector;
using std::cout;
using std::endl;

Tools::Tools() {}

Tools::~Tools() {}

VectorXd Tools::CalculateRMSE(const vector<VectorXd> &estimations,
                              const vector<VectorXd> &ground_truth) {
  /**
   * TODO: Calculate the RMSE here.
   */
  VectorXd rmse(4);
  rmse << 0.0, 0.0, 0.0, 0.0;
  if (estimations.size() == 0 || estimations.size() != ground_truth.size()) {
    std::cout << "invalid size for estimations, returning zero rmse!" << std::endl;
    return rmse;
  }
  for (unsigned int i = 0; i < estimations.size(); ++i) {
    VectorXd dx = estimations[i] - ground_truth[i];
    dx = dx.array() * dx.array();
    rmse += dx;
  }
  rmse /= estimations.size();
  cout << estimations.size() << endl;
  return rmse.array().sqrt();
}

MatrixXd Tools::CalculateJacobian(const VectorXd& x_state) {
  /**
   * TODO:
   * Calculate a Jacobian here.
   */
  MatrixXd Hj(3, 4);
  Hj << 0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0;
  double px = x_state(0);
  double py = x_state(1);
  double vx = x_state(2);
  double vy = x_state(3);
  double p_norm2 = px * px + py * py;
  double p_norm1 = sqrt(p_norm2);
  double p_norm3 = p_norm1 * p_norm2;
  double vpcross = vx * py - vy * px;
  Hj << px / p_norm1, py / p_norm1, 0, 0,
        -py / p_norm2, px / p_norm2, 0, 0,
        py * vpcross / p_norm3, px * vpcross / p_norm3, px / p_norm1, py / p_norm1;
  return Hj;
}
