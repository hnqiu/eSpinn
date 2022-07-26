/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#pragma once

#include "Utilities/DataGenerator.h"

/* @brief: training data generator
 * save the data to files
 */

namespace eSpinn {

    /* @brief: generate training data and
     * save the data to files
     */
    bool data_generator(std::size_t &timestep);
}
