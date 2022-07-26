/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#include "SigmNeuron.h"
using namespace eSpinn;


/* @brief: print class info 
 * do the actual printing here
 */
std::ostream& SigmNeuron::print(std::ostream &os) const {
    os << "sigmneuron ";
    Neuron::print(os);
    os << lambda << " ";
    return os;
}


/* @brief: duplicate this neuron */
SigmNeuron* SigmNeuron::duplicate() {
    return new SigmNeuron(*this);
}


/* @brief: load input data
 * use this method for input-layer neurons
 */
void SigmNeuron::load_input(const double *val) {
    i = *val;
}


/* @brief: load input data
 * use this method for non-input-layer neurons
 * accumulate synaptic inputs
 */
void SigmNeuron::load_input() {
    if (n_layer != L_INPUT) {
        double tmp = .0;
        for (auto &c : in_conn) {
            tmp += c->getWeight() * c->getRecentReceptor();
            #ifdef ESPINN_VERBOSE
            std::cout << "Connection #" << c->getID()
                << "'s recent receptor is " << c->getRecentReceptor() << std::endl;
            #endif
        }
        i = tmp;
    }
}


/* @brief: activate neuron and get output */        
const double SigmNeuron::activate() {
    o = 1 / (1+std::exp(-i*lambda));
    // o = i;
    return o;
}

/* @brief: forward neuron 
 * first accumulate synaptic inputs
 * then call activate() to get neuron output
 * finally, transmit output to all outgoing connections
 */
void SigmNeuron::forward() {
    load_input();
    activate();
    transmit(o);            
    plasticify_preConn();
}
