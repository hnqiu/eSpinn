/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#include "Plant.h"

using namespace eSpinn;


// definition of static members
constexpr double Plant::velRANGE[];
constexpr double Plant::posRANGE[];

/* @brief: get current acceleration */
const double Plant::getAcc() const {
    return acc;
}


/* @brief: get current velocity */
const double Plant::getVel() const {
    return vel;
}


/* @brief: get current position */
const double Plant::getPos() const {
    return pos;
}


/* @brief: run plant model to get current status
 * system dynamic
 * a(t) = kt * f(t) + kv * v(t) + b
 * v(t+1) = v(t) + a(t) * dt
 * s(t+1) = s(t) + v(t+1) * dt
 */
bool Plant::run(const double &force) {
    acc = kt * force + kv * vel + b + g;
    vel += acc * dt;
    // rectify();
    pos += vel * dt;
    return check();
}


/* @brief: reset status */
void Plant::reset() {
    acc = .0;
    vel = .0;
    pos = .0;
}


/* @brief: cap state variables */
void Plant::rectify() {
    if (vel < velRANGE[0])
        vel = velRANGE[0];
    else if (vel > velRANGE[1])
        vel = velRANGE[1];
}


/* @brief: check if state variables out of boundary */
bool Plant::check() {
    if (pos < posRANGE[0])
        return false;
    else if (pos > posRANGE[1])
        return false;

    return true;
}
