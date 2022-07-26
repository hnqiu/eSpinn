/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#pragma once

#include "eSpinn.h"
#include <iostream>
#include <fstream>
#include <memory>
#include <algorithm>
#include <chrono>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

namespace eSpinn {
    int test_utilities();
    int test_circularbuffer();
    
    int build_node();
    int serialize_node();

    int build_net();
    int copy_net();
    int print_net();

    int run_spikeNet();
    int run_sigmNet();
    int run_hybridNet();

    int serialize_net();

    int sort_org();
    int copy_org();
    int serialize_org();
    int test_org();

    int test_gate();
    int test_injector();

    int create_pop();
    int serialize_innovation();
    int serialize_species();
    int serialize_pop();
    int test_pop_archive();
}
