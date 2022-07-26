/* Copyright (C) 2021 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#include "Hexacopter.h"

using namespace eSpinn;


// definition of static members
constexpr double Hexacopter::velRANGE[];
constexpr double Hexacopter::pos_errRANGE[];
constexpr double Hexacopter::battRange[];
constexpr double Hexacopter::posERR[];

/* @brief: get current acceleration */
const double Hexacopter::getAcc() const {
    return acc;
}


/* @brief: get current velocity */
const double Hexacopter::getVel() const {
    return vel;
}


/* @brief: get current position */
const double Hexacopter::getPos() const {
    return pos;
}


/* @brief: get current hover value */
const double Hexacopter::getHover() const {
    return hover;
}


/* @brief: get approximated hover value from current batt */
const double Hexacopter::getApproxHover(const double &thr) const {
    return kb * (batt - 1*(thr-hover)) + b_h;
}


/* @brief: get current battery voltage */
const double Hexacopter::getBatt() const {
    return batt;
}

/* @brief: set battery voltage */
void Hexacopter::setBatt(const double &b) {
    batt = b;
}


/* @brief: run plant model to get current status
 * system dynamic
 * hover = kb * batt + b_h
 * a(t) = kt * (thr(t)-hover) + kv * v(t) + b
 * v(t+1) = v(t) + a(t) * dt
 * p(t+1) = p(t) + v(t+1) * dt
 * batt(t+1) = batt(t) - kb_t * dt
 */
bool Hexacopter::run(const double &thr) {
    hover = kb * batt + b_h;
    acc = kt * (thr-hover) + kv * vel + b;
    vel += acc * dt;
    // rectify();
    pos += vel * dt;

    batt -= kb_t * dt;
    // batt -= 1*(thr-hover);
    return check();
}


/* @brief: reset status */
void Hexacopter::reset() {
    acc = .0;
    vel = .0;
    pos = .0;
    batt = init_batt;
    hover = kb * init_batt + b_h;
}


/* @brief: cap state variables */
void Hexacopter::rectify() {
    if (vel < velRANGE[0])
        vel = velRANGE[0];
    else if (vel > velRANGE[1])
        vel = velRANGE[1];
}


/* @brief: check if state variables out of boundary */
bool Hexacopter::check() {
    if (pos < posERR[0] || pos > posERR[1])
        return false;

    return true;
}
