#include <iostream>
#include "kalman_filter.h"

using Eigen::MatrixXd;
using Eigen::VectorXd;
using std::cout;
using std::endl;

/* 
 * Please note that the Eigen library does not initialize 
 *   VectorXd or MatrixXd objects with zeros upon creation.
 */

KalmanFilter::KalmanFilter() {}

KalmanFilter::~KalmanFilter() {}

void KalmanFilter::Init(VectorXd &x_in, MatrixXd &P_in, MatrixXd &F_in,
                        MatrixXd &H_in, MatrixXd &R_in, MatrixXd &Q_in) {
  x_ = x_in;
  P_ = P_in;
  F_ = F_in;
  H_ = H_in;
  R_ = R_in;
  Q_ = Q_in;
}

void KalmanFilter::Predict() {
  /**
   * TODO: predict the state
   */
  // std::cout << "entering predict... ";
  x_ = F_ * x_;
  P_ = F_ * P_ * F_.transpose() + Q_;
  // std::cout << "done." << std::endl;
}

void KalmanFilter::Update(const VectorXd &z) {
  /**
   * TODO: update the state by using Kalman Filter equations
   */
  // std::cout << "entering update KF ";
  VectorXd y = z - H_ * x_;
  MatrixXd S = H_ * P_ * H_.transpose() + R_;
  MatrixXd K = P_ * H_.transpose() * S.inverse();
  x_ = x_ + K * y;
  MatrixXd I(4, 4);
  I << 1.0, 0, 0, 0,
       0, 1.0, 0, 0,
       0, 0, 1.0, 0,
       0, 0, 0, 1.0;
  P_ = (I - K * H_) * P_;
  // std::cout << "done." << std::endl;
}

void KalmanFilter::UpdateEKF(const VectorXd &z) {
  /**
   * TODO: update the state by using Extended Kalman Filter equations
   */
  // std::cout << "entering update EKF " << std::endl;
  std::cout << P_ << "end of P" << std::endl;
  VectorXd r(2);
  r << x_[0], x_[1];
  VectorXd v(2);
  v << x_[2], x_[3];
  VectorXd Hx(3);
  Hx << r.norm(), atan2(x_[1], x_[0]), v.dot(r) / r.norm();
  VectorXd y(3);
  y = z - Hx;
  while (y(1) < -M_PI) {
    y(1) += 2 * M_PI;
  }
  while (y(1) > M_PI) {
    y(1) -= 2 * M_PI;
  }
  MatrixXd S;
  S = H_ * P_ * H_.transpose() + R_;
  MatrixXd K;
  K = P_ * H_.transpose() * S.inverse();
  x_ = x_ + K * y;
  MatrixXd I(4, 4);
  I << 1.0, 0, 0, 0,
       0, 1.0, 0, 0,
       0, 0, 1.0, 0,
       0, 0, 0, 1.0;
  P_ = (I - K * H_) * P_;
  // std::cout << "done." << std::endl;
}
