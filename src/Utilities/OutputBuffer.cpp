/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#include "OutputBuffer.h"
using namespace eSpinn;


/* @brief: accumulate elements */
const double OutputBuffer::accumulate() const {
    double sum = .0;
    for (auto i = 0; i < this->size(); ++i) {
        sum += this->buffer[i];
    }
    return sum;
}


/* @brief: calculate the mean of elements */
const double OutputBuffer::mean() const {
    return accumulate() / this->size();
}
