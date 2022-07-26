/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#pragma once


#include "eSpinn_def.h"
#include "Connection.h"
#include <iostream>
#include <boost/serialization/base_object.hpp>

/* @brief: SpikeConnection - connection to spike neurons
 * initialization list: id, in_node, out_node, (weight, enable, conn_type)
 */
namespace eSpinn {
    class SpikeConnection : public Connection
    {
        /* @brief: declare serialization library as friend
         * used to grant to the serialization library access to class members
         */
        friend class boost::serialization::access;

	    /* @brief: template member serialize function
         * called when using i/oarchive to load/write class members via serialization
         * make serialize() private to avoid being called directly 
         * when called, an instance of this class will first be constructed using default constructor
         */
        template<typename Archive>
        void serialize(Archive &ar, const unsigned int version) {
            ar & boost::serialization::base_object<Connection>(*this);
        }

    private:
        /* data */
        static constexpr double spikeFactor = params::currentFactor;

        /* @brief: print class info 
         */
        std::ostream& print(std::ostream &os) const override;
    public:
        SpikeConnection(const connID &cid, Neuron *const inn, Neuron *const outn, 
            const double &w = .0, const synDel &d = params::NO_DELAY,
            const bool &en = true, const connType &ct = SPIKECONN,
            const HebbianType &h = NoHebbian
        ) :
            Connection(cid, inn, outn, w, d, en, ct, h)
        {
            #ifdef ESPINN_VERBOSE
            std::cout << "spikeFactor = " << spikeFactor << std::endl;
            #endif
        }
        /* @brief: construct a connection without input/output neurons */
        SpikeConnection(const connID &cid) : SpikeConnection(cid, nullptr, nullptr) { }
        /* @brief: default constructor for using boost serialization */
        SpikeConnection() : SpikeConnection(0) { }

        /* @brief: copy constructor */
        SpikeConnection(const SpikeConnection &conn) : 
            Connection(conn) { }

        /* @brief: destructor */
        ~SpikeConnection() = default;

        static const double getSpikeFactor() { return spikeFactor; }

        /* @brief: duplicate this connection */
        SpikeConnection* duplicate() override;
    };
    
}

