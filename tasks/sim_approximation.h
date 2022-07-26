/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#pragma once

#include "eSpinn.h"
#include <iostream>

namespace eSpinn {
    /* @brief: approximation task - supervised learning
     * load training data from files and
     * construct a population of networks to solve the problem
     * calculate mean square errors and evaluate their fitness values
     * use evolutionary algorithms to improve their performance
     */
    int sim_approximation();

    /* @brief: evaluate population using training data from gate
     * calculate mean square errors 
     * and assign fitness values to organisms
     * finally, check if problem is solved
     */
    template <typename T>
    bool evaluate(Population *pop, Gate *gate);

    /* @brief: re-evaluate organism
     * push network outputs to gate
     */
    template <typename T>
    void evaluate(Organism<T> *org, Gate *gate);
}
