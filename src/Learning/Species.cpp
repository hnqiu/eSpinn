/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#include "Species.h"
// put include here to avoid circular dependency
#include "Population.h"
using namespace eSpinn;


/* @brief: overloaded <<
 * print class info
 */
std::ostream& eSpinn::operator<<(std::ostream &os, const Species &species) {
    os << "Species #" << species.s_id << ", age = " << species.age 
        << ", age_impv = " << species.age_last_improved
        << ", max_fit = " << species.max_fitness 
        << ", org_size = " << species.size();
    #ifdef ESPINN_VERBOSE
    if (!species.orgs.empty()) {
        os << ", first org is: " << std::endl << *species.front();
    }
    #endif
    return os;
}


/* @brief: get species id */
const speciesID Species::getID() const {
    return s_id;
}


/* @brief: add an organism */
void Species::add_org(OrganismBase *const o) {
    orgs.push_back(o);
}


/* @brief: remove an organism */
void Species::remove_org(OrganismBase *const o) {
    bool deleted(false);
    for (auto org = orgs.begin(); org != orgs.end(); ++org) {
        if(*org == o) {
            orgs.erase(org);
            deleted = true;
            break;
        }
    }
    if (!deleted)
        std::cerr << "Warning: attempt to delete nonexistent organism from species!" << std::endl;
}


/* @brief: sort organisms in descending order */
void Species::sort_orgs() {
    std::stable_sort(orgs.begin(), orgs.end(), \
        static_cast<bool (*)(const OrganismBase*, const OrganismBase*)>(greater_fit));
}


/* @brief: return the representative organism */
OrganismBase *const Species::front() const {
    return orgs.front();
}


/* @brief: get the champion organism */
OrganismBase *const Species::get_champ() {
    auto it = std::max_element(orgs.begin(), orgs.end());
    champ = *it;
    return champ;
}

/* @brief: get champion fitness */
const double Species::champ_fit() const {
    return champ->getFit();
}


/* @brief: record age improved */
void Species::record_age_improved() {
    age_last_improved = age;
}


/* @brief: get the number of expected offspring */
eSpinn_size Species::getExpOffspring() const {
    return expected_offspring;
}

/* @brief: set the number of expected offspring */
void Species::setExpOffspring(const eSpinn_size &exp_num) {
    expected_offspring = exp_num;
}


/* @brief: increase the number of expected offspring */
void Species::incExpOffspring(const eSpinn_size &exp_num) {
    expected_offspring += exp_num;
}


/* @brief: calculate the expected size of offspring */
void Species::count_offspring(double &fracpart) {
    double expected_num = fracpart;
    for (auto &o : orgs) {
        expected_num += o->getExpectedOffspring();
    }
    expected_offspring = (eSpinn_size) floor(expected_num);
    fracpart = expected_num - expected_offspring;
}


/* @brief: adjust organisms' fitness
 * based on the fitness sharing scheme
 * and mark the top organisms as survivors
 */
void Species::adjustFit() {
    int age_debt = age - age_last_improved - neat::dropoff_age;
    if (age_debt >= 0)
        std::cout << "Species #" << s_id << " is being punished." << std::endl;
    
    for (auto &o : orgs) {
        // backup original fitness
        o->orig_fit = o->fitness;
        //adjust fit
        // if (o->fitness < 0) {
        //     o->fitness += 1.0;
        //     o->fitness *= 0.2;
        // }
        if (age_debt >= 0)
            // penalty for not progressing
            o->fitness *= 0.01;
        
        // share fitness within the species
        o->fitness /= size();
    }

    sort_orgs();
    champ = front();

    // update age_last_improved
    max_fitness = front()->getOrigFit();
    if (max_fitness > max_fit_ever) {
        max_fit_ever = max_fitness;
        age_last_improved = age;
    }
    
    // mark survivors, at least one will survive
    int num_survivor = 1;
    if (neat::survival_thresh >= 1.0)
        num_survivor = size();
    else
        num_survivor = neat::survival_thresh * size() + 1;
    auto cur_org = orgs.begin();
    std::advance(cur_org, num_survivor);
    while (cur_org != orgs.end()) {
        (*cur_org)->setDead();
        ++cur_org;
    }
}


/* @brief: reproduce offspring */
void Species::reproduce(const eSpinn_size &gen, Population *pop,
    const std::vector<Species*> &sorted_species) 
{
    // choose dad org from sorted_species
    bool champ_done = false;
    auto parent_size = size();
    for (eSpinn_size count = 0; count < expected_offspring; ++count) {
        // Network *child = new Network(gen);
        OrganismBase *child = nullptr;

        // elitism, duplicate the champion
        if (!champ_done && expected_offspring > 5) {
            // std::cout << "Clone the best organism in species " << s_id << std::endl;
            // by this time the first organism should be the champion 
            child = front()->duplicate(count, gen);
            champ_done = true;
        }
        else if (rand() < neat::mutate_only_prob) {
            // duplicate and mutate
            auto index = rand(0, parent_size-1); // use the parent orgs
            child = orgs[index]->duplicate(count, gen);
            child->evolve(pop->next_neuron_id, pop->next_conn_id, pop->innovation,
                pop->evolving_plastic_term);
        }
        else {
            // crossover (mate)
            // first pick mom
            auto index_mom = rand(0, parent_size-1);
            auto mom = orgs[index_mom];

            // pick dad, either a random one from this species or 
            // the champ of a diffrent species
            OrganismBase *dad;
            if (rand() < neat::mate_within_species_rate) {
                // mate within species
                auto index_dad = rand(0, parent_size-1);
                dad = orgs[index_dad];
            }
            else {
                // mate interspecies
                // use normal distribution to tend to choose a better species
                auto species_rank = std::abs(rand_normal(0, 0.35));
                if (species_rank > 1.0) species_rank = 1.0;
                // choose dad org from sorted_species to avoid
                // the possibility of mating mom with a newly created child
                int index_species = static_cast<int>(floor(
                            species_rank*(sorted_species.size()-1)+0.5 ));
                dad = sorted_species[index_species]->front();
            }
            // set mom as the better parent
            // auto parent = (*mom > *dad) ? mom : dad;
            if (mom->getOrigFit() < dad->getOrigFit()) { // use the orig fit
                std::swap(mom, dad);
            }

            // mating: inherit from mom and crossover with dad
            // mutate if parents are the same org
            child = mom->duplicate(count, gen);
            if (mom == dad) {
                child->evolve(pop->next_neuron_id, pop->next_conn_id, 
                    pop->innovation, pop->evolving_plastic_term);
            }
            else {
                child->crossover(dad);
            }
        }

        // add the child to species
        bool match = false;
        for (auto &ss : pop->species) {
            // the first organism is the representative of the species
            auto first_net = ss->front();
            if (child->calCompatDistance(first_net) < neat::compat_threshold) {
                // add child to this species
                ss->add_org(child);
                child->setSpecies(ss);
                match = true;
                break;
            }
        }
        // if no match, create a new species
        if (!match) {
            Species *newspecies = new Species(pop->next_species_id++);
            pop->add_species(newspecies);
            newspecies->add_org(child);
            child->setSpecies(newspecies);
            #ifdef ESPINN_VERBOSE
            std::cout << "New species created. Org representative is: "
                << std::endl << *child << std::endl;
            #endif
        }
    }
}
