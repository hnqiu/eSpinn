/* Copyright (C) 2021 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#pragma once

#include "eSpinn.h"
#include <iostream>
#include <chrono>

namespace eSpinn {
    namespace Hexa {
        const std::string Z_POP         (DIR_ARCHIVE + "z");
        const std::string Z_CHAMP       (DIR_ARCHIVE + "z.champ");
        const std::string Z_CHAMP_ORG   (DIR_ARCHIVE + "zchamp.org");
        const std::string POP_EXT       (".pop");
        const std::string INJ_ARCH      (DIR_ARCHIVE + "inj.arch");
        const std::string FILE_Z_REF    (DIR_DATA + "z.ref");
        const std::string FILE_THR      (DIR_DATA + "thr");
        const std::string FILE_Z_ACT    (DIR_DATA + "z.act");
        const std::string FILE_Z_FIT    (DIR_DATA + "z.fit");
        const std::string FILE_Z_WEIGHT (DIR_DATA + "z.weight");
        constexpr double WINNER_FIT = 0.98;
        constexpr double thrRange[2] = {-0.06, 0.06};
        constexpr double thr_norm_factor = thrRange[1] - thrRange[0];
        constexpr double thr_shift = thrRange[0];
        constexpr double thr_hover = 0.2729;
    }

    /* @brief: controller task
     * use neural networks to control the linearized hexa heave model
     * construct a population of networks to solve the problem
     * calculate mean square errors and evaluate their fitness values
     * use evolutionary algorithms to improve their performance
     */
    int sim_hexa_heave();

    /* @brief: evaluate population 
     * use each network to control the plant model
     * and save system outputs to log_pos
     * calculate mean square errors 
     * and assign fitness values to organisms
     * finally, check if problem is solved
     */
    template <typename T>
    bool evaluate(Population *pop, Hexacopter *hexa,
        Injector *inj, PlantLogger *log_pos);

    /* @brief: evaluate organism by controlling the plant model
     * log system outputs 
     * log controller outputs when re-evaluating champ organism
     * calculate mean square error 
     * and assign fitness value to organism
     */
    template <typename T>
    void evaluate(Organism<T> *org, Hexacopter *hexa,
        Injector *inj, PlantLogger *log_pos,
        Logger *const net_outp = nullptr, WeightWatcher *ww = nullptr);

    /* @brief: denormalize and shift controller output */
    double process(const double &raw_out, double hover);

    /* @brief: plasticify network connections
     * spawn the best organism
     * and evolve the plasticity parameters
     * assign fitness value to organisms
     * and find the best params
     */
    bool sim_plasticity();

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
