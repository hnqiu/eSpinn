/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#pragma once


#include "eSpinn_def.h"
#include <iostream>
#include <vector>

#include <boost/serialization/access.hpp>
#include <boost/serialization/split_member.hpp>

/* @brief: NetworkBase is an abstract class
 * use pointer of base network to access Network
 */
namespace eSpinn {
    class Neuron;
    class Connection;
    class NetworkBase {
        /* @brief: declare serialization library as friend
         * used to grant to the serialization library access to class members
         */
        friend class boost::serialization::access;

        template<class Archive>
        void save(Archive & ar, const unsigned int version) const
        {
            ar << net_id;
            #ifdef ESPINN_SERIALIZE_VERBOSE
            std::cout << "Serializing Network #" << net_id << "..." << std::endl;
            #endif
        }

        template<class Archive>
        void load(Archive & ar, const unsigned int version)
        {
            ar >> net_id;
            #ifdef ESPINN_SERIALIZE_VERBOSE
            std::cout << "Loading Network #" << net_id << "..." << std::endl;
            #endif
        }

	    /* @brief: template member serialize function
         * called when using i/oarchive to load/write class members via serialization
         * make serialize() private to avoid being called directly 
         * when called, an instance of this class will first be constructed using default constructor
         */
        template<class Archive>
        void serialize(Archive &ar, const unsigned int version) {
            boost::serialization::split_member(ar, *this, version);
        }

        /* @brief: overloaded << 
         * deligate printing to member function print()
         */
        friend std::ostream& operator<<(std::ostream &os, const NetworkBase &n) {
            return n.print(os);
        }
    protected:
        /* data */
        netID net_id;
        
        /* @brief: print class info 
         * do the actual printing here
         */
        virtual std::ostream& print(std::ostream &os) const;
    public:
        NetworkBase(const netID &nid) : net_id(nid) { }
        NetworkBase(const NetworkBase &net) : net_id(net.net_id) { }
        virtual ~NetworkBase() = default;

        /* @brief: get network id */
        const netID getID() const;

        /* @brief: set network id */
        void setID(const netID &i);

        /* @brief: get neuron size */
        virtual std::vector<Neuron*>::size_type get_neuron_size() const = 0;

        /* @brief: get connection size */
        virtual std::vector<Connection*>::size_type get_connection_size() const = 0;

        /* @brief: get connection weights */
        virtual const std::vector<double> get_connection_weights() const = 0;

        /* @brief: load network inputs */
        virtual void load_inputs(const double *p, const eSpinn_size n) = 0;

        /* @brief: load network inputs */
        virtual void load_inputs(const std::vector<double> &p) = 0;

        /* @brief: run network for ONE time slot */
        virtual const std::vector<double>&  run() = 0;
    };
}
