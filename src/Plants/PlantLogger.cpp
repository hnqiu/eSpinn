/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#include "PlantLogger.h"
using namespace eSpinn;


/* @brief: constructor - allocate memory */
PlantLogger::PlantLogger(const std::size_t &capacity)
    : data_length(capacity), val_ref(), val_act(), err()
{
    val_ref.assign(capacity, .0);
    val_act.assign(capacity, .0);
    err.assign(capacity, .0);
    #ifndef NDEBUG
    std::cout << "Allocate memory for " << data_length << " data" << std::endl;
    #endif
}


/* @brief: load reference signal from file
 * and assign to val_ref
 * use this method when memory has been allocated
 */
void PlantLogger::assign_ref_signal(const std::string &inp_file) {
    std::ifstream ifs(inp_file);
    if (!ifs) {
        std::cerr << BnR_ERROR << "can't open file " << inp_file << std::endl;
        return;
    }
    double s;
    std::size_t i = 0;
    while (ifs >> s) {
        val_ref[i++] = s;
    }
    ifs.close();
}


/* @brief: load reference signal from file
 * and allocate memory
 */
void PlantLogger::load_ref_signal(const std::string &inp_file) {
    std::ifstream ifs(inp_file);
    if (!ifs) {
        std::cerr << BnR_ERROR << "can't open file " << inp_file << std::endl;
        return;
    }
    double s;
    while (ifs >> s) {
        val_ref.push_back(s);
    }
    ifs.close();

    data_length = val_ref.size();
    val_act.assign(data_length, .0);
    err.assign(data_length, .0);

    #ifndef NDEBUG
    std::cout << "Loading data from " << inp_file
        << ". Data length is " << data_length << std::endl;
    #endif
}


/* @brief: return size of signal */
std::vector<double>::size_type PlantLogger::length() const {
    return data_length;
}


/* @brief: log system's actual output */
void PlantLogger::log_act(const eSpinn_size &ts, const double &o) {
    val_act[ts] = o;
}


/* @brief: get actual output at the timestep */
const double PlantLogger::act_at(const eSpinn_size &ts) {
    return val_act[ts];
}


/* @brief: get error at the timestep */
const double PlantLogger::err_at(const eSpinn_size &ts) {
    return err[ts];
}


/* @brief: log error at the timestep */
void PlantLogger::log_err(const eSpinn_size &ts, const double &e) {
    err[ts] = e;
}


/* @brief: calculate error at the timestep */
const double PlantLogger::cal_err(const eSpinn_size &ts) {
    err[ts] = val_act[ts] - val_ref[ts];
    return err[ts];
}


/* @brief: calculate mean standard error */
const double PlantLogger::cal_stde() {
    double std_err = .0;
    for (const auto &e : err) {
        std_err += std::abs(e);
    }
    return std_err/length();
}


/* @brief: calculate mean standard error of the first N elements */
const double PlantLogger::cal_stde(const eSpinn_size &num) {
    double std_err = .0;
    for (auto i = 0; i < num; ++i) {
        std_err += std::abs(err[i]);
    }
    return std_err/num;
}


/* @brief: calculate mean square error */
const double PlantLogger::cal_mse() {
    double sq_err = .0;
    for (const auto &e : err) {
        sq_err += e * e;
    }
    return sq_err/length();
}


/* @brief: calculate mean square error of the first N elements */
const double PlantLogger::cal_mse(const eSpinn_size &num) {
    double sq_err = .0;
    for (auto i = 0; i < num; ++i) {
        sq_err += err[i] * err[i];
    }
    return sq_err/num;
}


/* @brief: calculate mean cubic error */
const double PlantLogger::cal_e3() {
    double cb_err = .0;
    for (const auto &e : err) {
        cb_err += e * e * std::abs(e);
    }
    return cb_err/length();
}


/* @brief: calculate std error
 * call either of the above 3 methods to get std err
 */
const double PlantLogger::cal_std_err() {
    return cal_stde();
    // auto err = cal_mse();
    // return fast_sqrt(err);
    // auto err = cal_e3();
    // return std::cbrt(err);
}


/* @brief: save actual output to file */
bool PlantLogger::save_act(const std::string &ofile) {
    std::ofstream ofs(ofile);
    if (!ofs) {
        std::cerr << BnR_ERROR << "can't open file " << ofile << std::endl;
        return false;
    }
    ofs << val_act;
    ofs.close();
    return true;
}


/* @brief: save error to file */
bool PlantLogger::save_err(const std::string &ofile) {
    std::ofstream ofs(ofile);
    if (!ofs) {
        std::cerr << BnR_ERROR << "can't open file " << ofile << std::endl;
        return false;
    }
    ofs << err;
    ofs.close();
    return true;
}
