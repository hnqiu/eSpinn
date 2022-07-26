/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#include "test.h"


/* @brief: create a list of organisms and sort them by fitness
 */
int eSpinn::sort_org() {
    Network<Sensor, IzhiNeuron, IzhiNeuron> *net;
    Organism<IzhiNetwork> *org;
    std::vector<decltype(org)> orgs;
    // std::vector<Organism<IzhiNetwork> *> orgs;

    for (int i = 0; i < 3; ++i) {
        net = new Network<Sensor, IzhiNeuron, IzhiNeuron>(netID(i), 2, 1, 1);
        org = new Organism<IzhiNetwork>(net, 1);
        org->setFit(i+.5);
        orgs.push_back(org);
    }
    std::cout << "A vector of Organism has been created! \nIt has "
        << orgs.size() << " organisms. \n";
    int i = 0;
    for (auto &o : orgs) {
        std::cout << "#" << i++ << "'s id is " << o->getID()
            << ", fit is " << o->getFit() << std::endl;
    }

    // std::stable_sort(orgs.begin(), orgs.end()); // default sort is less_fit()
    std::stable_sort(orgs.begin(), orgs.end(), static_cast<bool (*)(const OrganismBase*, const OrganismBase*)>(greater_fit));
    // use cast to specify the overloaded instance of greater_fit()
    // we can also use lambda as the predicate
    // std::stable_sort(orgs.begin(), orgs.end(), [](const Organism<IzhiNetwork> &o1, const Organism &o2) {return o1 > o2;});
    std::cout << "\norgs sorted!\n";
    i = 0;
    for (auto &o : orgs) {
        std::cout << "#" << i++ << "'s id is " << o->getID()
            << ", fit is " << o->getFit() << std::endl;
    }

    for (auto &o : orgs) {
        delete o;
    }
    return 0;
}


int eSpinn::copy_org() {
    // net is added to org
    // need not to be deleted manually because org destructor will do it
    auto net = new SigmNetwork(netID(1), 2, 2, 1);
    auto org = Organism<SigmNetwork>(net, 1);

    // use copy constructor, an_org is identical to org
    auto an_org = org;

    // call Net::duplicate and use default constructor
    // specify org id & which generaton
    // duplicate use new operator, should delete it manually
    auto new_org = org.duplicate(2, 2); 

    org.setFit(3.0);
    std::cout << std::endl << org << std::endl;
    std::cout << std::endl << an_org << std::endl;
    std::cout << std::endl << *new_org << std::endl;
    
    delete new_org;

    return 0;
}


/* @brief: use boost::serialization
 * to write organisms to file and load organisms from file
 * organisms are archived with class pointer
 */
int eSpinn::serialize_org() {
    auto net = new HybridNetwork(netID(3), 2, 1, 1);
    auto org = new Organism<HybridNetwork>(net, 5);
    org->setFit(3.5);
    org->archive("./asset/archive/org.arch");
    /*
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
    oa & org;
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
    Organism<HybridNetwork> *new_org;
    ia & new_org;
    ifs.close();
    */
    auto new_org = new Organism<HybridNetwork>();
    new_org->load("./asset/archive/org.arch");
    std::cout << std::endl << "Org #" << new_org->getID() << " loaded!" << std::endl;
    std::cout << "gen = " << new_org->getGen() << "; fit = " << new_org->getFit() << std::endl;

    delete org;
    delete new_org;

    return 0;
}


int eSpinn::test_org() {
    auto net = new HybridNetwork(netID(1), 2, 1, 1);
    auto org = new Organism<HybridNetwork>(net, 0);

    OrganismBase * new_org = org->duplicate(2, 1);
    auto new_org_cast = dynamic_cast<Organism<HybridNetwork>*>(new_org);
    new_org_cast->mutateWeights();
    std::cout << *new_org_cast << std::endl;
    // use new_org_cast to check the net address when debugging

    auto d = org->calCompatDistance(new_org);
    std::cout << "Distance between the two organisms is " << d << std::endl;

    delete org;
    delete new_org;
    return 0;
}
