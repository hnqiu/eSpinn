/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#pragma once


#include "eSpinn_def.h"
#include "SpikeNeuron.h"

/* @brief: Leaky integrate-and-fire neuron
 * initialization list: id, layer, (v_thres, v_rest, tau, R)
 */
namespace eSpinn {
    class LifNeuron : public SpikeNeuron {
        /* @brief: declare serialization library as friend
         * used to grant to the serialization library access to class members
         */
        friend class boost::serialization::access;

        template<class Archive>
        void save(Archive & ar, const unsigned int version) const
        {
            #ifdef ESPINN_SERIALIZE_VERBOSE
            std::cout << "Serializing LifNeuron ";
            #endif
            ar << boost::serialization::base_object<const SpikeNeuron>(*this);
        }

        template<class Archive>
        void load(Archive & ar, const unsigned int version)
        {
            #ifdef ESPINN_SERIALIZE_VERBOSE
            std::cout << "Loading LifNeuron ";
            #endif
            ar >> boost::serialization::base_object<SpikeNeuron>(*this);
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
        static constexpr neuronType c_type = LIF;
        double v; // membrane potential
        const double v_rest, tau, R;

        /* @brief: print class info 
         * do the actual printing here
         */
        std::ostream& print(std::ostream &os) const override;
    public:
        /* @brief: constructors & destructor */
        LifNeuron(const neuronID &nid,
            const neuronLayer &nl,
            const neuronType &nt = LIF,
            const double &th = params::lif_vth, 
            const double &rest = params::lif_vrest, 
            const double &t = params::lif_tau, 
            const double &r = params::lif_R) :
            SpikeNeuron(nid, nl, nt, th), v(rest), v_rest(rest), tau(t), R(r) {
            #ifdef ESPINN_VERBOSE
            std::cout << "Constructing LifNeuron, id = " << n_id
                        << ", layer = " << n_layer
                        << ", v_threshold = " << thresh
                        << ", v_rest = " << v_rest
                        << ", tau = " << tau << ", R = " << R << std::endl;
            #endif
        }
        LifNeuron() : LifNeuron(0, L_INPUT) { }

        LifNeuron(const LifNeuron &node) : SpikeNeuron(node),
            v(node.v_rest), v_rest(node.v_rest), tau(node.tau), R(node.R) {
            #ifdef ESPINN_VERBOSE
            std::cout << "Copy constructing LifNeuron #" << n_id << std::endl;
            #endif
        }
        ~LifNeuron() {
            #ifdef ESPINN_VERBOSE
            std::cout << "Destructing LifNeuron #" << n_id << std::endl;
            #endif
        }

        /* @brief: get neuron type */
        static const neuronType getClassType() {
            return c_type;
        }

        /* @brief: duplicate this neuron */
        LifNeuron* duplicate() override;
        
        /* @brief: reset neuron status */
        void reset() override;

        /* @brief: get the unspiked portion of the membrane potential */
        const double get_unspiked_potential() const override;

        /* @brief: step neuron for ONE timestep */
        void step() override;
    };
    
    
}