/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#include "test.h"

/* @brief: construct neurons
 */
int eSpinn::build_node() {
    IzhiNeuron node(1, L_HIDDEN);
    std::shared_ptr<SigmNeuron> sigm_node = std::make_shared<SigmNeuron>(2, L_HIDDEN);
    std::cout << "sigm_node lambda = " << sigm_node->getLambda() << std::endl;
    
    std::shared_ptr<Sensor> new_node = std::make_shared<Sensor>(1, L_INPUT);
    new_node->forward();

    return 0;
}


/* @brief: use boost::serialization
 * to write neurons to file and load neurons from file
 * neurons are archived with class pointer
 */
int eSpinn::serialize_node() {
    auto node1 = new Sensor(1, L_INPUT);
    auto node2 = new IzhiNeuron(2, L_HIDDEN);
    auto node3 = new SigmNeuron(3, L_OUTPUT);

    std::ofstream ofs("FILE_TEST");
    if (!ofs) {
		std::cerr << "Can't open output file " << std::endl;
	    return -1;
	}
    boost::archive::text_oarchive oa(ofs);
    // boost::archive::binary_oarchive oa(ofs);
    oa & node1 & node2 & node3;
    ofs.close();

    std::ifstream ifs("FILE_TEST");
    if (!ifs) {
		std::cerr << "Can't open file FILE_TEST" << std::endl;
	    return -1;
	}
    boost::archive::text_iarchive ia(ifs);
    // boost::archive::binary_iarchive ia(ifs);
    // Neuron *new_node;
    // cannot use base class pointer to load archive
    Sensor *new_node1;
    IzhiNeuron *new_node2;
    SigmNeuron *new_node3;
    ia & new_node1 & new_node2 & new_node3;
    ifs.close();

    auto tmp(0.5);
    new_node1->load_input(&tmp);
    new_node2->forward();
    // dynamic_cast<SigmNeuron*>(new_node)->activate();
    new_node3->activate();

    delete node1;
    delete node2;
    delete node3;
    delete new_node1;
    delete new_node2;
    delete new_node3;

    return 0;
}
