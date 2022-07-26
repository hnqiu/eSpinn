/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#pragma once

#include "eSpinn_def.h"
#include "Utilities.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <boost/serialization/access.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

/* @brief: Injector 
 * normalized values will be injected to networks at every timestep
 * initialization list: size of data set
 */
namespace eSpinn {
    class Injector {
        friend class Gate;

        /* @brief: declare serialization library as friend
         * used to grant to the serialization library access to class members
         */
        friend class boost::serialization::access;

	    /* @brief: template member serialize function
         * called when using i/oarchive to load/write class members via serialization
         * make serialize() private to avoid being called directly 
         * when called, an instance of this class will first be constructed
         * Note we cannot archive data_width since it is const,
         * therefore Injector has no default constructor, we need to 
         * override function templates load_construct_data and save_construct_data
         */
        template<typename Archive>
        void serialize(Archive &ar, const unsigned int version) {
            #ifdef ESPINN_SERIALIZE_VERBOSE
            std::cout << "Serializing Injector ... " << std::endl;
            #endif
            ar & data_length;
            // cannot serialize 'new'ed array this way
            // ar & norm_scaler & shift;
            for (eSpinn_size i = 0; i < width(); ++i) {
                ar & norm_scaler[i] & shift[i];
            }
        }

        /* @brief: overloaded <<
         * deligate printing to member function print()
         */
        friend std::ostream& operator<<(std::ostream &os, const Injector &inj) {
            return inj.print(os);
        }
        /* @brief: print class info
         * do the actual printing here
         */
        std::ostream& print(std::ostream &os) const;
    private:
        /* data */
        const eSpinn_size data_width;
        eSpinn_size data_length;
        double *norm_scaler, *shift, *data_set;
        std::vector<std::vector<double>*> data_norm;
    public:
        /* @brief: constructor */
        Injector(const eSpinn_size &width) :
            data_width(width), data_length(0),
            norm_scaler(new double[width]), shift(new double[width]),
            data_set(new double[width+1]) // data_set[width] = bias
        {
            #ifdef ESPINN_VERBOSE
            std::cout << "Constructing Injector, data size = " << width << std::endl;
            #endif
            data_set[width] = 1; // set bias
            // default norm factors
            for (eSpinn_size w = 0; w < data_width; ++w) {
                norm_scaler[w] = 1;
                shift[w] = 0;
            }
        }

        /* @brief: deleted copy constructor */
        Injector(const Injector&) = delete;
        /* @brief: no assignment */
        Injector &operator=(const Injector&) = delete;

        /* @brief: move constructor */
        Injector(Injector && inj) noexcept :
            data_width(inj.data_width), data_length(inj.data_length),
            norm_scaler(inj.norm_scaler), shift(inj.shift),
            data_set(inj.data_set), data_norm(inj.data_norm)
        {
            #ifdef ESPINN_VERBOSE
            std::cout << "Calling move constructor" << std::endl;
            #endif
            // avoid multiple free
            inj.norm_scaler = nullptr;
            inj.shift = nullptr;
            inj.data_set = nullptr;
            inj.data_norm.clear();
        }

        /* @brief: deleted move assignment */
        Injector& operator=(Injector&&) = delete;

        /* @brief: destructor */
        ~Injector() {
            #ifdef ESPINN_VERBOSE
            std::cout << "Deleting Injector" << std::endl;
            #endif
            delete [] norm_scaler;
            delete [] shift;
            delete [] data_set;
            for (auto &d : data_norm)
                delete d;
        }

        /* @brief: size of data */
        inline const eSpinn_size width() const {
            return data_width;
        }

        /* @brief: get data length */
        inline const eSpinn_size length() const {
            return data_length;
        }

        /* @brief: set data length */
        inline void setLength(const eSpinn_size &length) {
            data_length = length;
        }

        /* @brief: set normalization factors for all data type */
        void setNormFactors(const std::vector<double> &min, const std::vector<double> &max);

        /* @brief: set normalization factors for all data type */
        void setNormFactors(const double *min, const double *max, const eSpinn_size &n);

        /* @brief: set normalization factors for ONE data type */
        void setNormFactors(const double &min, const double &max, const eSpinn_size &which);

        // no use for now
        /* @brief: set normalization factors from file */
        void setNormFactors(const std::string &filename);

        /* @brief: normalize training data */
        void normalize();

        /* @brief: normalize value */
        const double normalize(const eSpinn_size &which, const double &v);

        /* @brief: load normalized value to data_set
         * normalized data range between [0, 1]
         */
        void load_data(const eSpinn_size &which, const double &val);

        /* @brief: load and normalize training data at this timestep
         * normalized data range between [0, 1]
         */
        void load_data(const double *p, const eSpinn_size &n);

        /* @brief: get data_set at this timestep */
        const double *const get_data_set() const;

        /* @brief: get normalized data that will be injected at ts timestep */
        const double *const get_data_set(const eSpinn_size &ts) const;

        /* @brief: save injector to file */
        void archive(const std::string &ofile);
    };

    /* @brief: construct injector from file */
    Injector createInjector(const std::string &ifile);
}


namespace boost {
namespace serialization {
    template <typename archive>
    inline void save_construct_data(
        archive &ar, const eSpinn::Injector *t, const unsigned int file_version)
    {
        // save data required to construct instance
        ar << t->width();
    }

    template <typename archive>
    inline void load_construct_data(
        archive &ar, eSpinn::Injector *t, const unsigned int file_version)
    {
        // retrieve data from archive required for constructor
        eSpinn::eSpinn_size width;
        ar >> width;
        // invoke inplace constructor to initialize instance
        ::new(t)eSpinn::Injector(width);
    }
}
}
