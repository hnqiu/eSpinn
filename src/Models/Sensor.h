/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#pragma once


#include "eSpinn_def.h"
#include "Neuron.h"

/* @brief: Sensor neuron
 * initialization list: id, layer
 */
namespace eSpinn {
    class Sensor : public Neuron {
        /* @brief: declare serialization library as friend
         * used to grant to the serialization library access to class members
         */
        friend class boost::serialization::access;

        template<class Archive>
        void save(Archive & ar, const unsigned int version) const
        {
            #ifdef ESPINN_SERIALIZE_VERBOSE
            std::cout << "Serializing Sensor ";
            #endif
            ar << boost::serialization::base_object<const Neuron>(*this);
        }

        template<class Archive>
        void load(Archive & ar, const unsigned int version)
        {
            #ifdef ESPINN_SERIALIZE_VERBOSE
            std::cout << "Loading Sensor ";
            #endif
            ar >> boost::serialization::base_object<Neuron>(*this);
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
        static constexpr neuronType c_type = SENSOR;
        double sense_val;

        /* @brief: print class info 
         * do the actual printing here
         */
        std::ostream& print(std::ostream &os) const override;
    public:
        /* @brief: constructors & destructor */
        Sensor(const neuronID &nid, const neuronLayer &nl, const neuronType &nt = SENSOR): 
            Neuron(nid, nl, nt), sense_val(.0)
        {
            #ifdef ESPINN_VERBOSE
            std::cout << "Constructing Sensor, id = " << n_id 
            << ", layer = " << n_layer << std::endl;
            #endif
        }
        Sensor() : Sensor(0, L_INPUT) { }

        Sensor(const Sensor &node) : Neuron(node), sense_val(.0) {
            #ifdef ESPINN_VERBOSE
            std::cout << "Copy constructing Sensor #" << n_id << std::endl;
            #endif
        }
        ~Sensor() {            
            #ifdef ESPINN_VERBOSE
            std::cout << "Destructing Sensor #" << n_id << std::endl;
            #endif
        }

        /* @brief: get neuron type */
        static const neuronType getClassType() {
            return c_type;
        }

        /* @brief: duplicate this neuron */
        Sensor* duplicate() override;

        /* @brief: load input data
         * use this method for input-layer neurons
         */
        void load_input(const double *val) override;
        
        /* @brief: load input data
         * use this method for non-input-layer neurons
         * accumulate synaptic inputs
         */
        void load_input() override;

        /* @brief: cap sensor value within [-1.0, 1.0] */
        void rectify();

        /* @brief: get neuron output */
        const double getOut() const override { return sense_val; }

        /* @brief: forward neuron 
         * first accumulate synaptic inputs
         * then call transmit()
         */
        void forward() override;
    };
    typedef Sensor LinrNeuron;
}
