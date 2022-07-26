/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#include "Innovation.h"
using namespace eSpinn;


/* @brief: overloaded <<
 * print class info
 */
std::ostream& eSpinn::operator<<(std::ostream &os, const Innovation &inno) {
    os << "Inno: " << inno.i_type 
        << " " << inno.inodeid << " " << inno.onodeid
        << " " << inno.old_connid << " " << inno.new_nodeid
        << " " << inno.new_connid << " " << inno.new_connid2
        << " " << inno.new_weight << " " << inno.new_conn_type;
    return os;
}
