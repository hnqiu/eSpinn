/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#pragma once


#include "eSpinn_def.h"
#include <iostream>
#include <cassert>
#include <boost/serialization/access.hpp>

/* @brief: Connection Hebbian plasticity module
 * initialization list: 
 */
namespace eSpinn {
    class HebbPlasticity {
        friend class Connection;
        /* @brief: declare serialization library as friend
         * used to grant to the serialization library access to class members
         */
        friend class boost::serialization::access;

	    /* @brief: template member serialize function
         * called when using i/oarchive to load/write class members via serialization
         * make serialize() private to avoid being called directly 
         * when called, an instance of this class will first be constructed using default constructor
         */
        template<typename Archive>
        void serialize(Archive &ar, const unsigned int version) {
            ar & mag & corr;
        }

        /* @brief: overloaded << 
         * print member function
         */
        friend std::ostream& operator<<(std::ostream &os, const HebbPlasticity &hebb) {
            os << "hebb mag=" << hebb.mag << "; corr=" << hebb.corr;
            return os;
        }
    private:
        /* data */
        double mag, corr; // plastic terms
    public:
        // plastic term boundaries
        static constexpr double _mag[2] = {-1.0, 1.0};
        static constexpr double _corr[2] = {-1.0, 1.0};

        /* @brief: constructor  */
        HebbPlasticity() : mag(.0), corr(.0) { }

        /* @brief: copy constructor  */
        HebbPlasticity(const HebbPlasticity &h)
            : mag(h.mag), corr(h.corr) { }

        /* @brief: destructor  */
        ~HebbPlasticity() = default;

        /* @brief: overloaded assignment = */
        HebbPlasticity& operator=(const HebbPlasticity &h) {
            mag = h.mag;
            corr = h.corr;
            return *this;
        }

        /* @brief: cap plastic terms */
        void cap_plastic_terms();

        /* @brief: get plastic term */
        const double get_plastic_term(const eSpinn_size &which) const;

        /* @brief: set plastic term */
        void set_plastic_term(const double &val, const eSpinn_size &which);

        /* @brief: increase plastic term */
        void increase_plastic_term(const double &val, const eSpinn_size &which);
    };
}
