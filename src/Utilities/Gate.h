/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#pragma once

#include "eSpinn_def.h"
#include "Utilities.h"
#include "Injector.h"
#include "Ejector.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <initializer_list>
#include <string>
#include <utility>

/* @brief: Gate 
 * inject encoder & output decoder
 * initialization list: 
 */
namespace eSpinn {
    class Gate;
    /* @brief: load training data from files
     * return a pointer to Gate
     * note: friend declaration specifying a default argument must be a definition
     * therefore use the default argument in non-friend declaration
     */
    Gate* load_data_from_files(
        const std::initializer_list<std::string> &inp_files, 
        const std::initializer_list<std::string> &outp_files = {});

    class Gate
    {
        /* @brief: load training data from files
         * return a pointer to Gate
         */
        friend Gate* load_data_from_files(
            const std::initializer_list<std::string> &inp_files, 
            const std::initializer_list<std::string> &outp_files);
    private:
        /* data */
        eSpinn_size inject_channel_width, output_channel_width;
        // outer: which channel; inner: sequence of data
        std::vector<std::vector<double>*> inputs_raw, outputs_raw;
        eSpinn_size data_length; // length of sequence
    public:
        Injector *injector; // data of one timestep
        Ejector *ejector; // archive of normalized training outputs
    public:
        /* @brief: constructor */
        Gate(const eSpinn_size &Iwidth, const eSpinn_size &Owidth) :
            inject_channel_width(Iwidth), output_channel_width(Owidth),
            inputs_raw(), outputs_raw(), data_length(0), 
            injector(new Injector(Iwidth)), ejector(new Ejector(Owidth)) 
        {
            #ifdef ESPINN_VERBOSE
            std::cout << "Gate constructed" << std::endl;
            #endif
        }

        /* @brief: default constructor */
        Gate() : Gate(0, 0) { }

        /* @brief: deleted copy constructor */
        Gate(const Gate&) = delete;
        /* @brief: no assignment */
        Gate &operator=(const Gate&) = delete;

        /* @brief: destructor */
        ~Gate() {
            #ifdef ESPINN_VERBOSE
            std::cout << "Destructing Gate: deleting raw data & injector..." << std::endl;
            #endif
            for (auto &d : inputs_raw)
                delete d;
            for (auto &d : outputs_raw)
                delete d;
            delete injector;
            delete ejector;
        }

        /* @brief: set channel width */
        void setWidth(const eSpinn_size &Iwidth, const eSpinn_size &Owidth);

        /* @brief: get input channel width */
        const eSpinn_size getIwidth() const;

        /* @brief: get output channel width */
        const eSpinn_size getOwidth() const;

        /* @brief: get data length */
        const eSpinn_size getLength() const;

        /* @brief: load raw data and set data length
         * if raw data from different files do not have the same size
         * print error and return
         */
        bool load_time_serialized_data(std::ifstream &ifs, const dataLabel &label);

        /* @brief: set injecting and ejecting normalizing factors */
        void set_normalizing_factors(const std::string &filename);

        /* @brief: initialize injector & ejector
         * normalize and archive training data
         * allocate actual data memory
         */
        void init();

        /* @brief: get injected data at this timestep */
        const double *const get_injector_data_set(const eSpinn_size &ts) const;

        /* @brief: get normalized ejected data at ts timestep */
        const double *const get_ejector_data_set(const eSpinn_size &ts) const;

        /* @brief: archive network output data to ejector */
        void eject_net_outp(const std::vector<double>& outp, const eSpinn_size &ts);

        /* @brief: calculate ejector mean square errors */
        const double cal_mse() const;

        /* @brief: denormalize ejector net_outp data */
        void denormalize_outp();

        /* @brief: save ejector output data_act to file
         * ONE data channel at each column
         * data are saved in time-serial order
         */
        void archive_act_output(const std::string &filename);
    };
}
