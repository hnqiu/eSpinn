/* Copyright (C) 2017-2020 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#pragma once

#include "CartPole.h"
#include <vector>

/* @brief: CartPoleLogger
 * log cart-pole system states
 * archive states to file
 */
namespace eSpinn {
    class CartPoleLogger {
    private:
        /* data */
        eSpinn_size data_length;
        std::vector<double> x, x_dot, theta, theta_dot;
    public:
        /* @brief: constructor */
        CartPoleLogger() : data_length(0),
            x(), x_dot(), theta(), theta_dot() { }

        /* @brief: destructor */
        ~CartPoleLogger() = default;

        /* @brief: return size of states */
        const eSpinn_size length() const;

        /* @brief: log cart-pole system's states */
        void log_states(const CartPole &cp);

        /* @brief: clear elements */
        void clear();

        /* @brief: save system states to file */
        bool archive(const std::string &ofile);
    };
}
