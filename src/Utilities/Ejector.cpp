/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#include "Ejector.h"
using namespace eSpinn;


/* @brief: get output channel width */
const eSpinn_size Ejector::width() const {
    return data_width;
}

/* @brief: get data length */
const eSpinn_size Ejector::length() const {
    return data_length;
}


/* @brief: set data length */
void Ejector::setLength(const eSpinn_size &length) {
    data_length = length;
}


/* @brief: set normalization factors for all data type */
void Ejector::setNormFactors(const std::vector<double> &min, const std::vector<double> &max) {
    if (min.size() != width()) {
        std::cerr << BnR_ERROR << "data size not met!" << std::endl;
        return;
    }
    for (auto i = 0; i < width(); ++i) {
        if (min[i] == max[i]) {
            std::cerr << BnR_ERROR << "MIN == MAX!" << std::endl;
            return;
        }
        shift[i] = -min[i];
        norm_scaler[i] = 1.0/(max[i] - min[i]);
        #ifdef ESPINN_VERBOSE
        std::cout << "set ejector factors #" << i
            << ", normalizing scaler = " << norm_scaler[i] 
            << ", shift = " << shift[i] << std::endl;
        #endif
    }
}


/* @brief: set normalization factors for all data type */
void Ejector::setNormFactors(const double *min, const double *max, const eSpinn_size &n) {
    if (n != width()) {
        std::cerr << BnR_ERROR << "data size not met!" << std::endl;
        return;
    }
    for (auto i = 0; i < n; ++i) {
        if (min[i] == max[i]) {
            std::cerr << BnR_ERROR << "MIN == MAX!" << std::endl;
            return;
        }
        shift[i] = -min[i];
        norm_scaler[i] = 1.0/(max[i] - min[i]);
        #ifdef ESPINN_VERBOSE
        std::cout << "set ejector factors #" << i
            << ", normalizing scaler = " << norm_scaler[i] 
            << ", shift = " << shift[i] << std::endl;
        #endif
    }
}


/* @brief: set normalization factors for ONE data type */
void Ejector::setNormFactors(const double &min, const double &max, const eSpinn_size &which) {
    if (which >= width()) {
        std::cerr << BnR_ERROR << "out of range!" << std::endl;
        return;
    }
    if (min == max) {
        std::cerr << BnR_ERROR << "MIN == MAX!" << std::endl;
        return;
    }

    shift[which] = -min;
    norm_scaler[which] = 1.0/(max - min);
    #ifdef ESPINN_VERBOSE
    std::cout << "set ejector factors #" << which
        << ", normalizing scaler = " << norm_scaler[which] 
        << ", shift = " << shift[which] << std::endl;
    #endif
}


/* @brief: normalize training data */
void Ejector::normalize() {
    eSpinn_size n = 0;
    for (auto &d : data_norm) {
        for (auto l = 0; l < length(); ++l) {
            (*d)[l] = norm_scaler[n] * ((*d)[l] + shift[n]);
        }
        ++n;
    }
}


/* @brief: get norm data_set at the timestep */
const double *const Ejector::get_data_set(const eSpinn_size &ts) const {
    for (auto i = 0; i < width(); ++i) {
        data_set[i] = (*data_norm[i])[ts];
    }
    return data_set;
}


/* @brief: archive network output data to ejector */
void Ejector::eject_net_outp(const std::vector<double>& outp, const eSpinn_size &ts) {
    for (auto i = 0; i < width(); ++i) {
        (*net_outp[i])[ts] = outp[i];
    }
}


/* @brief: calculate normalized mean square errors for each output channel
 * and return the averaged mse
 */
const double Ejector::cal_mse() {
    for (auto chn = 0; chn < width(); ++chn) {
        double sq_err = .0, tmp_err = .0;
        for (auto l = 0; l < length(); ++l) {
            tmp_err = (*net_outp[chn])[l] - (*data_norm[chn])[l];
            sq_err += tmp_err * tmp_err;
        }
        mse[chn] = sq_err/length();
    }
    #ifdef ESPINN_VERBOSE
    std::cout << "Mean square errors are: ";
    for (auto chn = 0; chn < width(); ++chn) {
        std::cout << mse[chn] << " ";
    }
    std::cout << std::endl;
    #endif

    double avg_mse = .0;
    for (double *e = mse; e < mse+width(); ++e) {
        avg_mse += *e;
    }
    avg_mse /= width();
    return avg_mse;
}


/* @brief: denormalize network outputs
 * and archive to data_act
 */
void Ejector::denormalize_net_outp() {
    for (auto w = 0; w < width(); ++w) {
        double norm_scaler_rev = 1.0 / norm_scaler[w];
        for (auto l = 0; l < length(); ++l) {
            (*data_act[w])[l] = (*net_outp[w])[l] * norm_scaler_rev - shift[w];
        }
    }
}


/* @brief: save ejector output data_act to file
 * ONE data channel at each column
 * data are saved in time-serial order
 */
void Ejector::archive_act_output(const std::string &filename) {
    std::ofstream ofs(filename);
    if (!ofs) {
        std::cerr << BnR_ERROR << "can't open file " << filename << std::endl;
        return ;
    }
    for (auto l = 0; l < length(); ++l) {
        for (auto i = 0; i < width(); ++i)
            ofs << data_act[i]->at(l) << " ";
        ofs << std::endl;
    }
    ofs.close();
}
