/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#include "Neuron.h"
using namespace eSpinn;


/* @brief: print class info 
 * do the actual printing here
 */
std::ostream& Neuron::print(std::ostream &os) const {
    os << n_id << " " << n_seq << " " << n_layer << " ";
    return os;
}


/* @brief: check if neuron is spiking neuron */
bool Neuron::is_spike_neuron() const {
    return isSPIKING(n_type);
}


/* @brief: duplicate this neuron */
Neuron* Neuron::duplicate() {
    return new Neuron(*this);
}


/* @brief: add incoming connection */
void Neuron::add_inConn(Connection *conn) {
    in_conn.push_back(conn);
}


/* @brief: add outgoing connection */
void Neuron::add_outConn(Connection *conn) {
    out_conn.push_back(conn);
}


/* @brief: remove incoming connection */
void Neuron::remove_inConn(Connection *const conn) {
    bool deleted(false);
    for (auto c = in_conn.begin(); c != in_conn.end(); ++c) {
        if (conn == *c) {
            in_conn.erase(c);
            deleted = true;
            break;
        }
    }
    if (!deleted)
        std::cerr << BnR_ERROR 
        << "attempt to delete nonexistent connection from neuron!" << std::endl;
}


/* @brief: remove outgoing connection */
void Neuron::remove_outConn(Connection *const conn) {
    bool deleted(false);
    for (auto c = out_conn.begin(); c != out_conn.end(); ++c) {
        if (conn == *c) {
            out_conn.erase(c);
            deleted = true;
            break;
        }
    }
    if (!deleted)
        std::cerr << BnR_ERROR 
        << "attempt to delete nonexistent connection from neuron!" << std::endl;
}


/* @brief: transmit output to all outgoing connections */
void Neuron::transmit(const double &val) {
    for (auto &conn : out_conn) {
        conn->pushReceptor(val);
    }
}


/* @brief: handle all incoming plastic connections */
void Neuron::plasticify_preConn() {
    for (auto &c : in_conn) {
        c->updateWeight();
    }
}
