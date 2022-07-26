/* Copyright (C) 2017-2020 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#include "pole_balancing.h"

using namespace eSpinn;


int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << BnR_ERROR << "Prgram requires one param: markov?\n";
        return -1;
    }

    bool markov = true;
    if (std::string(argv[1]) == "markov") {
        markov = true;
    }
    else if (std::string(argv[1]) == "nonmarkov") {
        markov = false;
    }
    else {
        return -1;
    }

    return pole_balancing(markov);
}


/* @brief: pole balancing task
 * pole balancing with/without velocity information
 * use neural networks to solve the pole balancing problem
 */
int eSpinn::pole_balancing(const bool &markov) {
    std::cout << "Starting pole balancing task..." << std::endl;

    // construct cart pole system & logger
    const double dt = 0.01;
    CartPole mdl(dt);
    CartPoleLogger mdl_log;

    // initialize population
    eSpinn_size gen = 1;
    eSpinn_size inp_num = 5;
    if (!markov) {
        inp_num = 3;
    }
    // use lif net if markov
    // auto org = new Organism<LifNetwork>(netID(1), inp_num, 0, 1, gen);
    auto org = new Organism<HybridNetwork>(netID(1), inp_num, 0, 1, gen);
    auto pop = new Population(org, params::pop_size);
    pop->init();
    pop->archive(Pole::CARTPOLE + std::to_string(gen) + Pole::POP_EXT);
    // auto pop = new eSpinn::Population;
    // pop->load(Pole::CARTPOLE + std::to_string(gen) + Pole::POP_EXT);

    // initialize inject encoder
    Injector inj(inp_num-1);
    inj.setNormFactors(mdl.xRANGE[0],     mdl.xRANGE[1], 0);
    inj.setNormFactors(mdl.thetaRANGE[0], mdl.thetaRANGE[1], 1);
    if (markov) {
        inj.setNormFactors(mdl.xdotRANGE[0],     mdl.xdotRANGE[1], 2);
        inj.setNormFactors(mdl.thetadotRANGE[0], mdl.thetadotRANGE[1], 3);
    }

    // log fit & forces
    Logger fit_log(1);
    Logger force_log;
    Logger gen_rec(1);

    for (gen = 1; gen <= params::episode; ++gen) {
        // evaluate pop, check if solved
        if (evaluate<decltype(org)>(pop, &inj, &mdl, markov)
            || !(gen % params::print_every))
        {
            auto champ = dynamic_cast<decltype(org)>(pop->get_champ_org());
            std::cout << "Champion is " << *champ << std::endl;
            mdl_log.clear();
            force_log.clear();
            evaluate(champ, &inj, &mdl, markov, &mdl_log, &force_log);
            // if (!evaluate(champ, &inj, &mdl, markov, &mdl_log, &force_log)) {
            //     pop->reset_solved();
            // }

            // save to files
            force_log.save(Pole::FILE_FORCE);
            mdl_log.archive(Pole::FILE_MDL_STATES);
            pop->archive(Pole::CARTPOLE + std::to_string(gen) + Pole::POP_EXT);
            champ->archive(Pole::CHAMP_ORG);
            champ->save(Pole::CHAMP);
            if (pop->issolved()) {
                pop->archive(Pole::CARTPOLE + std::to_string(params::episode)
                             + Pole::POP_EXT);
                fit_log.append_to_file(champ->getFit(), FILE_FIT);
                break;
            }
        }
        // evolve
        bool done = !pop->epoch(gen);
        std::cout << "Gen #" << gen 
            << ": champ fit = " << pop->get_champ_fit() << std::endl;
        fit_log.append_to_file(pop->get_champ_fit(), FILE_FIT);
        if (done)
            break;
    }
    // evaluate<decltype(org)>(pop, &inj, &mdl, markov);
    pop->archive(Pole::CARTPOLE + std::to_string(params::episode)
                 + Pole::POP_EXT);
    gen_rec.append_to_file(gen, FILE_GEN_REC);

    delete org;
    delete pop;
    return 0;
}


/* @brief: evaluate population 
 * return true if problem solved
 * evaluate each organism one by one
 * and check if problem is solved
 */
template <typename T>
bool eSpinn::evaluate(Population *pop, Injector *inj, CartPole *mdl,
    const bool &markov)
{
    for (auto &org : pop->orgs) {
        auto org_cast = dynamic_cast<T>(org);
        if ( evaluate(org_cast, inj, mdl, markov) ) {
            pop->set_solved();
        }
    }

    return pop->issolved();
}


/* @brief: evaluate organism
 * return true if org is successful
 * load inputs using Injector
 * get network output to control the plant model
 * and assign fitness value to organism
 * log system states & controller outputs when re-evaluating champ organism
 */
template <typename T>
bool eSpinn::evaluate(Organism<T> *org, Injector *inj, CartPole *mdl,
    const bool &markov, CartPoleLogger *mdl_log, Logger *force_log)
{
    #ifndef NDEBUG
    std::cout << "Evaluating Network #" << org->getID() << std::endl;
    #endif

    bool failed = false;
    eSpinn_size steps = 0;
    mdl->reset();
    auto net = org->getNet();
    auto inp_size = net->get_inp_size();

    double net_outp = .0, force = .0;
    while (steps++ < Pole::MAX_STEP) {
        auto states = mdl->get_states();
        // load states to inj
        for (auto i = 0; i < inp_size-1; ++i) {
            inj->load_data(i, states[i]);
        }
        net->load_inputs(inj->get_data_set(), inp_size);
        net_outp = net->run().at(0);
        force = process(net_outp, markov);

        // log system states
        if (mdl_log) {
            mdl_log->log_states(*mdl);
        }
        // log network output to file
        if (force_log)
            force_log->push_back(force);

        if (!mdl->update(force)) { // state out of boundary
            failed = true;
            break;
        }
    }
    org->setFit(steps);
    org->setWinner(Pole::MAX_STEP);
    std::cout << "org #" << org->getID() << "'s fit is " 
        << org->getFit() << std::endl;
    return !failed;
}


/* @brief: convert network output to force to drive the cart */
double eSpinn::process(const double &net_out, const bool &markov) {
    // bang-bang control
    if (markov) {
        return (net_out == .0) ? -Pole::FORCE_MAG : Pole::FORCE_MAG;
        // return (net_out < .0) ? -Pole::FORCE_MAG : Pole::FORCE_MAG;
    }
    // or denormalize and shift net output
    else {
        // return net_out * Pole::FORCE_MAG;
        return (2*net_out - 1.0) * Pole::FORCE_MAG; // HybridNet
    }
}
