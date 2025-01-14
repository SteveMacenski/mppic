#pragma once

#include <xtensor/xmath.hpp>
#include <xtensor/xnorm.hpp>
#include <xtensor/xtensor.hpp>
#include <xtensor/xview.hpp>

#include "mppic/optimization/scoring/CriticFunction.hpp"

#include "mppic/utils/common.hpp"

namespace mppi::optimization {

template <typename T>
class GoalCritic : public CriticFunction<T>
{
public:
  using CriticFunction<T>::parent_;
  using CriticFunction<T>::node_name_;

  void getParams() final
  {
    auto getParam = utils::getParamGetter(parent_, node_name_);
    getParam(power_, "goal_cost_power", 1);
    getParam(weight_, "goal_cost_weight", 20);
  }

  /**
   * @brief Evaluate cost related to goal following
   *
   * @param costs [out] add reference cost values to this tensor
   */
  virtual void score(
    const geometry_msgs::msg::PoseStamped & robot_pose, const xt::xtensor<T, 3> & trajectories,
    const xt::xtensor<T, 2> & path, xt::xtensor<T, 1> & costs) final
  {
    (void)robot_pose;

    const auto goal_points = xt::view(path, -1, xt::range(0, 2));

    auto trajectories_end = xt::view(trajectories, xt::all(), -1, xt::range(0, 2));

    auto dim = trajectories_end.dimension() - 1;

    auto && dists_trajectories_end_to_goal =
      xt::norm_l2(std::move(trajectories_end) - goal_points, {dim});

    costs += xt::pow(std::move(dists_trajectories_end_to_goal) * weight_, power_);
  }

private:
  unsigned int power_{0};
  double weight_{0};
};

} // namespace mppi::optimization
