/* Copyright (C) 2017-2020 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#pragma once

#include <iostream>

/* @brief: Plant
 * linearized UAV heave model
 */
namespace eSpinn {
    class Plant {
    private:
        const double kt = -2.7653, kv = -0.7670, b = 9.8175, g = 9.81;
        double dt; // time interval in second
        double acc, vel, pos;
    public:
        // internal status boundaries
        // if the state variables exceed the following, solutions are infeasible
        // simulation will be terminated
        static constexpr double velRANGE[2] = {-4.0, 4.0};
        static constexpr double posRANGE[2] = {-4.0, 4.0};

        /* @brief: constructor */
        Plant(const double &del_t = 0.01) :
            dt(del_t),
            acc(.0), vel(.0), pos(.0)
        {
            #ifndef NDEBUG
            std::cout << "Plant model created." << std::endl;
            #endif
        }
        /* @brief: destructor */
        ~Plant() = default;

        /* @brief: get current acceleration */
        const double getAcc() const;

        /* @brief: get current velocity */
        const double getVel() const;

        /* @brief: get current position */
        const double getPos() const;

        /* @brief: run plant model to get current status
         * system dynamic
         * a(t) = kt * f(t) + kv * v(t) + b
         * v(t+1) = v(t) + a(t) * dt
         * s(t+1) = s(t) + v(t+1) * dt
         */
        bool run(const double &force);

        /* @brief: reset status */
        void reset();

        /* @brief: cap state variables */
        void rectify();

        /* @brief: check if state variables out of boundary */
        bool check();
    };
}
