/* Copyright (C) 2021 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#include "sim_hexa.h"

using namespace eSpinn;


int main(int argc, char *argv[]) {
    sim_hexa_heave();
    return sim_plasticity();
    // verify();
    // print_champ();
}


/* @brief: controller task
 * use neural networks to control the linearized hexa heave model
 * construct a population of networks to solve the problem
 * calculate mean square errors and evaluate their fitness values
 * use evolutionary algorithms to improve their performance
 */
int eSpinn::sim_hexa_heave() {
    std::cout << "Starting controller task..." << std::endl;

    // create logger and load reference signal
    auto log_pos = new PlantLogger();
    log_pos->load_ref_signal(Hexa::FILE_Z_REF);

    // construct plant model
    const double dt = 0.01;
    auto hexa = new Hexacopter(dt);

    // initialize population
    eSpinn_size gen = 1;
    eSpinn_size inp_num = 3;
    auto org = new Organism<HybridNetwork>(netID(1), inp_num, 0, 1, gen);
    // init new pop
    auto pop = new Population(org, params::pop_size);
    pop->init();
    pop->archive(Hexa::Z_POP + std::to_string(gen) + Hexa::POP_EXT);
    // or continue evolution by loading existing pop from which gen
    // gen = 1;
    // auto pop = new eSpinn::Population;
    // pop->load(Hexa::Z_POP + std::to_string(gen) + Hexa::POP_EXT);

    Injector inj(inp_num-1);
    inj.setNormFactors(hexa->pos_errRANGE[0], hexa->pos_errRANGE[1], 0); // err
    inj.setNormFactors(hexa->velRANGE[0], hexa->velRANGE[1], 1); // vel
    // inj.setNormFactors(hexa->battRange[0], hexa->battRange[1], 2); // batt_v
    inj.archive(Hexa::INJ_ARCH);

    Logger fit_logger(1);
    Logger net_outp(log_pos->length());

    for ( ; gen <= params::episode; ++gen) {
        // evaluate pop, check if solved
        if (evaluate<decltype(org)>(pop, hexa, &inj, log_pos)
            || !(gen % 1))
        {
            auto champ = dynamic_cast<decltype(org)>(pop->get_champ_org());
            std::cout << "Champion is " << *champ << std::endl;
            net_outp.clear();
            WeightWatcher w_watch(champ->getNet(), gen , log_pos->length()+5 );
            evaluate(champ, hexa, &inj, log_pos, &net_outp, &w_watch);
            net_outp.save(Hexa::FILE_THR);
            w_watch.save(Hexa::FILE_Z_WEIGHT);
            log_pos->save_act(Hexa::FILE_Z_ACT);
            pop->archive(Hexa::Z_POP + std::to_string(gen) + Hexa::POP_EXT);
            champ->archive(Hexa::Z_CHAMP_ORG);
            champ->save(Hexa::Z_CHAMP);
            if (pop->issolved()) {
                pop->archive(Hexa::Z_POP + std::to_string(params::episode)
                             + Hexa::POP_EXT);
                fit_logger.append_to_file(champ->getFit(), Hexa::FILE_Z_FIT);
                break;
            }
        }
        // evolve
        bool done = !pop->epoch(gen);
        std::cout << "Gen #" << gen 
            << ": champ fit = " << pop->get_champ_fit() << std::endl;
        fit_logger.append_to_file(pop->get_champ_fit(), Hexa::FILE_Z_FIT);
        if (done)
            break;
    }
    evaluate<decltype(org)>(pop, hexa, &inj, log_pos);
    pop->archive(Hexa::Z_POP + std::to_string(params::episode) + Hexa::POP_EXT);

    delete hexa;
    delete log_pos;
    delete org;
    delete pop;
    return 0;
}


/* @brief: evaluate population 
 * use each network to control the plant model
 * and save system outputs to log_pos
 * calculate mean square errors 
 * and assign fitness values to organisms
 * finally, check if problem is solved
 */
template <typename T>
bool eSpinn::evaluate(Population *pop, Hexacopter *hexa,
    Injector *inj, PlantLogger *log_pos)
{
    for (auto &org : pop->orgs) {
        auto org_cast = dynamic_cast<T>(org);
        evaluate(org_cast, hexa, inj, log_pos);
        if (org->setWinner(Hexa::WINNER_FIT)) {
            pop->set_solved();
        }
    }

    return pop->issolved();
}


/* @brief: evaluate organism by controlling the plant model
 * log system outputs 
 * log controller outputs when re-evaluating champ organism
 * calculate mean square error 
 * and assign fitness value to organism
 */
template <typename T>
void eSpinn::evaluate(Organism<T> *org, Hexacopter *hexa,
    Injector *inj, PlantLogger *log_pos,
    Logger *const net_outp, WeightWatcher *w_watch)
{
    #ifndef NDEBUG
    std::cout << "Evaluating Network #" << org->getID() << std::endl;
    // print out previous fitness values if evaluating existing networks
    std::cout << "prev fit is " << org->getFit() << std::endl;
    #endif

    auto net = org->getNet();
    auto inp_size = net->get_inp_size();
    const auto timesteps = log_pos->length();
    bool failed = false;
    net->backup_connection_weights();

    if (w_watch)
        w_watch->log_weights();

    hexa->reset();

    // OutputBuffer outp_channel(5);

    double raw_outp = .5, outp = .0, outp_pre = Hexa::thr_hover;
    for (auto i = 0; i < timesteps; ++i) {
        log_pos->log_act(i, hexa->getPos()); // archive actual position
        auto pos_err = log_pos->cal_err(i);
        // introduce observation noise (2cm)
        inj->load_data(0, pos_err + rand(-.02, .02) ); // load position error
        inj->load_data(1, hexa->getVel() + rand(-.02, .02) ); // load velocity
        // inj.load_data(2, hexa->getBatt()); // load battery voltage
        net->load_inputs(inj->get_data_set(), inp_size);
        raw_outp = net->run().at(0);
        outp = process( raw_outp, hexa->getApproxHover(outp_pre) );
        outp_pre = outp;
        // outp_channel.push(outp);
        // outp = outp_channel.mean();
        // std::cout << "Network output is " << outp << std::endl;

        // log connection weights to file
        if (w_watch) {
            // stop here when debug to look inside the val address of w_watch
            // to see if w_watch.reserve() works
            w_watch->log_weights();
        }
        // log network output to file
        if (net_outp)
            net_outp->push_back(outp);

        if (!hexa->run(outp)) { // position out of boundary
            failed = true;
            // set fit as 0.2 times the 
            // proportion of successful steps so far in the whole sim
            org->setFit(static_cast<double>(i)/timesteps * 0.2);
            std::cout << "org #" << org->getID() << "'s fit is " 
                << org->getFit() << std::endl;
            // or calculate fit based on the mean std error
            // and introduce a penalty to the fit fn
            // penalty is based on the steps that states are out of boundaries
            /*
            auto std_err = log_pos->cal_stde(i);
            std_err += static_cast<double>(timesteps-i)/timesteps * 0.3;
            if (std_err > 1.0)
                std::cerr << "Error: Fitness will be negative." << std::endl;
            org->calFit(std_err);
            */
            break;
        }
    }
    if (!failed) {
        auto std_err = log_pos->cal_std_err() / hexa->posMAX;
        // make sure fit is positive
        if (std_err >= 1.0)
            std_err = .8;
        org->calFit(std_err);
        std::cout << "org #" << org->getID() << "'s fit is " 
            << org->getFit() << std::endl;
    }
    // restore connection weights if network is plastic
    net->restore_connection_weights();
}


/* @brief: denormalize and shift controller output */
double eSpinn::process(const double &raw_out, double hover) {
    // double outp = (raw_out + 1.0) * 0.5 * Hexa::thr_norm_factor;
    // double outp = raw_out * Hexa::thr_norm_factor;
    double outp = raw_out * Hexa::thr_norm_factor + Hexa::thr_shift ;

    // saturation: cap output within thrRange
    if (outp < Hexa::thrRange[0]) {
        outp = Hexa::thrRange[0];
    }
    else if (outp > Hexa::thrRange[1]) {
        outp = Hexa::thrRange[1];
    }
    // hover += rand(-0.01, 0.01); // introduce observation noise
    outp += hover;
    // outp = Hexa::thr_hover;

    return outp;
}


/* @brief: plasticify network connections
 * spawn the best organism
 * and evolve the plasticity parameters
 * assign fitness value to organisms
 * and find the best params
 */
bool eSpinn::sim_plasticity() {
    std::cout << "Plasticify organisms..." << std::endl;

    // create logger and load reference signal
    auto log_pos = new PlantLogger();
    log_pos->load_ref_signal(Hexa::FILE_Z_REF);

    // construct plant model
    const double dt = 0.01;
    auto hexa = new Hexacopter(dt);

    // load previous population and get the champion
    auto pop = new eSpinn::Population;
    pop->load(Hexa::Z_POP + std::to_string(params::episode) + Hexa::POP_EXT);
    auto org = dynamic_cast<Organism<HybridNetwork>*>(pop->get_champ_org())->duplicate(1, 1);
    // set as rate Hebbian
    org->getNet()->set_connection_hebb_type(RateHebbian);
    delete pop;

    Injector inj = createInjector(Hexa::INJ_ARCH);
    std::cout << inj << std::endl;

    // /*
    // initialize population from the non-plastic champion
    eSpinn_size gen = params::episode+1;
    pop = new Population(org, params::pop_size, gen, false);
    pop->init();
    pop->set_evolving_plastic_term(true);
    // do not randomize the first org
    for (eSpinn_size i = 1; i < params::pop_size; ++i) {
        auto o = dynamic_cast<decltype(org)>(pop->orgs[i]);
        o->mutate_plastic_terms();
    }
    // */
    // or load pop and continue evolution
    /*
    eSpinn_size gen = 69;
    pop = new eSpinn::Population;
    pop->load(Hexa::Z_POP + std::to_string(gen) + Hexa::POP_EXT);
    pop->set_evolving_plastic_term(true);
    ++gen;
    */

    Logger fit_logger(1);
    fit_logger.append_newline_to_file(Hexa::FILE_Z_FIT);
    Logger net_outp(log_pos->length());

    for (; gen <= 2 * params::episode; ++gen) {
        // evaluate pop, check if solved
        if (evaluate<decltype(org)>(pop, hexa, &inj, log_pos) || !(gen%1)) {
            auto champ = dynamic_cast<decltype(org)>(pop->get_champ_org());
            std::cout << "Champion is " << *champ << std::endl;
            net_outp.clear();
            WeightWatcher w_watch(champ->getNet(), gen);
            evaluate(champ, hexa, &inj, log_pos, &net_outp, &w_watch);
            net_outp.save(Hexa::FILE_THR);
            w_watch.save(Hexa::FILE_Z_WEIGHT);
            log_pos->save_act(Hexa::FILE_Z_ACT);
            pop->archive(Hexa::Z_POP + std::to_string(gen) + Hexa::POP_EXT);
            champ->archive(Hexa::Z_CHAMP_ORG);
            champ->save(Hexa::Z_CHAMP);
            if (pop->issolved()) {
                pop->archive(Hexa::Z_POP + std::to_string(2*params::episode)
                             + Hexa::POP_EXT);
                fit_logger.append_to_file(champ->getFit(), Hexa::FILE_Z_FIT);
                break;
            }
        }
        // evolve
        bool done = !pop->epoch(gen);
        std::cout << "Gen #" << gen 
            << ": champ fit = " << pop->get_champ_fit() << std::endl;
        fit_logger.append_to_file(pop->get_champ_fit(), Hexa::FILE_Z_FIT);
        if (done)
            break;
    }
    evaluate<decltype(org)>(pop, hexa, &inj, log_pos);
    pop->archive(Hexa::Z_POP + std::to_string(2*params::episode) + Hexa::POP_EXT);

    delete hexa;
    delete log_pos;
    delete org;
    delete pop;
    return 0;
}


/* @brief: verify trained networks with a different signal
 * read population from file, and
 * evaluate them with a verification signal
 */
int eSpinn::verify() {
    std::cout << "Verifying trained networks..." << std::endl;

    // load the same reference signal for now
    auto log_pos = new PlantLogger();
    log_pos->load_ref_signal(Hexa::FILE_Z_REF);
    Logger net_outp(log_pos->length());

    // construct plant model
    const double dt = 0.01;
    auto hexa = new Hexacopter(dt);

    Population pop;
    pop.load(Hexa::Z_POP + std::to_string(params::episode) + Hexa::POP_EXT);
    std::cout << pop << std::endl;
    auto champ = pop.get_champ_org();
    auto champ_cast = dynamic_cast<Organism<HybridNetwork>*>(champ);
    std::cout << "Champ org: " << *champ_cast << std::endl;

    Injector inj = createInjector(Hexa::INJ_ARCH);
    std::cout << inj << std::endl;

    evaluate(champ_cast, hexa, &inj, log_pos, &net_outp);
    net_outp.save(Hexa::FILE_THR);
    log_pos->save_act(Hexa::FILE_Z_ACT);

    delete log_pos;
    delete hexa;

    return 0;
}


/* @brief: print out champ org
 * read population from file, and print the champ org
 */
int eSpinn::print_champ() {
    Population pop;
    pop.load(Hexa::Z_POP + std::to_string(params::episode) + Hexa::POP_EXT);
    auto champ = pop.get_champ_org();
    auto champ_cast = dynamic_cast<Organism<HybridNetwork>*>(champ);
    std::cout << "Champ org: " << *champ_cast << std::endl;

    return 0;
}
