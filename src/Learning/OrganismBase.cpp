/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#include "OrganismBase.h"
using namespace eSpinn;


/* @brief: print class info 
 * do the actual printing here
 */
std::ostream& OrganismBase::print(std::ostream &os) const {
    os << "org #" << getID() << " (gen " << getGen() << "): fit = " << getFit()
        << " winner = " << isWinner() << std::endl;
    return os;
}


/* @brief: get organism id */
const netID OrganismBase::getID() const {
    return org_id;
}


/* @brief: set organism id */
void OrganismBase::setID(const netID &oid) {
    org_id = oid;
}


/* @brief: get which generation organism belong to */
const eSpinn_size OrganismBase::getGen() const { return gen; }

/* @brief: get which generation organism belong to */
void OrganismBase::setGen(const eSpinn_size &g) { gen = g; }


/* @brief: get organism fit */
double OrganismBase::getFit() const { return fitness; }

/* @brief: set organism fit */
void OrganismBase::setFit(const double &f) {
    fitness = f;
}


/* @brief: calculate fitness */
void OrganismBase::calFit(const double &std_err) {
    fitness = 1.0 - std_err;
    // fitness = 1.0 / (1.0 + fast_sqrt(mse));
}


/* @brief: get organism original fit */
double OrganismBase::getOrigFit() const { return orig_fit; }


/* @brief: check organism winner status */
const bool OrganismBase::isWinner() const { return winner; }

/* @brief: set organism as winner */
void OrganismBase::setWinner() { winner = true; }


/* @brief: set organism winner status
 * set organism as winner if its fit >= std_fit
 */
const bool OrganismBase::setWinner(const double &std_fit) {
    if (fitness >= std_fit)
        winner = true;
    else
        winner = false;

    return winner;
}


/* @brief: check organism eliminate status */
bool OrganismBase::isDying() const { return eliminate; }

/* @brief: set organism as dead - should be eliminated */
void OrganismBase::setDead() { eliminate = true; }


/* @brief: get the number of expected offspring */
double OrganismBase::getExpectedOffspring() const {
    return expected_offspring;
}

/* @brief: set the number of expected offspring */
void OrganismBase::setExpectedOffspring(const double &exp_num) {
    expected_offspring = exp_num;
}


/* @brief: get organism species */
Species *const OrganismBase::getSpecies() const { return species; }

/* @brief: set organism species */
void OrganismBase::setSpecies(Species *s) { species = s; }
