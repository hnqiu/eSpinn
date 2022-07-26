/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#include "Connection.h"
#include "Neuron.h"
using namespace eSpinn;

double params::eta(1.0);

/* @brief: print class info 
 * do the major printing here
 */
std::ostream& Connection::_print(std::ostream &os) const {
    os << c_id << " " << in_node->getID() << " " << out_node->getID() << " "
        << weight << " " << synapse_delay << " " << enable << " ";
    os << hebb << " " << plastic_module << " ";
    return os;
}

/* @brief: print class info 
 * print different header if the class is inherited
 */
std::ostream& Connection::print(std::ostream &os) const {
    os << "connection ";
    _print(os);
    return os;
}


/* @brief: duplicate this connection */
Connection* Connection::duplicate() {
    return new Connection(*this);
}


/* @brief: get input neuron id */
const neuronID Connection::getInodeID() const { return in_node->getID(); }

/* @brief: get input neuron id */
const neuronID Connection::getOnodeID() const { return out_node->getID(); }


/* @brief: set input neuron */
void Connection::setInode(Neuron *const n) {
    in_node = n;
}

/* @brief: set output neuron */
void Connection::setOnode(Neuron *const n) {
    out_node = n;
}


/* @brief: get input neuron */
Neuron *const Connection::getInode() const {
    return in_node;
}

/* @brief: get output neuron */
Neuron *const Connection::getOnode() const {
    return out_node;
}


/* @brief: get input neuron seq */
const eSpinn_size Connection::getInodeSeq() const {
    return in_node->getSeq();
}

/* @brief: get output neuron seq */
const eSpinn_size Connection::getOnodeSeq() const {
    return out_node->getSeq();
}


/* @brief: get connection weight */
const double Connection::getWeight() const { return weight; }

/* @brief: set connection weight */
void Connection::setWeight(const double &val) { weight = val; }

/* @brief: increase connection weight */
void Connection::increaseWeight(const double &val) { weight += val; }


/* @brief: weight change for plastic connections
 * update connection weight via Hebbian learning 
 */
void Connection::updateWeight() {
    double dw = .0;
    switch (hebb) {
    case RateHebbian: {
        double uj = in_node->getOut(); // Hebbian pre component uj from in_node
        double ui = out_node->getOut(); // Hebbian post component ui from out_node
        if (out_node->getType() == SENSOR) {
            ui = (ui+1.0) / 2.0;
            if (ui > 1.0) {
                ui = 1.0;
            }
            else if (ui < .0) {
                ui = .0;
            }
        }
        // dw = eta * eta * eta * (k[0]*k[0]*k[0]*uj*ui +k[1]*k[1]*k[1]*uj +k[2]*k[2]*k[2]*ui);
        // 0.047 = pow(-0.95, 3.0) * pow(-0.38, 3.0)
        // dw = 0.46 * 0.047 * ui;
        double neg_num = 0.005*plastic_module.mag*(uj-ui+plastic_module.corr)+params::Am;
        dw = params::eta * ui *
            (params::Ap/(params::inv_tau_p+ui) + neg_num/(params::inv_tau_m+ui));
        weight += dw;
        capWeight();
        break;
    }
    case SpikeSTDP:
        // spike bsed STDP TODO
        weight += dw;
        break;
    
    case NoHebbian:
    default:
        break;
    }
}


/* @brief: back up weight during Hebbian rule development */
void Connection::backupWeight() {
    weight_pre = weight;
}


/* @brief: restore weight after Hebbian rule development */
void Connection::restoreWeight() {
    weight = weight_pre;
}


/* @brief: cap weight between [-MAX_WEIGHT, MAX_WEIGHT] */
void Connection::capWeight() {
    if (weight > params::MAX_WEIGHT)
        weight = params::MAX_WEIGHT;
    else if (weight < -params::MAX_WEIGHT)
        weight = -params::MAX_WEIGHT;
}


/* @brief: set Hebbian type */
void Connection::set_hebb_type(const HebbianType &h) {
    hebb = h;
}


/* @brief: get Hebbian type */
HebbianType Connection::get_hebb_type() {
    return hebb;
}


/* @brief: cap plastic terms */
void Connection::cap_plastic_terms() {
    plastic_module.cap_plastic_terms();
}


/* @brief: get plastic term */
const double Connection::get_plastic_term(const eSpinn_size &which) const {
    return plastic_module.get_plastic_term(which);
}


/* @brief: set plastic terms */
void Connection::set_plastic_term(const double &val, const eSpinn_size &which) {
    plastic_module.set_plastic_term(val, which);
}


/* @brief: increase plastic term */
void Connection::increase_plastic_term(const double &val, const eSpinn_size &which) {
    plastic_module.increase_plastic_term(val, which);
}


/* @brief: copy plasticity rule */
void Connection::copy_plasticity_module(const Connection *c) {
    plastic_module = c->plastic_module;
}


/* @brief: get connection synaptic delay */
const synDel Connection::getDelay() const { return synapse_delay; }

/* @brief: set connection synaptic delay */
void Connection::setDelay(const double &val) { synapse_delay = val; }


/* @brief: get connection enable status */
const bool Connection::isEnable() const {
    return enable;
}

/* @brief: set connection enable status */
void Connection::setEnable(const bool &status) {
    enable = status;
}


/* @brief: push output from in_node */
void Connection::pushReceptor(const double &r) {
    receptor.push_front(r);
    while(!receptor.empty() && receptor.size() != synapse_delay){
        receptor.pop_back();
    }    
}

/* @brief: get the delayed recent receptor */
const double Connection::getRecentReceptor() const {
    if (receptor.size())
        return receptor.back();
    else {
        #ifdef ESPINN_VERBOSE
        std::cerr << "Connection receptor from neuron #" << getInodeID() 
            << " to neuron #" << getOnodeID() << " is empty!" << std::endl;
        #endif
        return .0;
    }
}
