/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#pragma once

#include "eSpinn.h"
#include <iostream>

namespace eSpinn {
    /* @brief: test EA
     * use virtual evaluate method to assign fitness to organisms
     * and test whether pop::epoch() is working 
     */
    int sim_virtual();

    /* @brief: assign random value to organisms' fitness
     * random values will not be greater than std_fit
     * therefore, problem is never solved
     */
    bool virtual_evaluate(Population *pop);
}
