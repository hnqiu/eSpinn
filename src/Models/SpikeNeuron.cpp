/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#include "SpikeNeuron.h"
using namespace eSpinn;


/* @brief: duplicate this neuron */
SpikeNeuron* SpikeNeuron::duplicate() {
    return new SpikeNeuron(*this);
}


/* @brief: print class info 
 * do the actual printing here
 */
std::ostream& SpikeNeuron::print(std::ostream &os) const {
    // os << "spikeneuron ";
    Neuron::print(os);
    os << thresh << " ";
    return os;
}


/* @brief: get current spike status */
const bool SpikeNeuron::getSpike() const {
    return spike;
}

/* @brief: get spike status at this timestep */
const bool SpikeNeuron::getSpike(const eSpinn_size &pos) const {
    return spike_train[pos];
}


/* @brief: push spike status */
void SpikeNeuron::pushSpike(const bool &s) {
    spike_train <<= 1;
    spike_train.set(0, s);
}


/* @brief: set spike status at pos */
void SpikeNeuron::setSpike(const eSpinn_size &pos) {
    spike_train.set(pos);
}

/* @brief: reset spike status at pos */
void SpikeNeuron::resetSpike(const eSpinn_size &pos) {
    spike_train.reset(pos);
}

/* @brief: get accumulated spike number */
const eSpinn_size SpikeNeuron::getSpikeNum() const {
    return spike_train.count();
}


/* @brief: reset neuron status */
void SpikeNeuron::reset() {
    spike = 0;
    inc = 0;
}


/* @brief: load input data
 * use this method for input-layer neurons
 */
void SpikeNeuron::load_input(const double *val) {
    inc = *val;
}


/* @brief: load input data
 * accumulate synaptic inputs
 * use this method for non-input-layer neurons
 */
void SpikeNeuron::load_input() {
    if (n_layer != L_INPUT) {
        double tmp = .0;
        for (auto &c : in_conn) {
            double factor = 1.0;
            if (c->getType() == SPIKECONN)
                factor = dynamic_cast<SpikeConnection*>(c)->getSpikeFactor();
            tmp += c->getWeight() * c->getRecentReceptor() * factor;
            #ifdef ESPINN_VERBOSE
            std::cout << "Connection #" << c->getID()
                << "'s recent receptor is " << c->getRecentReceptor() << std::endl;
            #endif
        }
        inc = tmp;
    }
}


/* @brief: get neuron output */
const double SpikeNeuron::getOut() const {
    return (getSpikeNum()+get_unspiked_potential()) / params::TIMESTEP;
}


/* @brief: transmit spike status to all outgoing SPIKING connections */
void SpikeNeuron::transmit() {
    for (auto &c : out_conn) {
        if (c->getOnode()->is_spike_neuron()) {
            c->pushReceptor(spike);
        }
    }
}


/* @brief: transmit firing rate to all outgoing NON-SPIKING connections */
void SpikeNeuron::transmit_rate() {
    double r = getOut();
    for (auto &c : out_conn) {
        if (!c->getOnode()->is_spike_neuron()) {
            c->pushReceptor(r);
        }
    }
}


/* @brief: forward SpikeNeuron for ONE timestep 
 * first accumulate synaptic inputs
 * then use step function to get output spike
 * finally, transmit output to all outgoing connections
 */
void SpikeNeuron::forward() {
    load_input();
    step();
    transmit();
    // plasticify_preConn();
}
