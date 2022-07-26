/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#pragma once


#include "eSpinn_def.h"
#include "Connection.h"
#include "SpikeConnection.h"

#include <iostream>
#include <vector>

#include <boost/serialization/vector.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/split_member.hpp>

/* @brief: Neuron: base neuron class
 */
namespace eSpinn {
    class Neuron {
        /* @brief: declare serialization library as friend
         * used to grant to the serialization library access to class members
         */
        friend class boost::serialization::access;

        template<class Archive>
        void save(Archive & ar, const unsigned int version) const
        {
            ar << n_id << n_seq << n_layer << n_type;
            // need not archive connections
            // ar & in_conn & out_conn;
            #ifdef ESPINN_SERIALIZE_VERBOSE
            std::cout << "#" << n_id << "..." << std::endl;
            #endif
        }

        template<class Archive>
        void load(Archive & ar, const unsigned int version)
        {
            ar >> n_id >> n_seq >> n_layer >> n_type;
            // connections will be added later in Network::after_load()
            #ifdef ESPINN_SERIALIZE_VERBOSE
            std::cout << "#" << n_id << "..." << std::endl;
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
        friend std::ostream& operator<<(std::ostream &os, const Neuron &n) {
            return n.print(os);
        }
        template<typename Ti, typename Th, typename To> friend class Network;
    protected:
        /* data */
        static constexpr neuronType c_type = UNDEFINED;
        neuronID n_id;
        eSpinn_size n_seq; // sequence to activate 
        neuronLayer n_layer;
        neuronType n_type;
        std::vector<Connection*> in_conn, out_conn;

        /* @brief: print class info 
         * do the actual printing here
         */
        virtual std::ostream& print(std::ostream &os) const;
    public:
        /* @brief: constructor */
        Neuron(const neuronID &nid, const neuronLayer &nl, const neuronType &nt = UNDEFINED) : 
            n_id(nid), n_seq(nid), n_layer(nl), n_type(nt) { }

        /* @brief: default constructor */
        Neuron() : Neuron(0, L_INPUT) { }

        /* @brief: copy constructors
         * note it is not copying connections
         */
        Neuron(const Neuron &node): 
            n_id(node.n_id), n_seq(node.n_seq), 
            n_layer(node.n_layer), n_type(node.n_type) { }

        virtual ~Neuron() = default;

        /* @brief: get neuron type */
        static const neuronType getClassType() {
            return c_type;
        }

        /* @brief: get neuron id */
        const neuronID getID() const {
            return n_id;
        }

        /* @brief: set neuron sequence */
        void setID(const neuronID &nid) {
            n_id = nid;
        }

        /* @brief: get neuron type */
        const neuronType getType() const {
            return n_type;
        }

        /* @brief: set neuron type */
        void setType(const neuronType &nt) {
            n_type = nt;
        }

        /* @brief: check if neuron is spiking neuron */
        bool is_spike_neuron() const;

        /* @brief: get neuron sequence */
        const eSpinn_size getSeq() const {
            return n_seq;
        }

        /* @brief: set neuron sequence */
        void setSeq(const eSpinn_size &seq) {
            n_seq = seq;
        }

        /* @brief: get neuron layer */
        const neuronLayer getLayer() const {
            return n_layer;
        }

        /* @brief: set neuron layer */
        void setLayer(const neuronLayer &l) {
            n_layer = l;
        }

        /* @brief: duplicate this neuron */
        virtual Neuron* duplicate();

        /* @brief: add incoming connection */
        void add_inConn(Connection *conn);

        /* @brief: add outgoing connection */
        void add_outConn(Connection *conn);

        /* @brief: remove incoming connection */
        void remove_inConn(Connection *const conn);

        /* @brief: remove outgoing connection */
        void remove_outConn(Connection *const conn);

        /* @brief: load input data
         * use this method for input-layer neurons
         */
        virtual void load_input(const double *val) { }

        /* @brief: load input data
         * accumulate synaptic inputs
         * use this method for non-input-layer neurons
         */
        virtual void load_input() { }

        /* @brief: get neuron output */
        virtual const double getOut() const { return .0; }

        /* @brief: transmit output to all outgoing connections */
        void transmit(const double &val);

        /* @brief: handle all incoming plastic connections */
        void plasticify_preConn();

        /* @brief: forward neuron 
         * accumulate inputs and transmit output
         * & handle plasticity
         */
        virtual void forward() { }
    };
        
}
