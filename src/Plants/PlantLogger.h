/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#pragma once

#include "Utilities/Utilities.h"
#include <vector>
#include <string>
#include <fstream>
#include <cmath>

/* @brief: PlantLogger
 * archive reference signal from file & actual system output
 * save output to file
 */
namespace eSpinn {
    class PlantLogger
    {
    private:
        /* data */
        eSpinn_size data_length;
        std::vector<double> val_ref, val_act, err;
    public:
        /* @brief: constructor - allocate memory */
        PlantLogger(const std::size_t &capacity);

        /* @brief: constructor */
        PlantLogger() : val_ref(), val_act(), err() { }

        /* @brief: destructor */
        ~PlantLogger() = default;

        /* @brief: load reference signal from file
         * and assign to val_ref
         * use this method when memory has been allocated
         */
        void assign_ref_signal(const std::string &inp_file);

        /* @brief: load reference signal from file
         * and allocate memory
         */
        void load_ref_signal(const std::string &inp_file);

        /* @brief: return size of signal */
        std::vector<double>::size_type length() const;

        /* @brief: log system's actual output */
        void log_act(const eSpinn_size &ts, const double &o);

        /* @brief: get actual output at the timestep */
        const double act_at(const eSpinn_size &ts);

        /* @brief: get error at the timestep */
        const double err_at(const eSpinn_size &ts);

        /* @brief: log error at the timestep */
        void log_err(const eSpinn_size &ts, const double &e);

        /* @brief: calculate error at the timestep */
        const double cal_err(const eSpinn_size &ts);

        /* @brief: calculate mean standard error */
        const double cal_stde();

        /* @brief: calculate mean standard error of the first N elements */
        const double cal_stde(const eSpinn_size &num);

        /* @brief: calculate mean square error */
        const double cal_mse();

        /* @brief: calculate mean square error of the first N elements */
        const double cal_mse(const eSpinn_size &num);

        /* @brief: calculate mean cubic error */
        const double cal_e3();

        /* @brief: calculate std error
         * call either of the above 3 methods to get std err
         */
        const double cal_std_err();

        /* @brief: save actual output to file */
        bool save_act(const std::string &ofile);

        /* @brief: save error to file */
        bool save_err(const std::string &ofile);
    };
}
