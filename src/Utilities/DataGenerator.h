/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#pragma once

#include "files_def.h"
#include "Utilities.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>

/* @brief: training data generator
 * save the data to files
 */
namespace eSpinn {
    class DataGenerator
    {
    private:
        /* data */
        std::size_t timesteps;
        double dt;
        std::size_t iwidth, owidth;
        std::vector<std::vector<double>*> inps, outps;

        /* @brief: allocate memory */
        void init();

        /* @brief: system dynamic
         * a(t) = f(t)/m + k * v(t)
         * v(t+1) = v(t) + a(t) * dt
         */
        void system_dynamic();

    public:
        /* @brief: constructor */
        DataGenerator(const std::size_t &ts, const double &t_intv = 0.01, 
            const std::size_t &Iwidth = 2, const std::size_t &Owidth = 1
            ) :
            timesteps(ts), dt(t_intv), 
            iwidth(Iwidth), owidth(Owidth), 
            inps(), outps() 
        {
            init();
        }
        /* @brief: destructor */
        ~DataGenerator() {
            for (auto &d : inps)
                delete d;
            for (auto &d : outps)
                delete d;
        }

        /* @brief: generate training data
         * based on system dynamic
         */
        void generate();

        /* @brief: save data to file */
        bool save();
    };
}
