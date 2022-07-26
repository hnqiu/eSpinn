/* Copyright (C) 2017-2020 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#include "CartPole.h"

using namespace eSpinn;


/* @brief: random initialization
 * randomly initialize system states
 */
void CartPole::rand_init() {
    x = rand(-_0m3, _0m3);
    theta = rand(-_3deg, _3deg);
    // x = .0;
    // theta = _3deg;
}


/* @brief: reset system states */
void CartPole::reset() {
    rand_init();
    x_dot = .0;
    theta_dot = .0;
}


/* @brief: get current states */
const std::vector<double> CartPole::get_states() const {
    return {x, theta, x_dot, theta_dot};
}


/* @brief: update the system states */
bool CartPole::update(const double &force) {
    double costheta = std::cos(theta);
    double sintheta = std::sin(theta);

    double temp = (force + POLE_MASS*POLE_L * theta_dot*theta_dot * sintheta)
                  / TOTAL_MASS;
    double thetaacc = (G * sintheta - costheta * temp) / (POLE_L * (
                      _4THIRDS - POLE_MASS * costheta*costheta / TOTAL_MASS));
    double xacc  = temp - POLE_MASS*POLE_L * thetaacc * costheta / TOTAL_MASS;

    // update the four state variables using Euler's method
    x         += tau * x_dot;
    x_dot     += tau * xacc;
    theta     += tau * theta_dot;
    theta_dot += tau * thetaacc;

    return check();
}


/* @brief: check system states
 * return false if state variable is out of boundary
 */
bool CartPole::check() {
    if (x < xRANGE[0] || x > xRANGE[1])
        return false;

    if (theta < thetaRANGE[0] || theta > thetaRANGE[1])
        return false;

    return true;
}
