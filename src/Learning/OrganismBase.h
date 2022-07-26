/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#pragma once


#include "eSpinn_def.h"
#include "neat_def.h"
#include "Utilities/Utilities.h"
#include <iostream>

#include <boost/serialization/access.hpp>

/* @brief: OrganismBase - abstract class
 * wrap up networks for evolutionary algorithms
 * use pointer of OrganismBase to access Organism
 * initialization list: org id, which gen
 */
namespace eSpinn {
    class Species;
    class Innovation;
    class OrganismBase
    {
        friend class Species;
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
            #ifdef ESPINN_SERIALIZE_VERBOSE
            std::cout << "Serializing Organism..." << std::endl;
            #endif
            ar & org_id;
            ar & gen & fitness & winner;
            ar & species;
        }

        /* @brief: overloaded output operator<<
         * print class info 
         */
        friend std::ostream& operator<<(std::ostream &os, const OrganismBase &o) {
            return o.print(os);
        }
    protected:
        /* data */
        netID org_id;
        eSpinn_size gen;
        double fitness, orig_fit;
        bool winner, eliminate;
        double expected_offspring;
        Species *species;

        /* @brief: print class info 
         * do the actual printing here
         */
        virtual std::ostream& print(std::ostream &os) const;
    public:
        /* @brief: constructor */
        OrganismBase(const netID &oid, const eSpinn_size &g) : 
            org_id(oid), gen(g), fitness(.0), orig_fit(.0),
            winner(false), eliminate(false), expected_offspring(.0), species(nullptr) { }
        virtual ~OrganismBase() = default;

        /* @brief: get organism id */
        const netID getID() const;
        /* @brief: set organism id */
        virtual void setID(const netID &oid);

        /* @brief: duplicate this organism
         * with a new id and which generation
         */
        virtual OrganismBase* duplicate(const netID &n, const eSpinn_size &g) = 0;

        /* @brief: get which generation organism belong to */
        const eSpinn_size getGen() const;
        /* @brief: set which generation organism belong to */
        void setGen(const eSpinn_size &g);

        /* @brief: get organism fit */
        double getFit() const;
        /* @brief: set organism fit */
        void setFit(const double &f);

        /* @brief: calculate fitness */
        void calFit(const double &std_err);

        /* @brief: get organism original fit */
        double getOrigFit() const;

        /* @brief: check organism winner status */
        const bool isWinner() const;
        /* @brief: set organism as winner */
        void setWinner();

        /* @brief: set organism winner status
         * set organism as winner if its fit >= std_fit
         */
        const bool setWinner(const double &std_fit);

        /* @brief: check organism eliminate status */
        bool isDying() const;
        /* @brief: set organism as dead - should be eliminated */
        void setDead();

        /* @brief: get the number of expected offspring */
        double getExpectedOffspring() const;
        /* @brief: set the number of expected offspring */
        void setExpectedOffspring(const double &exp_num);

        /* @brief: get organism species */
        Species *const getSpecies() const;

        /* @brief: set organism species */
        void setSpecies(Species *s);

        /* @brief: get the next available neuron id
         */
        virtual const neuronID get_next_neuron_id() const = 0;

        /* @brief: get the next available connection id
         */
        virtual const connID get_next_conn_id() const = 0;

        /* @brief: randomize connection weights */
        virtual void randomizeWeights() { }

        /* @brief: randomize connection plastic terms */
        virtual void randomize_plastic_terms() { }

        /* @brief: calculate the compatibility distance
         */
        virtual double calCompatDistance(OrganismBase *org) = 0;

        /* @brief: crossover
         * random pick or average configurations
         * of the shared connections which both parents have
         * before calling this method,
         * the child org will inherit exactly the same configurations of the mom
         * then in this method it will crossover with the dad
         */
        virtual void crossover(OrganismBase* dad) { }

        /* @brief: evolve network
         * evolve network topology and connection weights (and delay, TODO later)
         */
        virtual void evolve(neuronID &next_nid, connID &next_cid, 
            std::vector<Innovation*> &innov, 
            const bool &evolving_plastic_terms)
        { }

        /* @brief: save network topology to file */
        virtual void save(const std::string &ofile) { }

        /* @brief: archive organism to file */
        virtual void archive(const std::string &ofile) { }

        /* @brief: construct organism from file */
        virtual void load(const std::string &ifile) { }

        /* relational operators */
        bool operator==(const OrganismBase &o) const {
            return fitness == o.fitness;
        }
        bool operator<(const OrganismBase &o) const {
            return fitness < o.fitness;
        }
        bool operator>(const OrganismBase &o) const {
            return fitness > o.fitness;
        }

    };

    inline bool less_fit(const OrganismBase *o1, const OrganismBase *o2) {
        return *o1 < *o2;
    }
    inline bool greater_fit(const OrganismBase &o1, const OrganismBase &o2) {
        return o1 > o2;
    }
    inline bool greater_fit(const OrganismBase *o1, const OrganismBase *o2) {
        return *o1 > *o2;
    }
}
