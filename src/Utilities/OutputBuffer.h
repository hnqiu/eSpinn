/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#pragma once
#include "CircularBuffer.h"

/* @brief: output buffer
 * network output filtering - moving average
 */
namespace eSpinn {
    class OutputBuffer : public CircularBuffer<double> {
    private:
        /* data */
    public:
        OutputBuffer(const std::size_t n) : CircularBuffer<double>(n) { }
        ~OutputBuffer() { }

        /* @brief: accumulate elements */
        const double accumulate() const;

        /* @brief: calculate the mean of elements */
        const double mean() const;
    };
}
