/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#include "Gate.h"
using namespace eSpinn;


/* @brief: load training data from files
 * return a pointer to Gate
 */
Gate* eSpinn::load_data_from_files(
    const std::initializer_list<std::string> &inp_files, 
    const std::initializer_list<std::string> &outp_files)
{
    #ifndef NDEBUG
    std::cout << "Loading data from " << inp_files.size() << " input data and " 
        << outp_files.size() << " output data files..." << std::endl;
    #endif
    auto gate = new Gate(inp_files.size(), outp_files.size());
    for (auto &f : inp_files) {
        std::ifstream ifs(f);
        if (!ifs) {
            // if failed, return a default gate
            std::cerr << BnR_ERROR << "can't open file " << f << std::endl;
            delete gate;
            return new Gate;
        }
        // load raw data and set data length
        // if length not the same, return a default gate
        if (!gate->load_time_serialized_data(ifs, INPUTDATA)) {
            ifs.close();
            delete gate;
            return new Gate;
        }
        ifs.close();
    }

    for (auto &f : outp_files) {
        std::ifstream ifs(f);
        if (!ifs) {
            // if failed, return a default gate
            std::cerr << BnR_ERROR << "can't open file " << f << std::endl;
            delete gate;
            return new Gate;
        }
        // load raw data and set data length
        // if length not the same, return a default gate
        if (!gate->load_time_serialized_data(ifs, OUTPUTDATA)) {
            ifs.close();
            delete gate;
            return new Gate;
        }
        ifs.close();
    }
    #ifndef NDEBUG
    std::cout << "Data length is " << gate->getLength() << std::endl;
    #endif
    return gate;
}


/* @brief: set channel width */
void Gate::setWidth(const eSpinn_size &Iwidth, const eSpinn_size &Owidth) {
    inject_channel_width = Iwidth;
    output_channel_width = Owidth;
}


/* @brief: get input channel width */
const eSpinn_size Gate::getIwidth() const {
    return inject_channel_width;
}

/* @brief: get output channel width */
const eSpinn_size Gate::getOwidth() const {
    return output_channel_width;
}


/* @brief: get data length */
const eSpinn_size Gate::getLength() const {
    return data_length;
}


/* @brief: load raw data and set data length
 * if raw data from different files do not have the same size
 * print error and return
 */
bool Gate::load_time_serialized_data(std::ifstream &ifs, const dataLabel &label) {
    // load data from file
    double d;
    auto raw_data = new std::vector<double>;
    while (ifs >> d) {
        raw_data->push_back(d);
    }

    // check if data size met
    if (inputs_raw.empty()) {
        data_length = raw_data->size();
    }
    else {
        if (data_length != raw_data->size()) {
            std::cerr << BnR_ERROR << "raw data size not met" << std::endl;
            delete raw_data; // don't forget to delete memory
            return false;
        }
    }

    // load data to Gate
    switch (label)
    {
        case INPUTDATA:
        default:
            inputs_raw.push_back(std::move(raw_data));
            break;
    
        case OUTPUTDATA:
            outputs_raw.push_back(std::move(raw_data));
            break;
    }

    return true;
}


/* @brief: set injecting and ejecting normalizing factors
 * the file should have content like:
   0.0 1.0
   0.0 1.0
   ---
   0.0 1.0
   
   // ignore from this line
 * end of file
 * values before "---" are injecting data range
 * after are ejecting data range
 * the former one at each line is the minimum
 * the latter is the maximum
 */
void Gate::set_normalizing_factors(const std::string &filename) {
    std::ifstream ifs(filename);
    if (!ifs) {
        std::cerr << BnR_ERROR << "can't open file " << filename << std::endl;
        return ;
    }
    std::string s;
    double min, max;
    std::vector<double> inj_mins, inj_maxs;
    std::vector<double> ej_mins, ej_maxs;
    bool isInject = true;

    #ifndef NDEBUG
    std::cout << "Content in file " << filename << " is " << std::endl;
    #endif
    while (!ifs.eof()) {
        std::getline(ifs, s);
        #ifndef NDEBUG
        std::cout << s << std::endl;
        #endif
        if (s == "---") {
            isInject = false;
            continue;
        }
        else if (s == "")
            break;
        else {
            std::stringstream iss(s);
            iss >> min >> max;
            if (isInject) {
                inj_mins.push_back(min);
                inj_maxs.push_back(max);
            }
            else {
                ej_mins.push_back(min);
                ej_maxs.push_back(max);
            }
        }
    }
    ifs.close();
    injector->setNormFactors(inj_mins, inj_maxs);
    ejector->setNormFactors(ej_mins, ej_maxs);
}


/* @brief: initialize injector & ejector
 * normalize and archive training data
 * allocate actual data memory
 */
void Gate::init() {
    #ifdef ESPINN_VERBOSE
    std::cout << "Initializing ejector..." << std::endl;
    #endif
    // set data length
    injector->setLength(getLength());
    ejector->setLength(getLength());

    // normalize training data and archive to injector
    for (auto &d : inputs_raw) {
        auto inps_norm = new std::vector<double>(*d);
        injector->data_norm.push_back(inps_norm);
    }
    injector->normalize();

    // normalize training data and archive to ejector
    for (auto &d : outputs_raw) {
        auto outps_norm = new std::vector<double>(*d);
        ejector->data_norm.push_back(outps_norm);
    }
    ejector->normalize();

    // allocate memory for actual data 
    for (auto i = 0; i < getOwidth(); ++i) {
        auto outps_act = new std::vector<double>(getLength(), 0);
        ejector->net_outp.push_back(outps_act);
    }

    for (auto i = 0; i < getOwidth(); ++i) {
        auto outps_act = new std::vector<double>(getLength(), 0);
        ejector->data_act.push_back(outps_act);
    }
}


/* @brief: get normalized injected data at ts timestep */
const double *const Gate::get_injector_data_set(const eSpinn_size &ts) const {
    return injector->get_data_set(ts);
}


/* @brief: get normalized ejected data at ts timestep */
const double *const Gate::get_ejector_data_set(const eSpinn_size &ts) const {
    return ejector->get_data_set(ts);
}


/* @brief: archive network output data to ejector */
void Gate::eject_net_outp(const std::vector<double>& outp, const eSpinn_size &ts) {
    ejector->eject_net_outp(outp, ts);
}


/* @brief: calculate ejector mean square errors */
const double Gate::cal_mse() const {
    return ejector->cal_mse();
}


/* @brief: denormalize ejector net_outp data */
void Gate::denormalize_outp() {
    ejector->denormalize_net_outp();
}


/* @brief: save ejector output data_act to file
 * ONE data channel at each column
 * data are saved in time-serial order
 */
void Gate::archive_act_output(const std::string &filename) {
    ejector->archive_act_output(filename);
}
