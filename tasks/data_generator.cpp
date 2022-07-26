/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#include "data_generator.h"

using namespace eSpinn;

/* @brief: training data generator
 * save the data to files
 */

int main(int argc, char *argv[]) {
    std::size_t ts;
    std::cout << "Generating training data... " << std::endl;
    if (argc != 2) {
        std::cerr << "Program requires ONE parameter. Input length of data: ";
        std::cin >> ts;
    }
    else {
        ts = atol(argv[1]);
    }
    #ifndef NDEBUG
    std::cout << "data length = " << ts << std::endl;
    #endif

    return data_generator(ts);
}


/* @brief: generate training data and
 * save the data to files
 */
bool eSpinn::data_generator(std::size_t &timestep) {
    DataGenerator dg(timestep);
    dg.generate();
    return dg.save();
}
