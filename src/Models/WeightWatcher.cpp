/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#include "WeightWatcher.h"
using namespace eSpinn;



/* @brief: constructor */
WeightWatcher::WeightWatcher(NetworkBase *const n, const eSpinn_size &epi,
                             const std::size_t &capacity)
    : width(n->get_connection_size()), length(0), episode(epi)
    , net(n) {
    for (auto i = 0; i < width; ++i) {
        auto v = new std::vector<double>();
        v->reserve(capacity);
        val.push_back(v);
    }
}


/* @brief: destructor */
WeightWatcher::~WeightWatcher() {
    for (auto &v : val) {
        delete v;
    }
}


/* @brief: return size of connections */
const eSpinn_size WeightWatcher::conn_size() const {
    return width;
}


/* @brief: clear elements */
void WeightWatcher::clear() {
    for (auto &v : val)
        v->clear();
    length = 0;
}


/* @brief: log connection weights */
void WeightWatcher::log_weights() {
    auto cur_weights = net->get_connection_weights();
    for (eSpinn_size i = 0; i < width; ++i) {
        val[i]->push_back(cur_weights[i]);
    }
    ++length;
}


/* @brief: save weight changes to file */
bool WeightWatcher::save(const std::string &filename) {
    // std::string filename = file_prefix + "cw_e" + std::to_string(episode)
    //     + "_net" + std::to_string(net->getID());
    std::ofstream ofs(filename);
    if (!ofs) {
        std::cerr << BnR_ERROR << "can't open file " << filename << std::endl;
        return false;
    }
    ofs << width << std::endl;
    for (eSpinn_size l = 0; l < length; ++l) {
        for (eSpinn_size w = 0; w < width; ++w) {
            ofs << (*val[w])[l] << " ";
        }
        ofs << std::endl;
    }
    ofs.close();

    ++episode;
    clear();
    return true;
}
