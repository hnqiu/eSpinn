/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#include "Logger.h"
using namespace eSpinn;


/* @brief: return size of vector */
std::vector<double>::size_type Logger::size() const {
    return val.size();
}


/* @brief: push back element */
void Logger::push_back(const double &v) {
    val.push_back(v);
}


/* @brief: get data at the timestep */
const double Logger::at(const std::size_t &ts) {
    return val[ts];
}


/* @brief: clear elements */
void Logger::clear() {
    val.clear();
}


/* @brief: save data to file */
bool Logger::save(const std::string &ofile) const {
    std::ofstream ofs(ofile);
    if (!ofs) {
        std::cerr << BnR_ERROR << "can't open file " << ofile << std::endl;
        return false;
    }
    ofs << val;
    ofs.close();
    return true;
}


/* @brief: append value to file */
bool Logger::append_to_file(const double &val, const std::string &ofile) const {
    std::ofstream ofs(ofile, std::ofstream::app);
    if (!ofs) {
        std::cerr << BnR_ERROR << "can't open file " << ofile << std::endl;
        return false;
    }
    ofs << val << std::endl;
    ofs.close();
    return true;
}


/* @brief: append an empty line to file */
bool Logger::append_newline_to_file(const std::string &ofile) const {
    std::ofstream ofs(ofile, std::ofstream::app);
    if (!ofs) {
        std::cerr << BnR_ERROR << "can't open file " << ofile << std::endl;
        return false;
    }
    ofs << std::endl;
    ofs.close();
    return true;
}
