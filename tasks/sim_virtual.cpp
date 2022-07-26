/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#include "sim_virtual.h"

using namespace eSpinn;


int main(int argc, char *argv[]) {
    return sim_virtual();
}


/* @brief: test EA
 * use virtual evaluate method to assign fitness to organisms
 * and test whether pop::epoch() is working 
 */
int eSpinn::sim_virtual() {
    std::cout << "Starting EA test..." << std::endl;

    // initialize population
    eSpinn_size gen = 1;
    auto net = new HybridNetwork(netID(1), 2, 3, 1);
    auto org = new Organism<HybridNetwork>(net, gen);
    auto pop = new Population(org, 50);
    pop->init();

    for (gen = 1; gen <= params::episode; ++gen) {
        virtual_evaluate(pop);
        // evolve
        pop->epoch(gen);
        std::cout << "Gen #" << gen 
            << ": champ fit = " << pop->get_champ_fit() << std::endl;
    }

    delete org;
    delete pop;
    return 0;
}


/* @brief: assign random value to organisms' fitness
 * random values will not be greater than std_fit
 * therefore, problem is never solved
 */
bool eSpinn::virtual_evaluate(Population *pop) {
    for (auto &org : pop->orgs) {
        org->setFit(rand(.0, .8));
        if (org->setWinner(params::std_fit))
            pop->set_solved();
    }
    return pop->issolved();
}
