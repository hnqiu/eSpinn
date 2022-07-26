/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#pragma once

#include "Utilities.h"
#include <vector>
#include <string>
#include <fstream>

/* @brief: Logger
 * archive vector and
 * save vector to file
 * initialization list: capacity
 */
namespace eSpinn {
    class Logger
    {
    private:
        /* data */
        std::vector<double> val;
    public:
        /* @brief: constructor */
        Logger(const std::size_t &capacity = 8) : val(std::vector<double>()) {
            val.reserve(capacity);
        }

        /* @brief: copy constructor */
        Logger(const Logger &l) : val(l.val) { }

        /* @brief: destructor */
        ~Logger() = default;

        /* @brief: return size of vector */
        std::vector<double>::size_type size() const;

        /* @brief: push back element */
        void push_back(const double &v);

        /* @brief: get data at the timestep */
        const double at(const std::size_t &ts);

        /* @brief: clear elements */
        void clear();

        /* @brief: save data to file */
        bool save(const std::string &ofile) const;

        /* @brief: append value to file */
        bool append_to_file(const double &val, const std::string &ofile) const;

        /* @brief: append an empty line to file */
        bool append_newline_to_file(const std::string &ofile) const;
    };
}
