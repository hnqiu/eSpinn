/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#pragma once


#include "eSpinn_def.h"
#include "Neuron.h"
#include "SigmNeuron.h"
#include "SpikeNeuron.h"
#include "IzhiNeuron.h"
#include "LifNeuron.h"
#include "Sensor.h"
#include "Connection.h"
#include "SpikeConnection.h"
#include "NetworkBase.h"
// #include "Learning/Organism.h"
// use friend-injection flag in cmake to remove this dependency

#include <iostream>
#include <vector>
#include <string>
#include <typeinfo>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/vector.hpp>

/* @brief: Network class
 * with 3 layer neurons
 * initialization list: net id, neuron type, neuron num ... 
 */
namespace eSpinn {
    template <typename Ti, typename Th, typename To>
    class Network : public NetworkBase {
        // just for tests
        friend int build_net();
        friend int copy_net();
        friend int serialize_net();

        /* @brief: declare serialization library as friend
         * used to grant to the serialization library access to class members
         */
        friend class boost::serialization::access;

        template <typename Archive>
        void save(Archive & ar, const unsigned int version) const
        {
            ar << boost::serialization::base_object<NetworkBase>(*this);
            ar << inp_neurons;
            ar << hid_neurons;
            ar << outp_neurons;
            ar << connections;
        }

        template <typename Archive>
        void load(Archive & ar, const unsigned int version)
        {
            ar >> boost::serialization::base_object<NetworkBase>(*this);
            ar >> inp_neurons;
            ar >> hid_neurons;
            ar >> outp_neurons;
            ar >> connections;
            after_load();
        }

	    /* @brief: template member serialize function
         * called when using i/oarchive to load/write class members via serialization
         * make serialize() private to avoid being called directly 
         * when called, an instance of this class will first be constructed using default constructor
         */
        template <typename Archive>
        void serialize(Archive &ar, const unsigned int version) {
            boost::serialization::split_member(ar, *this, version);
        }
        template <typename T> friend class Organism;

    private:
        /* data */
        std::vector<Neuron*> neurons;
        std::vector<Ti*> inp_neurons;
        std::vector<Th*> hid_neurons;
        std::vector<To*> outp_neurons;
        std::vector<Connection*> connections;

        std::vector<double> outputs;

        /* @brief: print class info 
         * do the actual printing here
         */
        std::ostream& print(std::ostream &os) const override;
    public:
        std::string comment;

        /* @brief: build a 3-layer network
         * neurons are fully connected
         */
        Network(const netID &nid, 
            const eSpinn_size &in_num = 0, 
            const eSpinn_size &hid_num = 0, 
            const eSpinn_size &out_num = 0);

        /* @brief: default constructor for using boost serialization
         * build a 3-layer network
         * with no neurons or connections
         */
        Network() : Network(0) { }

        /* @brief: copy construct a network
         * neurons & connections are newly-created copies
         */
        Network(const Network &net);

        /* @brief: destructor
         * neurons & connections are deleted here
         */
        ~Network();

        /* @brief: get network type */
        // std::vector<neuronType> getNetType() const override {
        //     return {neuron_type[0], neuron_type[1], neuron_type[2]};
        // }

        /* @brief: duplicate this network with a new id */
        Network* duplicate(const netID n);

        /* @brief: settings after loading network using serialization
         * add connections to neurons & copy pointers to neurons
         */
        void after_load();

        /* @brief: get neuron size */
        std::vector<Neuron*>::size_type get_neuron_size() const override;

        /* @brief: get input neuron size */
        typename std::vector<Ti*>::size_type get_inp_size() const;

        /* @brief: get hidden neuron size */
        typename std::vector<Th*>::size_type get_hid_size() const;

        /* @brief: get output neuron size */
        typename std::vector<To*>::size_type get_outp_size() const;

        /* @brief: get connection size */
        std::vector<Connection*>::size_type get_connection_size() const override;

        /* @brief: get input neuron type id */
        const std::type_info& get_inp_type() const;

        /* @brief: get hidden neuron type id */
        const std::type_info& get_hid_type() const;

        /* @brief: get output neuron type id */
        const std::type_info& get_outp_type() const;

        /* @brief: get connection weights */
        const std::vector<double> get_connection_weights() const override;

        /* @brief: back up connection weights */
        void backup_connection_weights();

        /* @brief: restore connection weights */
        void restore_connection_weights();

        /* @brief: get connection Hebbian type */
        std::vector<HebbianType> get_connection_hebb_type();

        /* @brief: set connection Hebbian type */
        void set_connection_hebb_type(const HebbianType &h);

        /* @brief: duplicate copy plastic rule */
        void duplicate_plastic_rule(const Network<Ti, Th, To> *net);

        /* @brief: check if the two networks have the same topology */
        bool has_same_topology(const Network<Ti, Th, To> *net);

        /* @brief: create a hidden neuron
         * based on its type 
         */
        Th *const create_hid_neuron(const neuronID &nid) const;

        /* @brief: assign hidden neurons sequence
         * n_seq is the order to activate 
         */
        void assign_hid_seq();

        /* @brief: load network inputs
         */
        void load_inputs(const double *p, const eSpinn_size n) override;

        /* @brief: load network inputs */
        void load_inputs(const std::vector<double> &p) override;


        /* @brief: run network for ONE time slot
         * apart from the default run() function
         * we also specialize 3 other run() in different network types
         * in these functions we call overloaded forward() functions based on neuron types
         * and then push results to network outputs
         * the compiler will compile the specialized functions for 
         * each template, which has been explicitly instantiated at the end of the cpp file
         * if the instantiation is not one of them, calling run() will call the default one
         */
        const std::vector<double>& run() override;

        /* @brief: load network outputs from output neurons */
        void load_outputs();

        /* @brief: save network topology to file */
        void save(const std::string &ofile);
    };

    typedef Network<Sensor, SigmNeuron, SigmNeuron> SigmNetwork;
    typedef Network<Sensor, SigmNeuron, LinrNeuron> LinrNetwork;
    typedef Network<Sensor, IzhiNeuron, IzhiNeuron> IzhiNetwork;
    typedef Network<Sensor, LifNeuron, LifNeuron> LifNetwork;
    typedef Network<Sensor, IzhiNeuron, SigmNeuron> HybridNetwork;
    typedef Network<Sensor, IzhiNeuron, LinrNeuron> HybLinNetwork;

    // specialization declarations 
    /* @brief: specialized run() - IzhiNetwork */
    template<> const std::vector<double>& IzhiNetwork::run();
    /* @brief: specialized run() - LifNetwork */
    template<> const std::vector<double>& LifNetwork::run();
    /* @brief: specialized run() - HybridNetwork */
    template<> const std::vector<double>& HybridNetwork::run();
    /* @brief: specialized run() - HybLinNetwork */
    template<> const std::vector<double>& HybLinNetwork::run();
}
