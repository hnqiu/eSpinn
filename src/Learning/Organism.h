/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#pragma once


#include "eSpinn_def.h"
#include "OrganismBase.h"
#include "Innovation.h"
#include <iostream>
#include <random>
#include <typeinfo>

#include <boost/serialization/base_object.hpp>

/* @brief: Organism 
 * wrap up networks for evolutionary algorithms
 * initialization list: net, which gen
 */
namespace eSpinn {
    template <typename T>
    class Organism : public OrganismBase
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
        template <typename Archive>
        void serialize(Archive &ar, const unsigned int version) {
            ar & net;
            ar & boost::serialization::base_object<OrganismBase>(*this);
        }

    private:
        /* data */
        T *net;

        /* @brief: print class info 
         * do the actual printing here
         */
        std::ostream& print(std::ostream &os) const override;
    public:
        /* @brief: constructor
         * constructed from pointer to net
         * org manages the lifecycle of net
         */
        Organism(T *const t, const eSpinn_size &g) : 
            OrganismBase(t->getID(), g), net(t) 
        {
            #ifdef ESPINN_VERBOSE
            std::cout << "Created organism #" << getID() << std::endl;
            #endif
        }
        Organism(T *const t) : OrganismBase(t->getID(), 0), net(t) { }

        /* @brief: constructor
         *  constructed with network specifications
         * @params:
         *      - network id
         *      - input neuron number
         *      - hidden neuron number
         *      - output neuron number
         *      - (opt) which generation
         */
        Organism(const netID &nid, const eSpinn_size &in_num,
            const eSpinn_size &hid_num, const eSpinn_size &out_num,
            const eSpinn_size &g = 0)
            : OrganismBase(nid, g), net(new T(nid, in_num, hid_num, out_num))
        { }

        /* @brief: default constructor for using boost serialization */
        Organism() : OrganismBase(0, 0), net() { }
        
        /* @brief: copy constructor
         * copy the object net points to, not the pointer itself
         */
        Organism(const Organism &org) : 
            OrganismBase(org.org_id, org.gen), net(new T(*org.net)) 
        {
            #ifndef NDEBUG
            std::cout << "Organism #" << getID() << " copied!" << std::endl;
            #endif
        }
        ~Organism() {
            #ifdef ESPINN_VERBOSE
            std::cout << "Deleting organism" << std::endl;
            #endif
            delete net;
            net = nullptr;
        }

        /* @brief: set organism id
         * set both org_id & net_id
         */
        void setID(const netID &oid) override;

        /* @brief: duplicate this organism
         * with a new id and which generation
         * method use new operator, should delete the returned object
         * maually or in wrapped up classes
         */
        Organism* duplicate(const netID &n, const eSpinn_size &g) override;

        /* @brief: get the next available neuron id
         */
        const neuronID get_next_neuron_id() const override;

        /* @brief: get the next available connection id
         */
        const connID get_next_conn_id() const override;

        /* @brief: return pointer to net */
        T *const getNet() const { return net; }

        /* @brief: calculate the compatibility distance
         */
        double calCompatDistance(OrganismBase *org) override;

        /* @brief: randomize connection weights */
        void randomizeWeights() override;

        /* @brief: randomize connection plastic terms */
        void randomize_plastic_terms() override;

        /* @brief: duplicate plastic rule */
        void duplicate_plastic_rule(const Organism<T> *org);

        /* @brief: evolve network
         * evolve network topology and connection weights (and delay, TODO later)
         */
        void evolve(neuronID &next_nid, connID &next_cid, 
            std::vector<Innovation*> &innov, 
            const bool &evolving_plastic_terms) override;

        /* @brief: mutate connection plastic terms */
        void mutate_plastic_terms();

        /* @brief: mutate connection weight */
        void mutateWeights();

        /* @brief: mutate sigmoid neuron lambda */
        void mutateLambda();

        /* @brief: crossover
         * random pick or average configurations
         * of the shared connections which both parents have
         * before calling this method,
         * the child org will inherit exactly the same configurations of the mom
         * then in this method it will crossover with the dad
         */
        void crossover(OrganismBase* dad) override;

        /* @brief: add a neuron
         * split an existing connection into two and add a new neuron in between
         * first find an existing connection to mutate
         * create a neuron & connections
         * then check if this mutation has already existed
         * assign neuron id and connection ids
         * then insert neuron into vector hid_neurons
         * the insert position is based on the sequences that hidden neurons are activated
         * finally, assign hidden neurons sequence again    
         */
        void addNeuron(neuronID &next_nid, connID &next_cid, 
            std::vector<Innovation*> &innov);

        /* @brief: add a hidden neuron that connects input and output neurons
         * get the next neuron id and check if it's recorded in the global innovation
         * assign next connection id
         * create a hidden neuron and push it to network
         * fully connect the neuron from input layer and to output layer
         * insert newly created connections
         */
        void add_neuron_in2out(connID &next_cid_global, std::vector<Innovation*> &innov);

        /* @brief: check if the connection already exists */
        const bool connection_exists(const neuronID &iid, const neuronID &oid) const;

        /* @brief: add a connection
         * find inode & onode to construct a new connection
         * first make sure the connection has not existed yet
         * then check if the innovation is novel
         */
        void addConnection(connID &next_cid, std::vector<Innovation*> &innov);

        /* @brief: save network topology to file */
        void save(const std::string &ofile) override;

        /* @brief: archive organism to file */
        void archive(const std::string &ofile) override;

        /* @brief: construct organism from file */
        void load(const std::string &ifile) override;
    };

    // template<typename T>
    // bool greater_fit(const Organism<T> *o1, const Organism<T> *o2) {
    //     return *o1 > *o2;
    // }
 
}