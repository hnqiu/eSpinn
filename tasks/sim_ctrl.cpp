/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#include "sim_ctrl.h"
#include "eta.h"

using namespace eSpinn;


int main(int argc, char *argv[]) {
    sim_ctrl();
    return sim_plasticity();
    // return verify();
    // return plasticify();
    // return sim_rate();
}


/* @brief: controller task
 * use neural networks to control a plant model
 * construct a population of networks to solve the problem
 * calculate mean square errors and evaluate their fitness values
 * use evolutionary algorithms to improve their performance
 */
int eSpinn::sim_ctrl() {
    std::cout << "Starting controller task..." << std::endl;

    // create logger and load reference signal
    auto log_pos = new PlantLogger();
    log_pos->load_ref_signal(FILE_REF_DATA);

    // construct plant model
    const double dt = 0.02;
    auto plant = new Plant(dt);

    // initialize population
    eSpinn_size gen = 1;
    auto org = new Organism<HybLinNetwork>(netID(1), 3, 0, 1, gen);
    auto pop = new Population(org, params::pop_size);
    pop->init();
    pop->archive(FILE_POP + std::to_string(gen) + FILE_EXT);
    // auto pop = new eSpinn::Population;
    // pop->load(FILE_POP + std::to_string(gen) + FILE_EXT);

    Logger fit_logger(1);
    Logger net_outp(log_pos->length());

    for (gen = 1; gen <= params::episode; ++gen) {
        // evaluate pop, check if solved
        if (evaluate<decltype(org)>(pop, plant, log_pos) || !(gen%params::print_every)) {
            auto champ = dynamic_cast<decltype(org)>(pop->get_champ_org());
            std::cout << "Champion is " << *champ << std::endl;
            net_outp.clear();
            WeightWatcher w_watch(champ->getNet(), gen);
            evaluate(champ, plant, log_pos, &net_outp, &w_watch);
            net_outp.save(FILE_CTRL_OUT);
            w_watch.save(FILE_WEIGHT);
            log_pos->save_act(FILE_ACT_OUT);
            pop->archive(FILE_POP + std::to_string(gen) + FILE_EXT);
            champ->getNet()->save(FILE_CHAMP + FILE_EXT);
            if (pop->issolved()) {
                pop->archive(FILE_POP + std::to_string(params::episode) + FILE_EXT);
                fit_logger.append_to_file(champ->getFit(), FILE_FIT);
                break;
            }
        }
        // evolve
        bool done = !pop->epoch(gen);
        std::cout << "Gen #" << gen 
            << ": champ fit = " << pop->get_champ_fit() << std::endl;
        fit_logger.append_to_file(pop->get_champ_fit(), FILE_FIT);
        if (done)
            break;
    }
    evaluate<decltype(org)>(pop, plant, log_pos);
    pop->archive(FILE_POP + std::to_string(params::episode) + FILE_EXT);

    delete plant;
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
bool eSpinn::evaluate(Population *pop, Plant *plant, PlantLogger *log_pos) {
    for (auto &org : pop->orgs) {
        auto org_cast = dynamic_cast<T>(org);
        evaluate(org_cast, plant, log_pos);
        if (org->setWinner(winner_fit)) {
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
void eSpinn::evaluate(Organism<T> *org, Plant *plant,
    PlantLogger *log_pos, Logger *const net_outp, WeightWatcher *w_watch) 
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

    plant->reset();
    Injector inj(inp_size-1);
    inj.setNormFactors(plant->posRANGE[0], plant->posRANGE[1], 0); // pos_err
    inj.setNormFactors(plant->velRANGE[0], plant->velRANGE[1], 1); // vel

    // OutputBuffer outp_channel(5);

    double raw_outp = .5, outp = .0;
    for (auto i = 0; i < timesteps; ++i) {
        log_pos->log_act(i, plant->getPos()); // archive actual position
        auto pos_err = log_pos->cal_err(i);
        inj.load_data(0, pos_err); // load position error
        inj.load_data(1, plant->getVel()); // load velocity
        // add noise when training plastic rules (?)
        // inj.load_data(0, pos_err + rand(-0.02,0.02)); // load position error
        // inj.load_data(1, plant->getVel() + rand(-0.02,0.02)); // load vel
        // inj.load_data(2, raw_outp); // load network output from previous step
        net->load_inputs(inj.get_data_set(), inp_size);
        raw_outp = net->run().at(0);
        outp = process(raw_outp);
        // outp_channel.push(outp);
        // outp = outp_channel.mean();
        // std::cout << "Network output is " << outp << std::endl;

        // log connection weights to file
        if (w_watch)
            w_watch->log_weights();
        // log network output to file
        if (net_outp)
            net_outp->push_back(outp);

        if (!plant->run(outp)) { // position out of boundary
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
        auto std_err = log_pos->cal_std_err() / plant->posRANGE[1];
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
double eSpinn::process(const double &raw_out) {
    // denormalize
    double outp = raw_out * ctrl_norm_factor;

    // saturation: cap output within ctrlRange
    if (outp < ctrlRange[0]) {
        outp = ctrlRange[0];
    }
    else if (outp > ctrlRange[1]) {
        outp = ctrlRange[1];
    }

    // shift
    outp += ctrl_shift;
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
    log_pos->load_ref_signal(FILE_REF_DATA);

    // construct plant model
    const double dt = 0.02;
    auto plant = new Plant(dt);

    // load previous population and get the champion
    auto pop = new eSpinn::Population;
    pop->load(FILE_POP + std::to_string(params::episode) + FILE_EXT);
    auto org = dynamic_cast<Organism<HybLinNetwork>*>(pop->get_champ_org())->duplicate(1, 1);
    // set as rate Hebbian
    org->getNet()->set_connection_hebb_type(RateHebbian);
    delete pop;


    // initialize population from the champion
    eSpinn_size gen = params::episode+1;
    pop = new Population(org, params::pop_size, gen, false);
    pop->init();
    pop->set_evolving_plastic_term(true);
    // do not randomize the first org
    for (eSpinn_size i = 1; i < params::pop_size; ++i) {
        auto o = dynamic_cast<decltype(org)>(pop->orgs[i]);
        o->mutate_plastic_terms();
    }

    Logger fit_logger(1);
    fit_logger.append_newline_to_file(FILE_FIT);
    Logger net_outp(log_pos->length());

    for (gen = params::episode+1; gen <= 2 * params::episode; ++gen) {
        // evaluate pop, check if solved
        if (evaluate<decltype(org)>(pop, plant, log_pos) || !(gen%1)) {
            auto champ = dynamic_cast<decltype(org)>(pop->get_champ_org());
            std::cout << "Champion is " << *champ << std::endl;
            net_outp.clear();
            WeightWatcher w_watch(champ->getNet(), gen);
            evaluate(champ, plant, log_pos, &net_outp, &w_watch);
            net_outp.save(FILE_CTRL_OUT);
            w_watch.save(FILE_WEIGHT);
            log_pos->save_act(FILE_ACT_OUT);
            pop->archive(FILE_POP + std::to_string(gen) + FILE_EXT);
            champ->getNet()->save(FILE_CHAMP + FILE_EXT);
            if (pop->issolved()) {
                pop->archive(FILE_POP + std::to_string(2*params::episode) + FILE_EXT);
                fit_logger.append_to_file(champ->getFit(), FILE_FIT);
                break;
            }
        }
        // evolve
        bool done = !pop->epoch(gen);
        std::cout << "Gen #" << gen 
            << ": champ fit = " << pop->get_champ_fit() << std::endl;
        fit_logger.append_to_file(pop->get_champ_fit(), FILE_FIT);
        if (done)
            break;
    }
    evaluate<decltype(org)>(pop, plant, log_pos);
    pop->archive(FILE_POP + std::to_string(2*params::episode) + FILE_EXT);

    delete plant;
    delete log_pos;
    delete org;
    delete pop;
    return 0;
}


/* @brief: plasticify non-plastic networks using the evolved plastic rule
 * compare network performance when plasticity is activated
 */
bool eSpinn::plasticify() {
    std::cout << "Plasticify non-plastic networks..." << std::endl;

    // create logger and load reference signal
    auto log_pos = new PlantLogger();
    log_pos->load_ref_signal(FILE_REF_DATA);

    // construct plant model
    const double dt = 0.02;
    auto plant = new Plant(dt);

    // load population and get the plastic champion
    auto pop = new eSpinn::Population;
    pop->load(FILE_POP + std::to_string(2*params::episode) + FILE_EXT);
    auto champ = dynamic_cast<Organism<HybLinNetwork>*>(pop->get_champ_org());
    champ = champ->duplicate(1, 1);
    delete pop;

    pop = new eSpinn::Population;
    pop->load(FILE_POP + std::to_string(params::episode) + FILE_EXT);

    for (auto &o : pop->orgs) {
        auto org = dynamic_cast<decltype(champ)>(o);
        org->duplicate_plastic_rule(champ);
        evaluate(org, plant, log_pos);
    }

    delete champ;
    delete pop;
    delete log_pos;
    delete plant;
    return 0;
}


/* @brief: iteration rate of evaluation
 * test evalution speed by
 * accumulating number of iterations during 10s real time
 */
int eSpinn::sim_rate() {
    std::cout << "Starting rate test..." << std::endl;

    // create loggers and load reference signal
    auto log_pos = new PlantLogger();
    log_pos->load_ref_signal(FILE_REF_DATA);
    const auto timesteps = log_pos->length();

    Logger net_outp(timesteps);

    // construct plant model
    const double dt = 0.01;
    auto plant = new Plant(dt);

    // initialize network
    const eSpinn_size inp_size = 3;
    auto net = new HybridNetwork(netID(1), inp_size, 50, 1);
    auto org = new Organism<HybridNetwork>(net, 1);

    Injector inj(inp_size-1);
    inj.setNormFactors(-1.0, 1.0, 0);
    inj.setNormFactors(-2.0, 2.0, 1);

    long count = 0;
    using Clock = std::chrono::system_clock;
    std::chrono::seconds duration;
    auto start = Clock::now();

    do {
        net_outp.clear();
        plant->reset();

        for (auto i = 0; i < timesteps; ++i) {
            log_pos->log_act(i, plant->getPos()); // archive actual position
            auto pos_err = log_pos->cal_err(i);
            inj.load_data(0, pos_err); // load position error
            inj.load_data(1, plant->getVel()); // load velocity
            net->load_inputs(inj.get_data_set(), inp_size);
            auto outp = net->run().at(0) * 2.0 - 1.0;
            // std::cout << "Network output is " << outp << std::endl;
            net_outp.push_back(outp);
            plant->run(outp);
            ++count;
            duration = std::chrono::duration_cast<std::chrono::seconds>
                (Clock::now() - start);
            if (duration.count() >= 10.0)
                break;
        }
    } while (duration.count() < 10.0);
    std::cout << "count reaches " << count << " in " 
        << duration.count() << "s" << std::endl;

    start = Clock::now();
    auto std_err = log_pos->cal_std_err();
    std::cout << "Mean standard error is " << std_err << std::endl;
    org->calFit(std_err);
    auto dur = std::chrono::duration_cast<std::chrono::microseconds> \
        (Clock::now() - start);
    std::cout << "duration to calculate org fit is " 
        << dur.count() << "microseconds" << std::endl;

    delete log_pos;
    delete plant;
    delete org;

    return 0;
}


/* @brief: verify trained networks with a different signal
 * read population from file, and
 * evaluate them with a verification signal
 */
int eSpinn::verify() {
    std::cout << "Verifying trained networks..." << std::endl;

    // create logger and load reference signal
    auto log_pos = new PlantLogger();
    log_pos->load_ref_signal(FILE_VERIFY_DATA);
    auto log_net_outp = new Logger(log_pos->length());

    // construct plant model
    const double dt = 0.01;
    auto plant = new Plant(dt);

    auto pop = new Population;
    std::string file_pop = FILE_POP + "50" + FILE_EXT;
    pop->load(file_pop);
    std::cout << *pop << std::endl;
    auto champ = pop->get_champ_org();
    auto champ_cast = dynamic_cast<Organism<HybridNetwork>*>(champ);
    std::cout << "Champ org: " << *champ_cast << std::endl;
    evaluate(champ_cast, plant, log_pos, log_net_outp);
    log_net_outp->save(FILE_VERIFY_CTRL_OUT);
    log_pos->save_act(FILE_VERIFY_OUT);

    delete log_pos;
    delete log_net_outp;
    delete plant;
    delete pop;

    return 0;
}


/* @brief: print out champ org
 * read population from file, and print the champ org
 */
int eSpinn::print_champ() {
    auto pop = new Population;
    std::string file_pop = FILE_POP + "51" + FILE_EXT;
    pop->load(file_pop);
    auto champ = pop->get_champ_org();
    auto champ_cast = dynamic_cast<Organism<HybridNetwork>*>(champ);
    std::cout << "Champ org: " << *champ_cast << std::endl;

    delete pop;
    return 0;
}
