/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#include "CartPoleLogger.h"
using namespace eSpinn;


/* @brief: return size of states */
const eSpinn_size CartPoleLogger::length() const {
    return data_length;
}


/* @brief: log cart-pole system's states */
void CartPoleLogger::log_states(const CartPole &cp) {
    x.push_back(cp.x);
    x_dot.push_back(cp.x_dot);
    theta.push_back(cp.theta);
    theta_dot.push_back(cp.theta_dot);
    ++data_length;
}


/* @brief: clear elements */
void CartPoleLogger::clear() {
    x.clear();
    x_dot.clear();
    theta.clear();
    theta_dot.clear();
    data_length = 0;
}


/* @brief: save system states to file */
bool CartPoleLogger::archive(const std::string &ofile) {
    std::ofstream ofs(ofile);
    if (!ofs) {
        std::cerr << BnR_ERROR << "can't open file " << ofile << std::endl;
        return false;
    }
    for (eSpinn_size i = 0; i < data_length; ++i) {
        ofs << x[i] << ' ' << x_dot[i] << ' '
            << theta[i] << ' ' << theta_dot[i] << '\n';
    }
    ofs.close();
    return true;
}
