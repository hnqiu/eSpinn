/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#pragma once


#include "eSpinn_def.h"
#include "OrganismBase.h"
#include "Organism.h"
#include "Species.h"
#include "Innovation.h"
#include "Models/Network.h"
#include "Utilities/Utilities.h"
#include <iostream>
#include <vector>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/export.hpp>

/* @brief: Population 
 * population of organisms
 * initialization list: org, population size, solved
 */
namespace eSpinn {
    class Population
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
            ar & gen & next_neuron_id & next_conn_id & next_species_id;
            ar & champ_fit & champ_fit_ever & stagnant_gens & solved;
            ar & species & orgs;
            ar & innovation;
        }
        /* @brief: overloaded << 
         * print class info
         */
        friend std::ostream& operator<<(std::ostream &os, const Population &pop);
    private:
        /* data */
        eSpinn_size gen; // which generation
        eSpinn_size next_neuron_id, next_conn_id, next_species_id;
        double champ_fit, champ_fit_ever;
        eSpinn_size stagnant_gens;
        bool solved;
    public:
        bool evolving_plastic_term; // true when evolving p terms
        std::vector<OrganismBase *> orgs;
        std::vector<Species *> species;
        std::vector<Innovation *> innovation;
        // std::vector<eSpinn_size> winners;
    public:
        /* @brief: constructors */
        Population(OrganismBase *const o, const eSpinn_size &num,
            const eSpinn_size &g = 1, const bool randomize = true);
        /* @brief: constructor
         * initializer: which gen
         */
        Population(const eSpinn_size &g);
        /* @brief: default constructor */
        Population();
        /* @brief: destructor */
        ~Population() {
            #ifndef NDEBUG
            std::cout << "Deleting population..." << std::endl;
            #endif
            for (auto &s : species)
                delete s;
            for (auto &i : innovation)
                delete i;
            orgs.clear();
            species.clear();
            innovation.clear();
        }

        /* @brief: return the num of organisms */
        inline const eSpinn_size size() const { return orgs.size(); }

        /* @brief: return which generation */
        inline const eSpinn_size getGen() const {
            return gen;
        }

        /* @brief: set generation */
        inline void incrementGen() {
            ++gen;
        }

        /* @brief: set the next available neuron id */
        inline void set_next_neuron_id(const neuronID &n_id) {
            next_neuron_id = n_id;
        }

        /* @brief: set the next available connection id */
        inline void set_next_conn_id(const connID &c_id) {
            next_conn_id = c_id;
        }

        /* @brief: set the next available species id */
        inline void set_next_species_id(const speciesID &s_id) {
            next_species_id = s_id;
        }

        /* @brief: check if this population will solve the problem */
        bool issolved() const;

        /* @brief: problem is solved */
        void set_solved();

        /* @brief: problem is not solved */
        void reset_solved();

        /* @brief: check if is evolving plastic terms */
        bool isevolving_plastic_term() const;

        /* @brief: set plastic terms */
        void set_evolving_plastic_term(const bool p);

        /* @brief: get the champion's fitness */
        const double get_champ_fit() const;

        /* @brief: reset stagnant label */
        inline void reset_stagnant() {
            stagnant_gens = 0;
        }

        /* @brief: add an organism */
        void add_org(OrganismBase *const o);

        /* @brief: add a species */
        void add_species(Species *const s);

        /* @brief: initialize population parameters */
        void init();

        /* @brief: categrize organisms into species */
        void speciate();

        /* @brief: get the champion */
        OrganismBase *const get_champ_org() const;

        /* @brief: sort organisms in descending order
         * based on their fitness
         */
        void sort_org();

        /* @brief: sort species in descending order
         * based on their champion's fitness
         */
        void sort(std::vector<Species*> &ss);

        /* @brief: generate the next generation
         * return true if new offspring are generated
         */
        bool epoch(const eSpinn_size &g);

        /* @brief: save population to file */
        void archive(const std::string &ofile);

        /* @brief: construct population from file */
        void load(const std::string &ifile);
    };
    
}
