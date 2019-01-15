#include "FusionEKF.h"
#include <iostream>
#include "Eigen/Dense"
#include "tools.h"

using Eigen::MatrixXd;
using Eigen::VectorXd;
using std::cout;
using std::endl;
using std::vector;

/**
 * Constructor.
 */
FusionEKF::FusionEKF() {
  is_initialized_ = false;

  previous_timestamp_ = 0;

  // initializing matrices
  R_laser_ = MatrixXd(2, 2);
  R_radar_ = MatrixXd(3, 3);
  H_laser_ = MatrixXd(2, 4);
  Hj_ = MatrixXd(3, 4);

  //measurement covariance matrix - laser
  R_laser_ << 0.0225, 0,
              0, 0.0225;

  //measurement covariance matrix - radar
  R_radar_ << 0.09, 0, 0,
              0, 0.0009, 0,
              0, 0, 0.09;

  /**
   * TODO: Finish initializing the FusionEKF.
   * TODO: Set the process and measurement noises
   */
  H_laser_ << 1, 0, 0, 0,
              0, 1, 0, 0;

}

/**
 * Destructor.
 */
FusionEKF::~FusionEKF() {}

void FusionEKF::ProcessMeasurement(const MeasurementPackage &measurement_pack) {
  /**
   * Initialization
   */
  if (!is_initialized_) {
    /**
     * TODO: Initialize the state ekf_.x_ with the first measurement.
     * TODO: Create the covariance matrix.
     * You'll need to convert radar from polar to cartesian coordinates.
     */

    // first measurement
    cout << "EKF: " << endl;
    // ekf_.x_ = VectorXd(4);
    // ekf_.x_ << 1, 1, 1, 1;
    VectorXd x_in(4);
    if (measurement_pack.sensor_type_ == MeasurementPackage::RADAR) {
      // TODO: Convert radar from polar to cartesian coordinates 
      //         and initialize state.
      const double r = measurement_pack.raw_measurements_[0];
      const double t = measurement_pack.raw_measurements_[1];
      const double v = measurement_pack.raw_measurements_[2];
      x_in << r * cos(t), r * sin(t), v * cos(t), v * sin(t);
    }
    else if (measurement_pack.sensor_type_ == MeasurementPackage::LASER) {
      // TODO: Initialize state.
      const double x = measurement_pack.raw_measurements_[0];
      const double y = measurement_pack.raw_measurements_[1];
      x_in << x, y, 0, 0;
    } else {
      cout << "invalid sensor type" << endl;
      return;
    }
    MatrixXd F_in(4, 4);
    F_in << 1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1;
    MatrixXd P_in(4, 4);
    P_in << 1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1000, 0,
            0, 0, 0, 1000;
    MatrixXd H_in(4, 4);
    MatrixXd R_in(4, 4);
    MatrixXd Q_in(4, 4);
    ekf_.Init(x_in, P_in, F_in, H_in, R_in, Q_in); 

    // done initializing, no need to predict or update
    is_initialized_ = true;
    previous_timestamp_ = measurement_pack.timestamp_;
    // cout << "first state initialized." << endl;
    return;
  }

  /**
   * Prediction
   */
  double dt = (measurement_pack.timestamp_ - previous_timestamp_) / 1000000.;
  previous_timestamp_ = measurement_pack.timestamp_;
  /**
   * TODO: Update the state transition matrix F according to the new elapsed time.
   * Time is measured in seconds.
   * TODO: Update the process noise covariance matrix.
   * Use noise_ax = 9 and noise_ay = 9 for your Q matrix.
   */
  ekf_.F_(0, 2) = dt;
  ekf_.F_(1, 3) = dt;
  double dt2 = dt * dt;
  double dt3 = dt2 * dt;
  double dt4 = dt3 * dt;
  double ax = 9;
  double ay = 9;
  ekf_.Q_ << dt4 * ax / 4, 0, dt3 * ax / 2, 0,
             0, dt4 * ay / 4, 0, dt3 * ay / 2,
             dt3 * ax / 2, 0, dt2 * ax, 0,
             0, dt3 * ay / 2, 0, dt2 * ay;

  ekf_.Predict();

  /**
   * Update
   */

  /**
   * TODO:
   * - Use the sensor type to perform the update step.
   * - Update the state and covariance matrices.
   */
  if (measurement_pack.sensor_type_ == MeasurementPackage::RADAR) {
    // TODO: Radar updates
    const double x = ekf_.x_[0];
    const double y = ekf_.x_[1];
    if (x * x + y * y < 0.001) {
      std::cout << "near_zero p norm, skipping ekf update!" << std::endl;
    } else {
      ekf_.H_ = tools.CalculateJacobian(ekf_.x_);
      ekf_.R_ = R_radar_;
      ekf_.UpdateEKF(measurement_pack.raw_measurements_);
    }
  } else  if (measurement_pack.sensor_type_ == MeasurementPackage::LASER) {
    // TODO: Laser updates
    ekf_.H_ = H_laser_;
    ekf_.R_ = R_laser_;
    ekf_.Update(measurement_pack.raw_measurements_);
  } else {
    cout << "invalid sensor type!" << endl;
  }

  // print the output
  cout << "x_ = " << ekf_.x_ << endl;
  cout << "P_ = " << ekf_.P_ << endl;
}
