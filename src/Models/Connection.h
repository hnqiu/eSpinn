/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#pragma once


#include "eSpinn_def.h"
#include "HebbPlasticity.h"
#include "Utilities/Utilities.h"
#include <iostream>
#include <deque>
#include <cassert>
#include <boost/serialization/access.hpp>

/* @brief: Connection default
 * initialization list: id, (in_node, out_node), (weight, enable, conn_type)
 */
namespace eSpinn {
    class Neuron;
    class Connection {
        /* @brief: declare serialization library as friend
         * used to grant to the serialization library access to class members
         */
        friend class boost::serialization::access;

	    /* @brief: template member serialize function
         * called when using i/oarchive to load/write class members via serialization
         * make serialize() private to avoid being called directly 
         * ? when called, an instance of this class will first be constructed using default constructor
         */
        template<typename Archive>
        void serialize(Archive &ar, const unsigned int version) {
            // virtual_serialize(ar, version);
            #ifdef ESPINN_SERIALIZE_VERBOSE
            std::cout << "Serializing Connection ";
            #endif
            ar & c_id;
            ar & in_node & out_node;
            ar & weight & synapse_delay;
            ar & enable & c_type;
            ar & hebb;
            ar & plastic_module;
            #ifdef ESPINN_SERIALIZE_VERBOSE
            std::cout << "#" << c_id << "..." << std::endl;
            #endif
        }

        /* @brief: overloaded << 
         * deligate printing to member function print()
         */
        friend std::ostream& operator<<(std::ostream &os, const Connection &c) {
            return c.print(os);
        }
        template <typename Ti, typename Th, typename To> friend class Network;
        friend class Neuron;
    protected:
        /* data */
        connID c_id;
        Neuron *in_node, *out_node;
        double weight, weight_pre;
        synDel synapse_delay;
        bool enable;
        connType c_type;
        HebbianType hebb;
        std::deque<double> receptor; // archive outputs from in_node

        /* @brief: print class info 
         * do the major printing here
         */
        std::ostream& _print(std::ostream &os) const;

        /* @brief: print class info 
         * print different header if the class is inherited
         */
        virtual std::ostream& print(std::ostream &os) const;
    public:
        /* data */
        HebbPlasticity plastic_module;

        /* @brief: constructors & destructor */
        Connection(const connID &cid, Neuron *const inn, Neuron *const outn, 
        const double &w = .0, const synDel &d = params::NO_DELAY,
        const bool &en = true, const connType &ct = DEFAULTCONN,
        const HebbianType &h = NoHebbian
        ) :
        c_id(cid), in_node(inn), out_node(outn), 
        weight(w), weight_pre(w), synapse_delay(d), enable(en), c_type(ct),
        hebb(h),
        // create a new empty std::deque<double> and assign to pointer receptor
        receptor(std::deque<double>()) 
        , plastic_module()
        {
            #ifdef ESPINN_MAX_WEIGHT
            weight = params::MAX_WEIGHT;
            #endif

            #ifdef ESPINN_VERBOSE
            std::cout << "Constructing Connection #" << c_id;
            // careful because these pointers can be null
            if (in_node != nullptr && out_node != nullptr) {
                std::cout << " from node #" << getInodeID() << " to node #" << getOnodeID();
            }
                std::cout << ", weight = " << weight << ", syn_delay = " 
                    << synapse_delay << std::endl;
            #endif
        }
        /* @brief: construct a connection without input/output neurons */
        Connection(const connID &cid): Connection(cid, nullptr, nullptr) { }
        /* @brief: default constructor for using boost serialization */
        Connection(): Connection(0) { }

        /* @brief: copy constructor
         * copy the Neuron pointer itself, not objects the pointers point toward
         * initialize a new empty receptor, not use the original
         */
        Connection(const Connection &conn) : 
        c_id(conn.c_id), in_node(conn.in_node), out_node(conn.out_node), 
        weight(conn.weight), weight_pre(conn.weight),
        synapse_delay(conn.synapse_delay),
        enable(conn.enable), c_type(conn.c_type), hebb(conn.hebb),
        receptor(std::deque<double>()) 
        , plastic_module(conn.plastic_module)
        {
            #ifdef ESPINN_VERBOSE
            std::cout << "Copy constructing Connection #" << c_id << std::endl;
            #endif
        }

        /* @brief: destructor 
         * do not use Connection to free Neurons 
         */
        virtual ~Connection() {
            #ifdef ESPINN_VERBOSE
            std::cout << "Destructing Connection #" << c_id << std::endl;
            #endif
        }

        /* @brief: get connection type */
        const connType getType() const { return c_type; }

        /* @brief: get connecion id */
        const connID getID() const { return c_id; }

        /* @brief: set connection id */
        void setID(const connID &cid) { c_id = cid; }

        /* @brief: duplicate this connection */
        virtual Connection* duplicate();

        /* @brief: get input neuron id */
        const neuronID getInodeID() const;

        /* @brief: get output neuron id */
        const neuronID getOnodeID() const;

        /* @brief: set input neuron */
        void setInode(Neuron *const n);

        /* @brief: set output neuron */
        void setOnode(Neuron *const n);

        /* @brief: get input neuron */
        Neuron *const getInode() const;

        /* @brief: get output neuron */
        Neuron *const getOnode() const;

        /* @brief: get input neuron seq */
        const eSpinn_size getInodeSeq() const;

        /* @brief: get output neuron seq */
        const eSpinn_size getOnodeSeq() const;

        /* @brief: get connection weight */
        const double getWeight() const;

        /* @brief: set connection weight */
        void setWeight(const double &val);

        /* @brief: increase connection weight */
        void increaseWeight(const double &val);

        /* @brief: weight change for plastic connections
         * update connection weight via Hebbian learning 
         */
        void updateWeight();

        /* @brief: back up weight during Hebbian rule development */
        void backupWeight();

        /* @brief: restore weight after Hebbian rule development */
        void restoreWeight();

        /* @brief: cap weight between [-MAX_WEIGHT, MAX_WEIGHT] */
        void capWeight();

        /* @brief: get Hebbian type */
        HebbianType get_hebb_type();

        /* @brief: set Hebbian type */
        void set_hebb_type(const HebbianType &h);

        /* @brief: cap plastic terms */
        void cap_plastic_terms();

        /* @brief: get plastic term */
        const double get_plastic_term(const eSpinn_size &which) const;

        /* @brief: set plastic term */
        void set_plastic_term(const double &val, const eSpinn_size &which);

        /* @brief: increase plastic term */
        void increase_plastic_term(const double &val, const eSpinn_size &which);

        /* @brief: copy plasticity rule */
        void copy_plasticity_module(const Connection *c);

        /* @brief: get connection synaptic delay */
        const synDel getDelay() const;

        /* @brief: set connection synaptic delay */
        void setDelay(const double &val);

        /* @brief: get connection enable status */
        const bool isEnable() const;

        /* @brief: set connection enable status */
        void setEnable(const bool &status);

        /* @brief: push output from in_node */
        void pushReceptor(const double &r);

        /* @brief: get the delayed recent receptor */
        const double getRecentReceptor() const;
    };
    
}