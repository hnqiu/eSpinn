/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#include "Sensor.h"
using namespace eSpinn;


/* @brief: print class info 
 * do the actual printing here
 */
std::ostream& Sensor::print(std::ostream &os) const {
    os << "sensor ";
    Neuron::print(os);
    return os;
}


/* @brief: duplicate this neuron */
Sensor* Sensor::duplicate() {
    return new Sensor(*this);
}


/* @brief: load input data
 * use this method for input-layer neurons
 */
void Sensor::load_input(const double *val) {
    sense_val = *val;
    rectify();
}


/* @brief: load input data
 * use this method for non-input-layer neurons
 * accumulate synaptic inputs
 */
void Sensor::load_input() {
    if (n_layer != L_INPUT) {
        double tmp = .0;
        for (auto &c : in_conn) {
            tmp += c->getWeight() * c->getRecentReceptor();
            #ifdef ESPINN_VERBOSE
            std::cout << "Connection #" << c->getID()
                << "'s recent receptor is " << c->getRecentReceptor() << std::endl;
            #endif
        }
        sense_val = tmp;
        rectify();
    }
}


/* @brief: cap sensor value within [-1.0, 1.0] */
void Sensor::rectify() {
    if (sense_val > 1.0)
        sense_val = 1.0;
    else if (sense_val < -1.0)
        sense_val = -1.0;
}


/* @brief: forward neuron 
 * first accumulate synaptic inputs
 * then call transmit()
 */
void Sensor::forward() {
    load_input();
    transmit(sense_val);
    plasticify_preConn();
}
