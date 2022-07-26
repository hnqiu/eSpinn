/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#pragma once

#include "eSpinn.h"
#include <iostream>
#include <chrono>

namespace eSpinn {
    const std::string FILE_REF_DATA_PSNN  (DIR_DATA + "ref_data_psnn");
    constexpr double winner_fit = 0.999;
    constexpr double ctrlRange[2] = {-4.0, 4.0};
    constexpr double ctrl_norm_factor = 6.0; // ctrlRange[1] - ctrlRange[0];
    constexpr double ctrl_shift = 7.0;

    /* @brief: controller task
     * use neural networks to control a plant model
     * construct a population of networks to solve the problem
     * calculate mean square errors and evaluate their fitness values
     * use evolutionary algorithms to improve their performance
     */
    int sim_ctrl();

    /* @brief: evaluate population 
     * use each network to control the plant model
     * and save system outputs to log_pos
     * calculate mean square errors 
     * and assign fitness values to organisms
     * finally, check if problem is solved
     */
    template <typename T>
    bool evaluate(Population *pop, Plant *plant, PlantLogger *log_pos);

    /* @brief: evaluate organism by controlling the plant model
     * log system outputs 
     * log controller outputs when re-evaluating champ organism
     * calculate mean square error 
     * and assign fitness value to organism
     */
    template <typename T>
    void evaluate(Organism<T> *org, Plant *plant, PlantLogger *log_pos, 
        Logger *const net_outp = nullptr, WeightWatcher *ww = nullptr);

    /* @brief: denormalize and shift controller output */
    double process(const double &raw_out);

    /* @brief: plasticify network connections
     * spawn the best organism
     * and evolve the plasticity parameters
     * assign fitness value to organisms
     * and find the best params
     */
    bool sim_plasticity();

    /* @brief: plasticify non-plastic networks using the evolved plastic rule
     * compare network performance when plasticity is activated
     */
    bool plasticify();

    /* @brief: iteration rate of evaluation
     * test evalution speed by
     * accumulating number of iterations during 10s real time
     */
    int sim_rate();

    /* @brief: verify trained networks with a different signal
     * read population from file, and
     * evaluate them with a verification signal
     */
    int verify();

    /* @brief: print out champ org
     * read population from file, and print the champ org
     */
    int print_champ();
}
