/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#pragma once


#include "eSpinn_def.h"
#include "neat_def.h"
#include "OrganismBase.h"
#include "Utilities/Utilities.h"
#include <iostream>
#include <vector>
#include <algorithm>

/* @brief: Species 
 * Species of organisms
 * initialization list: org
 */
namespace eSpinn {
    class Population;
    class Species
    {
        friend class Population;
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
            ar & s_id & age & age_last_improved & novel;
            ar & orgs;
        }
        /* @brief: overloaded << 
         * print class info
         */
        friend std::ostream& operator<<(std::ostream &os, const Species &species);
    private:
        /* data */
        speciesID s_id;
        eSpinn_size age, age_last_improved;
        std::vector<OrganismBase *> orgs;
        OrganismBase *champ;
        eSpinn_size expected_offspring;
        double max_fitness, max_fit_ever;
        bool novel, dying;
    public:
        /* @brief: constructor
         * set age = 0, novel = true
         * use this method when reproducing offspring
         */
        Species(const speciesID &sid) :
            s_id(sid), age(0), age_last_improved(0), orgs(), champ(),
            expected_offspring(0), max_fitness(.0), max_fit_ever(-1.0),
            novel(true), dying(false) { }
        
        /* @brief: constructor
         * set age = sage, novel = false
         * use this method when initializing population
         */
        Species(const speciesID &sid, const eSpinn_size &sage) :
            s_id(sid), age(sage), age_last_improved(0), orgs(), champ(),
            expected_offspring(0), max_fitness(.0), max_fit_ever(-1.0),
            novel(false), dying(false) { }
        
        /* @brief: constructor for boost serialization */
        Species() : Species(0, 0) { }
        ~Species() {
            #ifdef ESPINN_VERBOSE
            std::cout << "Deleting species..." << std::endl;
            #endif
            for (auto &o : orgs)
                delete o;
            champ = nullptr;
            orgs.clear();
        }

        /* @brief: get species id */
        const speciesID getID() const;

        /* @brief: get the size of organisms */
        inline const eSpinn_size size() const { return orgs.size(); }

        /* @brief: add an organism */
        void add_org(OrganismBase *const o);

        /* @brief: remove an organism */
        void remove_org(OrganismBase *const o);

        /* @brief: sort organisms in descending order */
        void sort_orgs();

        /* @brief: return the representative organism */
        OrganismBase *const front() const;

        /* @brief: get the champion organism */
        OrganismBase *const get_champ();

        /* @brief: get champion fitness */
        const double champ_fit() const;

        /* @brief: record age improved */
        void record_age_improved();

        /* @brief: get the number of expected offspring */
        eSpinn_size getExpOffspring() const;

        /* @brief: set the number of expected offspring */
        void setExpOffspring(const eSpinn_size &exp_num);

        /* @brief: increase the number of expected offspring */
        void incExpOffspring(const eSpinn_size &exp_num = 1);

        /* @brief: calculate the expected size of offspring */
        void count_offspring(double &fracpart);

        /* @brief: adjust organisms' fitness
         * based on the fitness sharing scheme
         * and mark the top organisms as survivors
         */
        void adjustFit();

        /* @brief: reproduce offspring */
        void reproduce(const eSpinn_size &gen, Population *pop,
            const std::vector<Species*> &sorted_species);

        /* relational operators */
        bool operator==(const Species &s) const {
            return max_fitness == s.max_fitness;
        }
        bool operator<(const Species &s) const {
            return max_fitness < s.max_fitness;
        }
        bool operator>(const Species &s) const {
            return max_fitness > s.max_fitness;
        }
    };

    inline bool greater_fit(const Species *s1, const Species *s2) {
        return *s1 > *s2;
    }
}
