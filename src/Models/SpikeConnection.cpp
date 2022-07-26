/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#include "SpikeConnection.h"
#include "Neuron.h"
#include "SpikeNeuron.h"
using namespace eSpinn;


/* @brief: duplicate this connection */
SpikeConnection* SpikeConnection::duplicate() {
    return new SpikeConnection(*this);
}


/* @brief: print class info 
 */
std::ostream& SpikeConnection::print(std::ostream &os) const {
    os << "spikeconnection ";
    _print(os);
    os << spikeFactor << " ";
    return os;
}
