/* Copyright (C) 2020-2021 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#pragma once

#include "eSpinn_def.h"

/* @brief: test eta */

namespace eSpinn {

    /* @brief: assign eta */
    inline void set_eta(const double &e) {
        params::eta = e;
    }

    /* @brief: get eta */
    inline const double get_eta() {
        return params::eta;
    }
}
