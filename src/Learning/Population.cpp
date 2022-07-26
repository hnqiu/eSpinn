/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#include "Population.h"
using namespace eSpinn;


/* @brief: overloaded << 
 * print class info
 */
std::ostream& eSpinn::operator<<(std::ostream &os, const Population &pop) {
    os << "Pop: gen #" << pop.gen << " " << pop.next_neuron_id
        << " " << pop.next_conn_id << " " << pop.next_species_id
        << "; champ_fit = " << pop.champ_fit 
        << ", champ_fit_ever = " << pop.champ_fit_ever
        << ", solved? = " << pop.solved << "; org_size = " << pop.size();
    return os;
}


/* @brief: constructor
 * spawn a population with an organism
 */
Population::Population(OrganismBase *const o, const eSpinn_size &num,
    const eSpinn_size &g, const bool randomize
    ) : 
    gen(g), 
    next_neuron_id(0), next_conn_id(0), next_species_id(0),
    champ_fit(.0), champ_fit_ever(.0), stagnant_gens(0), solved(false),
    evolving_plastic_term(0),
    orgs(std::vector<OrganismBase *>()), species(std::vector<Species *>()),
    innovation(std::vector<Innovation *>())
{
    for (eSpinn_size i = 0; i < num; ++i) {
        auto org = o->duplicate(i, g);
        if (randomize) {
            org->randomizeWeights();
        }
        add_org(org);
    }
}


/* @brief: constructor
 * initializer: which gen
 */
Population::Population(const eSpinn_size &g) : 
    gen(g), 
    next_neuron_id(0), next_conn_id(0), next_species_id(0),
    champ_fit(.0), champ_fit_ever(.0), stagnant_gens(0), solved(false), 
    evolving_plastic_term(0),
    orgs(), species(), innovation()
{ }


/* @brief: default constructor */
Population::Population() : Population(0) { }


/* @brief: check if this population will solve the problem */
bool Population::issolved() const { return solved; }


/* @brief: problem is solved */
void Population::set_solved() {
    solved = true;
}


/* @brief: problem is not solved */
void Population::reset_solved() {
    solved = false;
}


/* @brief: check if is evolving plastic terms */
bool Population::isevolving_plastic_term() const {
    return evolving_plastic_term;
}


/* @brief: set plastic terms */
void Population::set_evolving_plastic_term(const bool p) {
    evolving_plastic_term = p;
}


/* @brief: get the champion's fitness */
const double Population::get_champ_fit() const {
    return champ_fit;
}


/* @brief: add an organism */
void Population::add_org(OrganismBase *const o) {
    orgs.push_back(o);
}

/* @brief: add a species */
void Population::add_species(Species *const s) {
    species.push_back(s);
}


/* @brief: initialize population parameters */
void Population::init() {
    #ifndef NDEBUG
    std::cout << "Initializing population parameters and speciating organisms..." << std::endl;
    #endif
    set_next_neuron_id(orgs.back()->get_next_neuron_id());
    set_next_conn_id(orgs.back()->get_next_conn_id());
    speciate();
}


/* @brief: categrize organisms into species */
void Population::speciate() {
    int count = 0;

    // find a species for each organism
    for (auto &o : orgs) {
        if (species.empty()) {
            // create the first species
            Species *newspecies = new Species(count++, 1);
            add_species(newspecies);
            newspecies->add_org(o);
            o->setSpecies(newspecies); // point organism to its own species
        } else {
            bool match = 0;
            for (auto &s : species) {
                // the first net is the representative of the species
                // species should at least have one organism, so first_net should not be empty
                auto first_org = s->front();
                if (o->calCompatDistance(first_org) < neat::compat_threshold) {
                    // add org to this species
                    s->add_org(o);
                    o->setSpecies(s);
                    match = 1;
                    break; // search is over
                }
            }
            // if no match, create a new species
            if (!match) {
                Species *newspecies = new Species(count++, 1);
                add_species(newspecies);
                newspecies->add_org(o);
                o->setSpecies(newspecies); // point organism to its own species
            }
            
        }
    }
    set_next_species_id(count);
}


/* @brief: get the champion */
OrganismBase *const Population::get_champ_org() const {
    return *std::max_element(orgs.begin(), orgs.end(), less_fit);
}


/* @brief: sort organisms in descending order
 * based on their fitness
 */
void Population::sort_org() {
    std::stable_sort(orgs.begin(), orgs.end(), \
        static_cast<bool (*)(const OrganismBase*, const OrganismBase*)>(greater_fit));
}


/* @brief: sort species in descending order
 * based on their champion's fitness
 */
void Population::sort(std::vector<Species*> &ss) {
    std::stable_sort(ss.begin(), ss.end(), \
        static_cast<bool (*)(const Species*, const Species*)>(greater_fit));
}


/* @brief: generate the next generation
 * return true if new offspring are generated
 */
bool Population::epoch(const eSpinn_size &generation) {

    // check species, should not be empty
    assert(!species.empty() && "Error: no species in population!");

    // adjust fitness and mark survivors
    for (auto &s : species)
        s->adjustFit();
    
    // use an extra vector to sort species based on the max fit
    std::vector<Species*> sorted_species = species;
    sort(sorted_species);
    
    
    // calculate expected offspring
    double avg_fit = 0.0;
    for (auto &o : orgs)
        avg_fit += o->getFit();
    avg_fit /= size();
    // expected num of offspring for each individual
    for (auto &o : orgs)
        o->setExpectedOffspring(o->getFit()/avg_fit);
    // expected num of offspring for each species
    int total_expected = 0;
    double fracpart = 0.0;
    for (auto &s : species) {
        s->count_offspring(fracpart);
        total_expected += s->getExpOffspring();
    }
    // may lose precision in offspring count
    // in that case, give an extra baby to the species expecting the most
    if (total_expected < size()) {
        auto most_expected_species = species.begin();
        int most_expected = 0;
        for (auto ss = species.begin(); ss != species.end(); ++ss) {
            if (most_expected <= (*ss)->getExpOffspring()) {
                most_expected = (*ss)->getExpOffspring();
                most_expected_species = ss;
            }
        }
        (*most_expected_species)->incExpOffspring();
        ++total_expected;
    
        // the numbers are still not met when:
        // a stagnant species dominates the entire population and
        // gets killed by its age
        if (total_expected < size()) {
            std::cerr << BnR_ERROR << "Wrong number (" << total_expected
                << ") when calculating expected offspring (" << size() << ")\n";
            for (auto &s : species)
                s->setExpOffspring(0);
            (*most_expected_species)->setExpOffspring(size());
        }
    }
    
    
    // check if population stagnant
    auto curspecies = sorted_species.begin();
    champ_fit = ((*curspecies)->front())->getOrigFit();
    if (champ_fit > champ_fit_ever) {
        champ_fit_ever = champ_fit;
        stagnant_gens = 0;
        std::cout << "New fitness record: " << champ_fit << std::endl;
    } else {
        ++stagnant_gens;
        std::cout << "Current fitness record is: " << champ_fit << std::endl
        << stagnant_gens << " generations since last highest fitness record: "
        << champ_fit_ever << std::endl;
    }
    // if reach neat::stagnant_gen, keep the first two species
    // update: if stagnant, keep the pop & return
    if (stagnant_gens >= neat::stagnant_gen) {
        // return false;
        stagnant_gens = 0;
        curspecies = sorted_species.begin();
        if (sorted_species.size() == 1) {
            (*curspecies)->setExpOffspring(size());
        } else {
            eSpinn_size half_pop = size()/2;
            (*curspecies)->setExpOffspring(half_pop);
            (*curspecies)->record_age_improved();
            ++curspecies;
            (*curspecies)->setExpOffspring(size() - half_pop);
            (*curspecies)->record_age_improved();
            ++curspecies;
            while (curspecies != sorted_species.end()) {
                (*curspecies)->setExpOffspring(0);
                ++curspecies;
            }
        }
        
    }
    
    // delete organisms marked as dead
    for (auto cur_org = orgs.begin(); cur_org != orgs.end(); ) {
        if ((*cur_org)->isDying()) {
            (*cur_org)->getSpecies()->remove_org(*cur_org); // remove from species
            delete *cur_org; // free memory
            cur_org = orgs.erase(cur_org); // remove from population
        } else
            ++cur_org;
    }
    
    // reproduce
    for (curspecies = species.begin(); curspecies != species.end(); ++curspecies) {
        if (!(*curspecies)->novel) {
            auto curspecies_id = (*curspecies)->getID();
            (*curspecies)->reproduce(generation, this, sorted_species);
            // reset the iterator to the last species
            // if there is new species added, the previous pointer becomes invalid
            for (auto lastspecies = species.begin(); lastspecies != species.end(); ++lastspecies) {
                if ((*lastspecies)->getID() == curspecies_id) {
                    curspecies = lastspecies;
                    break;
                }
            }
        }
    }
    
    // delete old organisms
    for (auto cur_org = orgs.begin(); cur_org != orgs.end(); ++cur_org) {
        (*cur_org)->getSpecies()->remove_org(*cur_org); // remove from species
        delete *cur_org; // free memory
    }
    orgs.clear();
    
    // delete empty species and set age for those survive
    // and add species' organisms to population
    for (auto curspecies = species.begin(); curspecies != species.end(); ) {
        if ((*curspecies)->orgs.empty()) {
            #ifndef NDEBUG
            std::cout << "Species " << (*curspecies)->getID() << " is empty. Deleting...\n";
            #endif
            delete (*curspecies);
            curspecies = species.erase(curspecies);
        } else {
            // set age
            if ((*curspecies)->novel)
                (*curspecies)->novel = false;
            else
                (*curspecies)->age++;
            
            // add organisms to population
            for (auto &o : (*curspecies)->orgs) {
                orgs.push_back(o);
            }

            ++curspecies;
        }
    }

    // assign new orgs' id
    netID oid = 0;
    for (auto &o : orgs) {
        o->setID(oid++);
    }
    // increment gen
    incrementGen();

    // debug: print species
    #ifndef NDEBUG
    std::cout << "New generation has " << species.size() 
        << " species: " << std::endl;
    for (const auto &s : species)
        std::cout << *s << std::endl;
    #endif
    std::cout << std::endl;

    return true;
}


/* @brief: save population to file */
void Population::archive(const std::string &ofile) {
    #ifndef NDEBUG
    std::cout << "Archiving population to file " << ofile << std::endl;
    #endif
    std::ofstream ofs(ofile);
    if (!ofs) {
        std::cerr << BnR_ERROR << "Can't open file " << ofile << std::endl;
        return;
    }
    boost::archive::text_oarchive oa(ofs);
    // register_type(oa);
    oa & *this;
    ofs.close();
}


/* @brief: construct population from file */
void Population::load(const std::string &ifile) {
    #ifndef NDEBUG
    std::cout << "Loading population from file " << ifile << std::endl;
    #endif
    std::ifstream ifs(ifile);
    if (!ifs) {
        std::cerr << BnR_ERROR << "Can't open file " << ifile << std::endl;
        return;
    }
    boost::archive::text_iarchive ia(ifs);
    ia & *this;
    ifs.close();
}
