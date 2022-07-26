/* Copyright (C) 2017-2020 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#pragma once

#include "Utilities/Utilities.h"
#include <cmath>

/* @brief: CartPole
 * cart pole system
 */
namespace eSpinn {
    class CartPoleLogger;
    class CartPole {
        friend class CartPoleLogger;
    private:
        /* math constants */
        const double _4THIRDS = 1.33333333;
        const double _1deg = M_PI / 180.0;
        const double _3deg = 3 * _1deg;
        const double _12deg = 12 * _1deg;
        const double _0m3 = 0.3;
        const double _2m4 = 2.4;
        /* system params */
        const double G = 9.8;
        const double CART_MASS = 1.0;
        const double POLE_MASS = 0.1;
        const double TOTAL_MASS = CART_MASS + POLE_MASS;
        const double POLE_L = 0.5;
        /* data */
        double tau;              // time step in second
        double x, x_dot;         // cart position and velocity
        double theta, theta_dot; // pole angle and angular rate

    public:
        // state variables should not exceed the following boundaries
        const double xRANGE[2] = {-_2m4, _2m4};
        const double xdotRANGE[2] = {-1.0, 1.0};
        const double thetaRANGE[2] = {-_12deg, _12deg};
        const double thetadotRANGE[2] = {-1.5, 1.5};
        /* @brief: constructor */
        CartPole(const double &del_t = 0.01) :
            tau(del_t),
            x(.0), x_dot(.0), theta(.0), theta_dot(.0)
        {
            rand_init();
            #ifndef NDEBUG
            std::cout << "Cart pole system created." << std::endl;
            #endif
        }
        /* @brief: destructor */
        ~CartPole() = default;

        /* @brief: random initialization
         * randomly initialize system states
         */
        void rand_init();

        /* @brief: reset system states */
        void reset();

        /* @brief: get current states */
        const std::vector<double> get_states() const;

        /* @brief: update the system states */
        bool update(const double &force);

        /* @brief: check system states
         * return false if state variable is out of boundary
         */
        bool check();

    };
}
