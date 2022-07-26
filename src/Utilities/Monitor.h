/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#pragma once


#include "eSpinn_def.h"
#include <iostream>
#include <fstream>
#include <sstream>

/* @brief: Monitor 
 * print class info on screen
 * initialization list: cout, sep
 */
namespace eSpinn {
    template <typename T>
    class Print
    {
    private:
        /* data */
        std::ostream &os;
        char sep;
    public:
        Print(std::ostream &o = std::cout, const char c = ' ') :
            os(o), sep(c) { }
        ~Print() = default;

        void operator() (const T &t) const { os << t << sep; }
        // TODO: overload operator<< in T
    };
}
