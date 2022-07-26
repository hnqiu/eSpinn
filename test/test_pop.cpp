/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#include "test.h"


/* @brief: create a population of organisms
 */
int eSpinn::create_pop() {
    auto net = new HybridNetwork(netID(1), 2, 1, 1);
    auto org = Organism<HybridNetwork>(net, 0);
    auto pop = Population(&org, 2);
    pop.init();

    // delete net;
    return 0;
}


/* @brief: use boost::serialization
 * to write innovation to file and load it from file
 */
int eSpinn::serialize_innovation() {
    auto inno = new Innovation(neat::NEWCONN);
    std::string filename("asset/archive/inno.arc");
    std::ofstream ofs(filename);
    if (!ofs) {
		std::cerr << BnR_ERROR << "Can't open file " << filename << std::endl;
	    return -1;
	}
    boost::archive::text_oarchive oa(ofs);
    oa & inno;
    ofs.close();

    std::ifstream ifs(filename);
    if (!ifs) {
		std::cerr << BnR_ERROR << "Can't open file " << filename << std::endl;
	    return -1;
	}
    boost::archive::text_iarchive ia(ifs);
    Innovation *new_inno;
    ia & new_inno;
    ifs.close();
    std::cout << *new_inno << std::endl;

    delete inno;
    delete new_inno;
    return 0;
}


/* @brief: use boost::serialization
 * to write species to file and load it from file
 */
int eSpinn::serialize_species() {
    Species spec(1, 1);
    auto net = new SigmNetwork(1, 2, 1, 1);
    auto org = new Organism<SigmNetwork>(net, 1);
    spec.add_org(org);
    std::string filename("asset/archive/spec.arc");
    std::ofstream ofs(filename);
    if (!ofs) {
		std::cerr << BnR_ERROR << "Can't open file " << filename << std::endl;
	    return -1;
	}
    boost::archive::text_oarchive oa(ofs);
    oa.register_type<Organism<SigmNetwork>>();
    oa & spec;
    ofs.close();

    std::ifstream ifs(filename);
    if (!ifs) {
		std::cerr << BnR_ERROR << "Can't open file " << filename << std::endl;
	    return -1;
	}
    boost::archive::text_iarchive ia(ifs);
    ia.register_type<Organism<SigmNetwork>>();
    Species new_spec;
    ia & new_spec;
    ifs.close();
    std::cout << new_spec << std::endl;
    std::cout << *new_spec.get_champ() << std::endl;

    return 0;
}


/* @brief: use boost::serialization
 * to write population to file and load it from file
 */
int eSpinn::serialize_pop() {
    auto net = new SigmNetwork(1, 2, 1, 1);
    auto org = new Organism<SigmNetwork>(net, 1);
    auto org2 = org->duplicate(2, 1);
    org2->setFit(42);

    auto spec = new Species(1, 1);
    spec->add_org(org);
    spec->add_org(org2);

    Population pop(2);
    pop.add_species(spec);
    pop.add_org(org);
    pop.add_org(org2);

    std::string filename(FILE_POP);
    std::ofstream ofs(filename);
    if (!ofs) {
		std::cerr << BnR_ERROR << "Can't open file " << filename << std::endl;
	    return -1;
	}
    boost::archive::text_oarchive oa(ofs);
    oa.register_type<Organism<SigmNetwork>>();
    oa & pop;
    ofs.close();

    std::ifstream ifs(filename);
    if (!ifs) {
		std::cerr << BnR_ERROR << "Can't open file " << filename << std::endl;
	    return -1;
	}
    boost::archive::text_iarchive ia(ifs);
    ia.register_type<Organism<SigmNetwork>>();
    Population new_pop;
    ia & new_pop;
    ifs.close();
    std::cout << new_pop << std::endl;
    std::cout << *new_pop.get_champ_org() << std::endl;

    return 0;
}


/* @brief: use Population member functions
 * to write pop to file and load it from file
 */
int eSpinn::test_pop_archive() {
    auto net = new LinrNetwork(netID(1), 2, 1, 1);
    auto org = new Organism<LinrNetwork>(net, 1);
    auto pop = new Population(org, 10);
    pop->init();
    auto inno = new Innovation(1, 2, 1, .0, DEFAULTCONN);
    pop->innovation.push_back(inno);

    pop->archive(FILE_POP);

    auto new_pop = new Population;
    new_pop->load(FILE_POP);
    std::cout << *new_pop << std::endl;

    auto first_org = new_pop->orgs[0];
    auto org_cast = dynamic_cast<Organism<LinrNetwork>*>(first_org);
    std::cout << *org_cast->getNet() << std::endl;

    delete org;
    delete pop;
    delete new_pop;
    return 0;
}
