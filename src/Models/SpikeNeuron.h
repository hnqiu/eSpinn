/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#pragma once


#include "eSpinn_def.h"
#include "Neuron.h"
#include <bitset>

/* @brief: SpikeNeuron: base spiking neuron class
 */
namespace eSpinn {
    class SpikeNeuron : public Neuron {
        /* @brief: declare serialization library as friend
         * used to grant to the serialization library access to class members
         */
        friend class boost::serialization::access;

        template<class Archive>
        void save(Archive & ar, const unsigned int version) const
        {
            ar << boost::serialization::base_object<const Neuron>(*this);
            ar << thresh;
        }

        template<class Archive>
        void load(Archive & ar, const unsigned int version)
        {
            ar >> boost::serialization::base_object<Neuron>(*this);
            ar >> thresh;
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

    protected:
        /* data */
        static constexpr neuronType c_type = SPIKING;
        double thresh; // membrance threshold
        double inc; // input current
        bool spike;
        std::bitset<params::TIMESTEP> spike_train;

        /* @brief: print class info 
         * do the actual printing here
         */
        std::ostream& print(std::ostream &os) const override;
    public:
        /* @brief: constructors & destructor */
        SpikeNeuron(const neuronID &nid, const neuronLayer &nl,
            const neuronType &nt = SPIKING, const double &th = params::izhi_thresh) :
            Neuron(nid, nl, nt), thresh(th), inc(.0), spike(0),
            spike_train(std::bitset<params::TIMESTEP>()) { }
        SpikeNeuron() : SpikeNeuron(0, L_INPUT) { }

        SpikeNeuron(const SpikeNeuron &node) : 
            Neuron(node), thresh(node.thresh), inc(.0), spike(0),
            // create a new spike train, not use the original one
            spike_train(std::bitset<params::TIMESTEP>()) { }
        
        virtual ~SpikeNeuron() { }

        /* @brief: get neuron type */
        static const neuronType getClassType() {
            return c_type;
        }

        /* @brief: duplicate this neuron */
        SpikeNeuron* duplicate() override;
        
        /* @brief: get current spike status */
        const bool getSpike() const;

        /* @brief: get spike status at this timestep */
        const bool getSpike(const eSpinn_size &pos) const;

        /* @brief: push spike status */
        void pushSpike(const bool &s);

        /* @brief: set spike status at pos */
        void setSpike(const eSpinn_size &pos);

        /* @brief: reset spike status at pos */
        void resetSpike(const eSpinn_size &pos);

        /* @brief: get accumulated spike number */
        const eSpinn_size getSpikeNum() const;

        /* step neuron for ONE timestep */
        virtual void step() { }

        /* @brief: reset neuron status */
        virtual void reset();

        /* @brief: load input data
         * use this method for input-layer neurons
         */
        void load_input(const double *val) override;

        /* @brief: load input data
         * accumulate synaptic inputs
         * use this method for non-input-layer neurons
         */
        void load_input() override;

        /* @brief: get the unspiked portion of the membrane potential */
        virtual const double get_unspiked_potential() const { return .0; }

        /* @brief: get neuron output */
        const double getOut() const override;

        /* @brief: transmit spike status to all outgoing SPIKING connections */
        void transmit();

        /* @brief: transmit firing rate to all outgoing NON-SPIKING connections */
        void transmit_rate();

        /* @brief: forward SpikeNeuron for ONE timestep 
         * first accumulate synaptic inputs
         * then use step function to get output spike
         * finally, transmit output to all outgoing connections
         */
        void forward() override;
    };
        
}