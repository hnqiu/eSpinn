/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#include "IzhiNeuron.h"
using namespace eSpinn;


/* @brief: print class info 
 * do the actual printing here
 */
std::ostream& IzhiNeuron::print(std::ostream &os) const {
    os << "izhineuron ";
    SpikeNeuron::print(os);
    os << a << " " << b << " " << c << " " << d << " ";
    return os;
}


/* @brief: duplicate this neuron */
IzhiNeuron* IzhiNeuron::duplicate() {
    return new IzhiNeuron(*this);
}


/* @brief: reset neuron status */
void IzhiNeuron::reset() {
    SpikeNeuron::reset();
    v = c;
    u = b*v;
}


/* @brief: get the unspiked portion of the membrane potential */
const double IzhiNeuron::get_unspiked_potential() const {
    return (v-c) / (thresh-c);
}


/* @brief: step neuron for ONE timestep */
void IzhiNeuron::step() {
    double dv = 0.04 * (v*v) + 5.0 * v + 140.0 - u + inc;
    double du = a * (b * v - u);

    v += dv;
    u += du;

    if (v >= thresh) {
        spike = 1;
        v = c;
        u += d;
    } else {
        spike = 0;
    }
    pushSpike(spike);
}
