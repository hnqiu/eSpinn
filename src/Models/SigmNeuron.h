/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#pragma once


#include "eSpinn_def.h"
#include "Neuron.h"
#include <cmath>

/* @brief: Sigmoid neuron
 * initialization list: id, layer, (lambda = 1.0)
 */
namespace eSpinn {
    class SigmNeuron : public Neuron {
        /* @brief: declare serialization library as friend
         * used to grant to the serialization library access to class members
         */
        friend class boost::serialization::access;

        template<class Archive>
        void save(Archive & ar, const unsigned int version) const
        {
            #ifdef ESPINN_SERIALIZE_VERBOSE
            std::cout << "Serializing SigmNeuron ";
            #endif
            ar << boost::serialization::base_object<const Neuron>(*this);
            ar << lambda;
        }

        template<class Archive>
        void load(Archive & ar, const unsigned int version)
        {
            #ifdef ESPINN_SERIALIZE_VERBOSE
            std::cout << "Loading SigmNeuron ";
            #endif
            ar >> boost::serialization::base_object<Neuron>(*this);
            ar >> lambda;
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

    private:
        /* data */
        static constexpr neuronType c_type = SIGMOID;
        double lambda;
        double i, o;

        /* @brief: print class info 
         * do the actual printing here
         */
        std::ostream& print(std::ostream &os) const override;
    public:
        /* @brief: constructors & destructor */
        SigmNeuron(const neuronID &nid, const neuronLayer &nl, const double &l, const neuronType &nt = SIGMOID): 
            Neuron(nid, nl, nt), lambda(l), i(.0), o(.0) 
        {
            #ifdef ESPINN_VERBOSE
            std::cout << "Constructing sigmoidal node, id = " << n_id 
            << ", layer = " << n_layer << ", lambda = " << lambda << std::endl;
            #endif
        }
        SigmNeuron(const neuronID &nid, const neuronLayer &nl) : 
            SigmNeuron(nid, nl, params::sigm_lambda) { }
        
        SigmNeuron(): SigmNeuron(0, L_INPUT) {
            #ifdef ESPINN_VERBOSE
            std::cout << "using default id, layer & lambda" << std::endl;
            #endif
        }
        SigmNeuron(const SigmNeuron &node) : 
            Neuron(node), lambda(node.lambda), i(.0), o(.0) {
            #ifdef ESPINN_VERBOSE
            std::cout << "Copy constructing SigmNeuron #" << n_id << std::endl;
            #endif
            }
        
        ~SigmNeuron() {
            #ifdef ESPINN_VERBOSE
            std::cout << "Destructing SigmNeuron #" << n_id << std::endl;
            #endif
        }

        /* @brief: get neuron type */
        static const neuronType getClassType() {
            return c_type;
        }

        /* @brief: duplicate this neuron */
        SigmNeuron* duplicate() override;

        /* @brief: get lambda */
        const double getLambda() const { return lambda; }

        /* @brief: set lambda */
        void setLambda(const double &l) { lambda = l; }

        /* @brief: increase lambda */
        void increaseLambda(const double &l) { lambda += l; }

        /* @brief: load input data
         * use this method for input-layer neurons
         */
        void load_input(const double *val) override;

        /* @brief: load input data
         * use this method for non-input-layer neurons
         * accumulate synaptic inputs
         */
        void load_input() override;

        /* @brief: get neuron output */
        const double getOut() const override { return o; }

        /* @brief: activate neuron and get output */        
        const double activate();

        /* @brief: forward neuron 
         * first accumulate synaptic inputs
         * then call activate() to get neuron output
         * finally, transmit output to all outgoing connections
         */
        void forward() override;

    };
    
    
}
