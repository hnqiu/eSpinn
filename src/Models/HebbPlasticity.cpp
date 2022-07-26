/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#include "HebbPlasticity.h"
using namespace eSpinn;


constexpr double HebbPlasticity::_mag[2];
constexpr double HebbPlasticity::_corr[2];

/* @brief: cap plastic terms */
void HebbPlasticity::cap_plastic_terms() {
    if (mag > _mag[1]) {
        mag = _mag[1];
    }
    else if (mag < _mag[0]) {
        mag = _mag[0];
    }

    if (corr > _corr[1]) {
        corr = _corr[1];
    }
    else if (corr < _corr[0]) {
        corr = _corr[0];
    }
}


/* @brief: get plastic term */
const double HebbPlasticity::get_plastic_term(const eSpinn_size &which) const {
    assert(which < 2);
    if (which) {
        return mag;
    }
    else {
        return corr;
    }
}


/* @brief: set plastic term */
void HebbPlasticity::set_plastic_term(const double &val, const eSpinn_size &which) {
    assert(which < 2);
    if (which) {
        mag = val;
    }
    else {
        corr = val;
    }
}


/* @brief: increase plastic term */
void HebbPlasticity::increase_plastic_term(const double &val, const eSpinn_size &which) {
    assert(which < 2);
    if (which) {
        mag += val;
    }
    else {
        corr += val;
    }
}
