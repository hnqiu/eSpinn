/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#include "test.h"


/* @brief: build a network
 */
int eSpinn::build_net() {
    netID nid(1);
    HybridNetwork net(nid, 2, 2, 1);
    auto node = net.outp_neurons[0];
    if (node->getType() == SIGMOID) {
        auto tmp = node->getLambda();
        std::cout << "The 1st output neuron is SIGMOID, lambda = " << tmp << std::endl;
    }
    std::cout << std::endl;

    IzhiNetwork an_net(nid, 2, 2, 1);
    auto conn = an_net.connections.back();
    if (conn->getType() == SPIKECONN) {
        std::cout << "The last connection is spiking, spike factor = " 
            << dynamic_cast<SpikeConnection*>(conn)->getSpikeFactor() << std::endl;
    }
    std::cout << std::endl;    

    return 0;
}


/* @brief: copy a network
 */
int eSpinn::copy_net() {
    Network<Sensor, IzhiNeuron, SigmNeuron> net(1, 2, 1, 1);
    int p = 5;
    auto node = net.hid_neurons.front();
    node->setSpike(p);
    std::cout << "net hidden node's spike_train[" << p << "] status is " 
        << node->getSpike(p) << "\n"
        << "accumulated spike num = " << node->getSpikeNum() << std::endl;

    auto an_net = net;
    auto an_node = an_net.hid_neurons.front();
    std::cout << "an_net hidden node's spike_train[" << p << "] status is " 
        << an_node->getSpike(p) << "\n"
        << "accumulated spike num = " << an_node->getSpikeNum() << std::endl;
    auto conn = an_net.connections.front();
    if (conn->getType() == SPIKECONN) {
        std::cout << "The first connection is spiking, spike factor = "
            << dynamic_cast<SpikeConnection*>(conn)->getSpikeFactor() << std::endl;
    }


    auto new_net = net.duplicate(3);
    std::cout << "new_net's id = " << new_net->getID() << std::endl;
    delete new_net;

    return 0;
}


/* @brief: print a network
 */
int eSpinn::print_net() {
    HybridNetwork net(1, 2, 1, 1);
    std::cout << net << std::endl;
    return 0;
}


/* @brief: run a spiking network
 * test iterations of network runs in 10s (real time)
 */
int eSpinn::run_spikeNet() {
    const double inp[3] = {0.5, 0.2, 1};
    std::cout << "size of inp[] is " << size_of(inp) << std::endl;

    auto net = new IzhiNetwork(netID(1), 3, 50, 1);

    long count = 0;
    using Clock = std::chrono::system_clock;
    std::chrono::seconds duration;
    auto start = Clock::now();

    do {
        net->load_inputs(inp, sizeof(inp)/sizeof(*inp));
        auto outp = net->run();
        std::cout << "Network outputs are " << outp << std::endl;
        ++count;
        duration = std::chrono::duration_cast<std::chrono::seconds>(Clock::now() - start);
    } while (duration.count() < 10.0);

    std::cout << "count reaches " << count << " in " << duration.count() << "s" << std::endl;


    delete net;
    return 0;
}


/* @brief: run a sigmoid network
 */
int eSpinn::run_sigmNet() {
    const double inp[2] = {0.5, 0.2};
    SigmNetwork net(netID(1), 2, 1, 1);
    net.load_inputs(inp, size_of(inp));
    auto outp = net.run();
    std::cout << "Network outputs are " << outp << std::endl;

    return 0;
}


/* @brief: run a hybrid network
 * test a network that has more complex structure
 */
int eSpinn::run_hybridNet() {
    const double inp[2] = {0.5, 0.2};
    auto net = new HybridNetwork(netID(1), 2, 2, 1);

    // add a neuron
    auto org = new Organism<HybridNetwork>(net, 0);
    std::vector<Innovation*> innov;
    eSpinn_size next_nid = 5, next_cid = 7;
    org->addNeuron(next_nid, next_cid, innov);


    for (auto i = 0; i < 10; ++i) {
        net->load_inputs(inp, size_of(inp));
        auto outp = net->run();
        std::cout << "Network outputs are " << outp << std::endl;
    }

    delete org;
    for (auto &i : innov)
        delete i;
    return 0;
}


// BOOST_CLASS_EXPORT(eSpinn::SpikeConnection)
// BOOST_CLASS_EXPORT(eSpinn::Connection)
// BOOST_CLASS_EXPORT(eSpinn::Sensor)
// BOOST_CLASS_EXPORT(eSpinn::IzhiNeuron)
// BOOST_CLASS_EXPORT(eSpinn::SigmNeuron)

/* @brief: use boost::serialization
 * to write networks to file and load networks from file
 * networks are archived with class pointer
 */
int eSpinn::serialize_net() {
    auto net = new HybridNetwork(netID(3), 2, 1, 1);
    std::ofstream ofs("FILE_TEST");
    if (!ofs) {
		std::cerr << "Can't open output file " << std::endl;
	    return -1;
	}
    boost::archive::text_oarchive oa(ofs);
    oa.register_type<Sensor>();
    oa.register_type<IzhiNeuron>();
    oa.register_type<SigmNeuron>();
    oa.register_type<Connection>();
    oa.register_type<SpikeConnection>();
    oa & net;
    ofs.close();

    std::ifstream ifs("FILE_TEST");
    if (!ifs) {
		std::cerr << "Can't open file FILE_TEST" << std::endl;
	    return -1;
	}
    boost::archive::text_iarchive ia(ifs);
    ia.register_type<Sensor>();
    ia.register_type<IzhiNeuron>();
    ia.register_type<SigmNeuron>();
    ia.register_type<Connection>();
    ia.register_type<SpikeConnection>();
    HybridNetwork *new_net;
    ia & new_net;
    ifs.close();
    std::cout << "Net #" << new_net->getID() << " loaded!" << std::endl;
    if (new_net->inp_neurons.front()->getType() == SENSOR)
        std::cout << "The first input neuron is SENSOR" << std::endl;

    delete net;
    delete new_net;
    return 0;
}
