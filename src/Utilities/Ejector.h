/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#pragma once

#include "eSpinn_def.h"
#include "Utilities.h"
#include <vector>
#include <iostream>
#include <fstream>

/* @brief: Ejector 
 * archive normalized training output data
 * initialization list: size of output data set
 */
namespace eSpinn {
    class Ejector
    {
        friend class Gate;
    private:
        /* data */
        const eSpinn_size data_width;
        eSpinn_size data_length;
        double *norm_scaler, *shift, *data_set;
        std::vector<std::vector<double>*> data_norm, net_outp, data_act;
        double *mse;
    public:
        /* @brief: constructor */
        Ejector(const eSpinn_size &width) :
            data_width(width), data_length(0),
            norm_scaler(new double[width]), shift(new double[width]),
            data_set(new double[width]), 
            data_norm(), net_outp(), data_act() 
            , mse(new double[width])
        {
            #ifdef ESPINN_VERBOSE
            std::cout << "Constructing Ejector, data width = " << width << std::endl;
            #endif
        }

        /* @brief: deleted copy constructor */
        Ejector(const Ejector&) = delete;
        /* @brief: no assignment */
        Ejector &operator=(const Ejector&) = delete;

        /* @brief: destructor */
        ~Ejector() {
            delete [] norm_scaler;
            delete [] shift;
            delete [] data_set;
            delete [] mse;
            for (auto &d : data_norm)
                delete d;
            for (auto &d : net_outp)
                delete d;
            for (auto &d : data_act)
                delete d;
        }

        /* @brief: get output channel width */
        inline const eSpinn_size width() const;

        /* @brief: get data length */
        inline const eSpinn_size length() const;

        /* @brief: set data length */
        void setLength(const eSpinn_size &length);

        /* @brief: set normalization factors for all data type */
        void setNormFactors(const std::vector<double> &min, const std::vector<double> &max);

        /* @brief: set normalization factors for all data type */
        void setNormFactors(const double *min, const double *max, const eSpinn_size &n);

        /* @brief: set normalization factors for ONE data type */
        void setNormFactors(const double &min, const double &max, const eSpinn_size &which);

        /* @brief: normalize training data */
        void normalize();

        /* @brief: get norm data_set at the timestep */
        const double *const get_data_set(const eSpinn_size &ts) const;

        /* @brief: archive network output data to ejector */
        void eject_net_outp(const std::vector<double>& outp, const eSpinn_size &ts);

        /* @brief: calculate normalized mean square errors for each output channel
         * and return the averaged mse
         */
        const double cal_mse();

        /* @brief: denormalize network outputs
         * and archive to data_act
         */
        void denormalize_net_outp();

        /* @brief: save ejector output data_act to file
         * ONE data channel at each column
         * data are saved in time-serial order
         */
        void archive_act_output(const std::string &filename);
    };
}
