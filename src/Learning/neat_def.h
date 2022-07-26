/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#pragma once


#include "eSpinn_def.h"

namespace eSpinn {
    typedef unsigned int orgID;
    typedef unsigned int speciesID;
    typedef int innoID;

    namespace params {
        const eSpinn_size episode = 50;
        const eSpinn_size print_every = 5;
        const eSpinn_size pop_size = 150;
    }

    namespace neat {
        enum innoType {
            NEWNODE = 0,
            NEWCONN = 1,
            NEWNODE_IN2OUT = 2
        };

        const double survival_thresh = 0.2;
        const eSpinn_size dropoff_age = 15;
        constexpr eSpinn_size stagnant_gen = 12;

        const double compat_threshold = 3.0;
        const double disjoint_coeff = 1.0;
        const double excess_coeff = 1.0;
        const double weightdiff_coeff = 0.4;
        const double delaydiff_coeff = 0.1;
        const double lambdadiff_coeff = 0.1;

        constexpr double mutate_only_prob = 0.5;
        constexpr double mate_within_species_rate = 0.8;

        // for NEAT only: 0.02, 0.01, .0
        // for adding feed-forward neurons & recurrent conns: .01, .0, .01
        // for NEAT & feed-forward: .01, .005, .01
        constexpr double add_conn_prob = 0.02;
        constexpr double add_node_prob = 0.01;
        constexpr double add_ff_node_prob = 0.005; // feed-forward topology

        constexpr double mutate_weight_prob = 0.8;
        constexpr double mutate_lambda_prob = 0.8;
        constexpr double creep_mutate_prob = 0.9;
        constexpr double mutate_weight_min = 0.01;
        constexpr double mutate_lambda_min = 0.05;

        constexpr double mutate_plasticity_prob = 0.8;
        constexpr double plasticity_creep_mutate_prob = 0.9;
        constexpr double mutate_plasticity_min = 0.01;
    }
}
