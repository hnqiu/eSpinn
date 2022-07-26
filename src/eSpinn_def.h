/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


/* @brief: definitions and parameters
 * include this in implementor files
 */
#pragma once


namespace eSpinn {

    typedef unsigned int neuronID;
    typedef unsigned int connID;
    typedef unsigned int netID;
    typedef unsigned int synDel;
    typedef unsigned int eSpinn_size;

    /* @brief: neuron types */
    enum neuronType {
        UNDEFINED = 0,
        SENSOR = 1,
        SIGMOID = 2,
        BIAS = 3,
        SPIKING = 10,
        LIF,
        IZHIKEVICH,
    };

    /* check if neuron is SPIKING */
    inline bool isSPIKING(const neuronType &t) {
        return t >= SPIKING;
    }

    /* @brief: connection types */
    enum connType {
        DEFAULTCONN = 0,
        SPIKECONN = 1
    };

    /* @brief: connection types */
    enum HebbianType {
        NoHebbian = 0,
        RateHebbian = 1,
        SpikeSTDP
    };

    enum neuronLayer {
        L_INPUT = 1,
        L_HIDDEN = 2,
        L_OUTPUT = 3
    };

    enum dataLabel {
        INPUTDATA = 0,
        OUTPUTDATA
    };

    namespace params {
        const double izhi_a = 0.02;
        const double izhi_b = 0.2;
        const double izhi_c = -50;
        const double izhi_d = 2;
        const double izhi_thresh = 30.0;

        constexpr double lif_vth = -50;
        constexpr double lif_vrest = -65;
        constexpr double lif_tau = 2; //15; //4;
        constexpr double lif_R = 1; //2; //1; // * currentFactor

        const double sigm_lambda = 2.0;

        constexpr double currentFactor = 50.0;
        constexpr double MAX_WEIGHT = 1.0;
        constexpr double MAX_LAMBDA = 6.0;
        constexpr double MIN_LAMBDA = 1.0;
        constexpr synDel MAX_DELAY = 1;
        constexpr synDel NO_DELAY = 1;

        /* @brief: Hebbian window params */
        // amplitude of pre-post & post-pre part of STDP window
        constexpr double Ap = 0.1;
        constexpr double Am = -0.1;
        // decay time constant of pre-post & post-pre of STDP window
        constexpr double tau_p = 0.02;
        constexpr double tau_m = 0.02;
        constexpr double inv_tau_p = 1.0f/tau_p;
        constexpr double inv_tau_m = 1.0f/tau_m;

        const int TIMESTEP = 50;

        constexpr double std_fit = 0.98;

        extern double eta;
    }
}
