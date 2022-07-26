/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#pragma once


#include "eSpinn_def.h"
#include "NetworkBase.h"
#include "Utilities/Utilities.h"

#include <vector>
#include <string>
#include <fstream>


/* @brief: Connection weight watcher
 * initialization list: which net, (which episode)
 */
namespace eSpinn {
    class WeightWatcher {
    private:
        /* data */
        eSpinn_size width, length, episode;
        NetworkBase* net;
        std::vector<std::vector<double> *> val;
    public:
        /* @brief: constructor */
        WeightWatcher(NetworkBase *const n, const eSpinn_size &epi = 0,
            const std::size_t &capacity = 8);

        /* @brief: deleted copy constructor */
        WeightWatcher(const WeightWatcher &) = delete;

        /* @brief: destructor */
        ~WeightWatcher();

        /* @brief: return size of connections */
        const eSpinn_size conn_size() const;

        /* @brief: clear elements */
        void clear();

        /* @brief: log connection weights */
        void log_weights();

        /* @brief: save weight changes to file */
        bool save(const std::string &filename);
    };
}
