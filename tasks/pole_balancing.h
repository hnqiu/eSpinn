/* Copyright (C) 2017-2020 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#pragma once

#include "eSpinn.h"

namespace eSpinn {

  namespace Pole {
    const std::string CARTPOLE      (DIR_ARCHIVE + "cartpole");
    const std::string POP_EXT       (".pop");
    const std::string CHAMP_ORG     (DIR_ARCHIVE + "champ.org");
    const std::string CHAMP         (DIR_ARCHIVE + "champ");
    const std::string FILE_FORCE        (DIR_DATA + "force");
    const std::string FILE_MDL_STATES   (DIR_DATA + "states");
    constexpr int MAX_STEP = 50000;
    constexpr double FORCE_MAG = 10.0;
  }

    /* @brief: pole balancing task
     * pole balancing with/without velocity information
     * use neural networks to solve the pole balancing problem
     */
    int pole_balancing(const bool &markov);

    /* @brief: evaluate population 
     * return true if problem solved
     * evaluate each organism one by one
     * and check if problem is solved
     */
    template <typename T>
    bool evaluate(Population *pop, Injector *inj, CartPole *mdl,
        const bool &markov);

    /* @brief: evaluate organism
     * return true if org is successful
     * load inputs using Injector
     * get network output to control the plant model
     * and assign fitness value to organism
     * log system states & controller outputs when re-evaluating champ organism
     */
    template <typename T>
    bool evaluate(Organism<T> *org, Injector *inj, CartPole *mdl,
        const bool &markov,
        CartPoleLogger *log = nullptr, Logger *force_log = nullptr);

    /* @brief: convert network output to force to drive the cart */
    double process(const double &net_out, const bool &markov);

}
