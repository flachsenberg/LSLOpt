#include <Eigen/Dense>

#include "LJLike_0.hpp"


struct ModelSystem {
  // Calculate the objective function value at position x.
  double value(const Eigen::VectorXd& x) const;

  // Calculate the gradient at position x.
  Eigen::VectorXd gradient(const Eigen::VectorXd& x) const;

  // Get the initial step length in the direction of p.
  double initial_step_length(
      const Eigen::VectorXd& x, // the current set of parameters
      const Eigen::VectorXd& p  // the current search direction
  ) const;

  // Check if change from old to new parameter set is acceptable.
  double change_acceptable(
      const Eigen::VectorXd& x_old, // the current set of parameters
      const Eigen::VectorXd& x_new  // the new set of parameters
  ) const;

  // Convert representation to Cartesian coordinates.
  Eigen::Vector2d to_cartesian(const Eigen::VectorXd& x) const;

  ModelSystem(const Eigen::Vector2d& x0 = Eigen::Vector2d(1.0, 1.0));

  Eigen::Vector2d x0;
  double d_max = 0.5;
  double r;

  AutoGeneratedSplines::LJLike_0::Spline spline;
};
