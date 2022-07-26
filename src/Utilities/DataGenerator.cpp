/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#include "DataGenerator.h"
using namespace eSpinn;


/* @brief: allocate memory */
void DataGenerator::init() {
    for (auto w = 0; w < iwidth; ++w) {
        auto d = new std::vector<double>(timesteps, 0);
        inps.push_back(d);
    }
    for (auto w = 0; w < owidth; ++w) {
        auto d = new std::vector<double>(timesteps, 0);
        outps.push_back(d);
    }
}


/* @brief: system dynamic
 * a(t) = f(t)/m + k * v(t)
 * v(t+1) = v(t) + a(t) * dt
 */
void DataGenerator::system_dynamic() {
    const double m_rev = 0.5; // mass = 2
    const double k = -0.2; 
    double v = .0; // initial velocity
    for (auto i = 0; i < timesteps; ++i) {
        (*inps[1])[i] = v;
        (*outps[0])[i] = (*inps[0])[i] * m_rev + k * v;
        v += (*outps[0])[i] * dt;
    }
}


/* @brief: generate training data
 * based on system dynamic
 */
void DataGenerator::generate() {
    // assign force
    for (int i = 0; i < timesteps; ++i) {
        // t = dt*i
        (*inps[0])[i] = 1 - 0.2 * (dt*i); // 1 sin(dt*i)
    }
    // assign vt & acc
    system_dynamic();
}


/* @brief: save data to file */
bool DataGenerator::save() {
    std::string ofile;
    std::ofstream ofs;
    for (int w = 0; w < iwidth; ++w) {
        ofile = FILE_IN + std::to_string(w);
        ofs.open(ofile);
        if (!ofs) {
            std::cerr << BnR_ERROR << "can't open file " << ofile << std::endl;
            return false;
        }
        ofs << *inps[w];
        ofs.close();
    }
    for (int w = 0; w < owidth; ++w) {
        ofile = FILE_OUT + std::to_string(w);
        ofs.open(ofile);
        if (!ofs) {
            std::cerr << BnR_ERROR << "can't open file " << ofile << std::endl;
            return false;
        }
        ofs << *outps[w];
        ofs.close();
    }
    return true;
}
