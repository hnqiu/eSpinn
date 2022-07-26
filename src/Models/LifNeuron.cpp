/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#include "LifNeuron.h"
using namespace eSpinn;


/* @brief: print class info 
 * do the actual printing here
 */
std::ostream& LifNeuron::print(std::ostream &os) const {
    os << "Lifneuron ";
    SpikeNeuron::print(os);
    os << v_rest << " " << tau << " " << R << " ";
    return os;
}


/* @brief: duplicate this neuron */
LifNeuron* LifNeuron::duplicate() {
    return new LifNeuron(*this);
}


/* @brief: reset neuron status */
void LifNeuron::reset() {
    SpikeNeuron::reset();
    v = v_rest;
}


/* @brief: get the unspiked portion of the membrane potential */
const double LifNeuron::get_unspiked_potential() const {
    return (v-v_rest) / (thresh-v_rest);
}


/* @brief: step neuron for ONE timestep */
void LifNeuron::step() {
    double dv = (R * inc - v + v_rest) / tau;
    v += dv;

    if (v >= thresh) {
        spike = 1;
        v = v_rest;
    } else {
        spike = 0;
    }
    pushSpike(spike);
}
