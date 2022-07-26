/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#pragma once


#include "eSpinn_def.h"
#include "SpikeNeuron.h"

/* @brief: Izhikevich neuron
 * initialization list: id, layer, (threshold, v, u, a, b, c, d)
 */
namespace eSpinn {
    class IzhiNeuron : public SpikeNeuron {
        /* @brief: declare serialization library as friend
         * used to grant to the serialization library access to class members
         */
        friend class boost::serialization::access;

        template<class Archive>
        void save(Archive & ar, const unsigned int version) const
        {
            #ifdef ESPINN_SERIALIZE_VERBOSE
            std::cout << "Serializing IzhiNeuron ";
            #endif
            ar << boost::serialization::base_object<const SpikeNeuron>(*this);
        }

        template<class Archive>
        void load(Archive & ar, const unsigned int version)
        {
            #ifdef ESPINN_SERIALIZE_VERBOSE
            std::cout << "Loading IzhiNeuron ";
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
        static constexpr neuronType c_type = IZHIKEVICH;
        double v, u; // membrane potential & recovery parameter
        const double a, b, c, d;

        /* @brief: print class info 
         * do the actual printing here
         */
        std::ostream& print(std::ostream &os) const override;
    public:
        /* @brief: constructors & destructor */
        IzhiNeuron(const neuronID &nid, 
            const neuronLayer &nl, 
            const neuronType &nt = IZHIKEVICH,
            const double &th = params::izhi_thresh, 
            const double &aa = params::izhi_a, 
            const double &bb = params::izhi_b, 
            const double &cc = params::izhi_c, 
            const double &dd = params::izhi_d) :
            SpikeNeuron(nid, nl, nt, th), v(cc), u(bb*cc), a(aa), b(bb), c(cc), d(dd) {
            #ifdef ESPINN_VERBOSE
            std::cout << "Constructing IzhiNeuron, id = " << n_id
                        << ", layer = " << n_layer
                        << ", threshold = " << thresh
                        << ", v = " << v << ", u = " << u 
                        << ", a = " << a << ", b = " << b 
                        << ", c = " << c << ", d = " << d << std::endl; 
            #endif
        }
        IzhiNeuron() : IzhiNeuron(0, L_INPUT) { }

        IzhiNeuron(const IzhiNeuron &node) : SpikeNeuron(node), 
            v(node.c), u(node.b * node.c), a(node.a), b(node.b), c(node.c), d(node.d) {
            #ifdef ESPINN_VERBOSE
            std::cout << "Copy constructing IzhiNeuron #" << n_id << std::endl;
            #endif
        }
        ~IzhiNeuron() {
            #ifdef ESPINN_VERBOSE
            std::cout << "Destructing IzhiNeuron #" << n_id << std::endl;
            #endif
        }

        /* @brief: get neuron type */
        static const neuronType getClassType() {
            return c_type;
        }

        /* @brief: duplicate this neuron */
        IzhiNeuron* duplicate() override;
        
        /* @brief: reset neuron status */
        void reset() override;

        /* @brief: get the unspiked portion of the membrane potential */
        const double get_unspiked_potential() const override;

        /* @brief: step neuron for ONE timestep */
        void step() override;
    };
    
    
}