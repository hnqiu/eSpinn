/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#include "NetworkBase.h"
using namespace eSpinn;


/* @brief: print class info 
 * do the actual printing here
 */
std::ostream& NetworkBase::print(std::ostream &os) const {
    os << net_id << " ";
    return os;
}


/* @brief: get network id */
const netID NetworkBase::getID() const {
    return net_id;
}

/* @brief: get network id */
void NetworkBase::setID(const netID &i) {
    net_id = i;
}
