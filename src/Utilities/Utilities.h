/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#pragma once


#include "eSpinn_def.h"
#include <random>
#include <memory>
#include <string>
#include <iostream>
#include <fstream>

namespace eSpinn {
    // bold & red 'Error'
    const std::string BnR_ERROR("\033[1;31mError: \033[0m");

    /* return bold & red text
     * \033: 'ESC' character
     * color codes are between '[' & 'm', seperated by ';'
     * 1: bold; 4: underline
     * 31: red text
     * 0: reset to normal
     */
    inline const std::string emphasizeText(const std::string &text) {
        return "\033[1;31m" + text + "\033[0m";
    }


    /* fast inverse square root
     * return 1/sqrt(x)
     * using magic number - https://en.wikipedia.org/wiki/Fast_inverse_square_root
     */
    const float fast_rsqrt(const float &x);

    /* fast square root
     * return sqrt(x) using magic number
     * for other methods 
     * https://www.codeproject.com/Articles/69941/Best-Square-Root-Method-Algorithm-Function-Precisi
     */
    const float fast_sqrt(const float &x);


    /* return a random real value between [0, 1]
     * using uniform distribution
     */
    inline const double rand() {
        static std::mt19937 e;
        static std::uniform_real_distribution<double> u(.0, 1.0);
        return u(e);
    }

    /* return a random integer between [min, max]
     * using uniform distribution
     */
    inline const int rand(const int &min, const int &max) {
        std::random_device rd{};
        std::mt19937 e{rd()};
        // static std::mt19937 e;
        // static std::default_random_engine e;
        std::uniform_int_distribution<int> u(min, max);
        return u(e);
    }

    /* return a random real value between [min, max]
     * using uniform distribution
     */
    inline const double rand(const double &min, const double &max) {
        std::random_device rd{};
        std::mt19937 e{rd()};
        std::uniform_real_distribution<double> u(min, max);
        return u(e);
    }

    /* return a random real value with mean and deviation
     * using normal distribution
     */
    inline const double rand_normal(const double &mean, const double &dev) {
        static std::mt19937 e;
        std::normal_distribution<double> u(mean, dev);
        return u(e);
    }

    /* return a smart pointer to a vector
     * of random real values with mean and deviation
     * using normal distribution
     */
    std::shared_ptr<std::vector<double>> rand_normal(const double &mean, const double &dev, const eSpinn_size &s);


    /* return a random weight value between [-1.0, 1.0] */
    inline const double randWeight() {
        static std::mt19937 e;
        static std::uniform_real_distribution<double> u(-1.0, 1.0);
        return u(e);
    }

    /* return a random lambda value between [MIN_LAMBDA, MAX_LAMBDA] */
    inline const double randLambda() {
        static std::mt19937 e;
        static std::uniform_real_distribution<double> u(params::MIN_LAMBDA, params::MAX_LAMBDA);
        return u(e);
    }

    /* return a random int value between [0, MAX_DELAY] */
    inline const synDel randDelay() {
        if (params::MAX_DELAY)
            return eSpinn::rand(1, params::MAX_DELAY);
        else
            return 1; //TODO - throw
    }


    /* return a random value for plastic Connection */
    inline const double rand_plastic_term() {
        static std::mt19937 e;
        static std::uniform_real_distribution<double> u(-1.0, 1.0);
        return u(e);
    }


	/* return the size of an array (nums of elements) */
    template<typename T, unsigned N>
    eSpinn_size size_of(const T (&t)[N]) {
        return sizeof(t)/sizeof(*t);
    }
    // the following will always return 1
    // eSpinn_size size_of(const T *t)
    // eSpinn_size size_of(const T t[])
    // because they will just pass the pointer to the function


	/* print out elements in vector, seperated by " " */
    template <typename T>
    std::ostream& operator<<(std::ostream &os, const std::vector<T> &t) {
        for (auto &elem : t) {
            os << elem << " ";
        }
        return os;
    }

    /* save elements in vector, seperated by "\n"
     * use this method for file streams
     */
    template <typename T>
    std::ofstream& operator<<(std::ofstream &os, const std::vector<T> &t) {
        for (auto &elem : t) {
            os << elem << std::endl;
        }
        return os;
    }

    /* @brief: save vector to file */
    bool archive(const std::vector<double> &v, const std::string &ofile);
}
