/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#include "Injector.h"
using namespace eSpinn;


/* @brief: print class info 
 * do the actual printing here
 */
std::ostream& Injector::print(std::ostream &os) const {
    os << "Injector (width " << width() << ", length " << length() << ")";
    for (eSpinn_size i = 0; i < width(); ++i) {
        os << " " << norm_scaler[i] << " " << shift[i];
    }
    return os;
}


/* @brief: set normalization factors for all data type */
void Injector::setNormFactors(const std::vector<double> &min, const std::vector<double> &max) {
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
        std::cout << "set injector factors #" << i
            << ", normalizing scaler = " << norm_scaler[i] 
            << ", shift = " << shift[i] << std::endl;
        #endif
    }
}


/* @brief: set normalization factors for all data type */
void Injector::setNormFactors(const double *min, const double *max, const eSpinn_size &n) {
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
        std::cout << "set injector factors #" << i
            << ", normalizing scaler = " << norm_scaler[i] 
            << ", shift = " << shift[i] << std::endl;
        #endif
    }
}


/* @brief: set normalization factors for ONE data type */
void Injector::setNormFactors(const double &min, const double &max, const eSpinn_size &which) {
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
    std::cout << "set injector factors #" << which
        << ", normalizing scaler = " << norm_scaler[which] 
        << ", shift = " << shift[which] << std::endl;
    #endif
}


/* @brief: set normalization factors from file */
void Injector::setNormFactors(const std::string &filename) {
    #ifndef NDEBUG
    std::cout << "Loading injecting data range..." << std::endl;
    #endif
    std::ifstream ifs(filename);
    if (!ifs) {
        std::cerr << BnR_ERROR << "can't open file " << filename << std::endl;
        return ;
    }
    double min = 0, max = 0;
    for (auto i = 0; i < width(); ++i) {
        if (ifs >> min >> max) {
            setNormFactors(min, max, i);
        }
        else {
            // if MINs/MAXs size is too short, the rest are set to 0/1
            setNormFactors(0, 1, i);
        }
    }

    ifs.close();
}


/* @brief: normalize training data */
void Injector::normalize() {
    eSpinn_size n = 0;
    for (auto &d : data_norm) {
        for (auto l = 0; l < length(); ++l) {
            (*d)[l] = norm_scaler[n] * ((*d)[l] + shift[n]);
        }
        ++n;
    }
}


/* @brief: normalize value */
const double Injector::normalize(const eSpinn_size &which, const double &v) {
    return norm_scaler[which] * (v+shift[which]);
}


/* @brief: load normalized value to data_set
 * normalized data range between [0, 1]
 */
void Injector::load_data(const eSpinn_size &which, const double &val) {
    data_set[which] = norm_scaler[which] * (val+shift[which]);
}


/* @brief: load and normalize training data at this timestep
 * normalized data range between [0, 1]
 */
void Injector::load_data(const double *p, const eSpinn_size &n) {
    if (n != width()) {
        std::cerr << BnR_ERROR << "data size not met!" << std::endl;
        return;
    }
    for (auto i = 0; i < n; ++i) {
        data_set[i] = norm_scaler[i] * (*(p++)+shift[i]);
        // data_set[i] = normalize(i, *(p++));
    }
}


/* @brief: get data_set at this timestep */
const double *const Injector::get_data_set() const {
    return data_set;
}


/* @brief: get normalized data that will be injected at ts timestep */
const double *const Injector::get_data_set(const eSpinn_size &ts) const {
    // if (ts >= length()) {
    //     std::cerr << BnR_ERROR << "out of range" << std::endl;
    //     return; // TODO
    // }

    for (auto i = 0; i < width(); ++i) {
        data_set[i] = (*data_norm[i])[ts];
    }
    return data_set;
}


/* @brief: save injector to file */
void Injector::archive(const std::string &ofile) {
    #ifndef NDEBUG
    std::cout << "Archiving injector to file " << ofile << std::endl;
    #endif
    std::ofstream ofs(ofile);
    if (!ofs) {
        std::cerr << BnR_ERROR << "Can't open file " << ofile << std::endl;
        return;
    }
    ofs << data_width << " ";
    boost::archive::text_oarchive oa(ofs);
    oa & *this;
    ofs.close();
}


/* @brief: construct injector from file */
Injector eSpinn::createInjector(const std::string &ifile) {
    #ifndef NDEBUG
    std::cout << "Creating injector from file " << ifile << std::endl;
    #endif
    std::ifstream ifs(ifile);
    if (!ifs) {
        std::cerr << BnR_ERROR << "Can't open file " << ifile << std::endl;
        return Injector(0);
    }
    eSpinn_size width;
    ifs >> width;
    Injector inj(width);
    boost::archive::text_iarchive ia(ifs);
    ia & inj;
    ifs.close();

    return inj;
}
