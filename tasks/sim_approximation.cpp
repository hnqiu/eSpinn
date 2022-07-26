/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#include "sim_approximation.h"

using namespace eSpinn;


int main(int argc, char *argv[]) {
    return sim_approximation();
}


/* @brief: approximation task - supervised learning
 * load training data from files and
 * construct a population of networks to solve the problem
 * calculate mean square errors and evaluate their fitness values
 * use evolutionary algorithms to improve their performance
 */
int eSpinn::sim_approximation() {
    std::cout << "Starting approximation task..." << std::endl;

    std::initializer_list<std::string> inp_files{FILE_IN0, FILE_IN1};
    std::initializer_list<std::string> outp_files{FILE_OUT0};
    auto gate = load_data_from_files(
        inp_files, outp_files);
    gate->set_normalizing_factors(FILE_DATA_RANGE);
    gate->init();

    auto net = new LinrNetwork(netID(1), inp_files.size()+1, 0, outp_files.size());
    auto org = new Organism<LinrNetwork>(net, 1);
    auto pop = new Population(org, 50);
    pop->init();

    std::vector<double> champ_fits;

    for (eSpinn_size gen = 1; gen <= 20; ++gen) {
        if (evaluate<decltype(org)>(pop, gate) || !(gen%params::print_every)) {
            auto champ = dynamic_cast<decltype(org)>(pop->get_champ_org());
            std::cout << "Champion is " << *champ << std::endl;
            evaluate(champ, gate);
            gate->denormalize_outp();
            gate->archive_act_output(FILE_ACT_OUT);
            pop->archive(FILE_POP);
            if (pop->issolved()) {
                champ_fits.push_back(champ->getFit());
                break;
            }
        }
        pop->epoch(gen);
        std::cout << "Champion's fitness = " << pop->get_champ_fit() << std::endl;
        champ_fits.push_back(pop->get_champ_fit());
    }
    archive(champ_fits, FILE_FIT);


    delete gate;
    delete org;
    delete pop;
    return 0;
}


/* @brief: evaluate population using training data from gate
 * calculate mean square errors 
 * and assign fitness values to organisms
 * finally, check if problem is solved
 */
template <typename T>
bool eSpinn::evaluate(Population *pop, Gate *gate) {
    for (auto &org : pop->orgs) {
        auto org_cast = dynamic_cast<T>(org);
        auto net = org_cast->getNet();
        auto inp_size = net->get_inp_size();
        for (auto i = 0; i < gate->getLength(); ++i) {
            auto inps = gate->get_injector_data_set(i);
            net->load_inputs(inps, inp_size);
            auto outp = net->run();
            gate->eject_net_outp(outp, i);
            std::cout << "Network output is " << outp << std::endl;
        }
        auto mse = gate->cal_mse();
        std::cout << "Mean square error is " << mse << std::endl;
        org->calFit(mse);
        if (org->setWinner(params::std_fit)) {
            pop->set_solved();
        }
    }

    return pop->issolved();
}


/* @brief: re-evaluate organism
 * push network outputs to gate
 */
template <typename T>
void eSpinn::evaluate(Organism<T> *org, Gate *gate) {
    auto net = org->getNet();
    auto inp_size = net->get_inp_size();
    for (auto i = 0; i < gate->getLength(); ++i) {
        auto inps = gate->get_injector_data_set(i);
        net->load_inputs(inps, inp_size);
        auto outp = net->run();
        gate->eject_net_outp(outp, i);
        std::cout << "Network output is " << outp << std::endl;
    }
}
