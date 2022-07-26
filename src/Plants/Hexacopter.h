/* Copyright (C) 2021 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#pragma once

#include <iostream>

/* @brief: Hexacopter
 * linearized hexacopter heave model
 */
namespace eSpinn {
    class Hexacopter {
    private:
        const double kt = 15.5, kv = -0.052, b = .3; // acc ~ thr + rate
        const double kb = -0.05274, b_h = 0.87942; // hover ~ batt
        const double kb_t = 0.0049109; // battery voltage decrease rate vs. time
        double dt; // time interval in second
        double acc, vel, pos;
        double hover; // hover thr
        double batt, init_batt;
    public:
        // internal status boundaries
        static constexpr double posMAX = 2.0;
        static constexpr double velRANGE[2] = {-1.0, 1.0};
        static constexpr double pos_errRANGE[2] = {-.2, .2};
        static constexpr double battRange[2] = {9.0, 12.0};
        // if pos exceeds the following, solutions are infeasible
        // simulation will be terminated
        static constexpr double posERR[2] = {-1.0, 3.0};

        /* @brief: constructor */
        Hexacopter(const double &del_t = 0.01, const double &init_b = 11.5) :
            dt(del_t),
            acc(.0), vel(.0), pos(.0),
            hover(init_b * kb + b_h),
            batt(init_b), init_batt(init_b)
        {
            #ifndef NDEBUG
            std::cout << "Hexacopter model created." << std::endl;
            #endif
        }
        /* @brief: destructor */
        ~Hexacopter() = default;

        /* @brief: get current acceleration */
        const double getAcc() const;

        /* @brief: get current velocity */
        const double getVel() const;

        /* @brief: get current position */
        const double getPos() const;

        /* @brief: get current hover value */
        const double getHover() const;

        /* @brief: get approximated hover value from current batt */
        const double getApproxHover(const double &thr) const;

        /* @brief: get current battery voltage */
        const double getBatt() const;

        /* @brief: set battery voltage */
        void setBatt(const double &b);

        /* @brief: run plant model to get current status
         * system dynamic
         * a(t) = kt * thr(t) + kv * v(t) + kb * batt + b
         * v(t+1) = v(t) + a(t) * dt
         * p(t+1) = p(t) + v(t+1) * dt
         * batt(t+1) = batt(t) - kb_t * dt
         */
        bool run(const double &thr);

        /* @brief: reset status */
        void reset();

        /* @brief: cap state variables */
        void rectify();

        /* @brief: check if state variables out of boundary */
        bool check();
    };
}
