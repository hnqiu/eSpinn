/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#include "Organism.h"
// put include here to avoid circular dependency
#include "Models/Network.h"
#include "Species.h" // avoid forward declaration error
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/export.hpp>
using namespace eSpinn;


/* @brief: print class info 
 * do the actual printing here
 */
template <typename T>
std::ostream& Organism<T>::print(std::ostream &os) const {
    OrganismBase::print(os);
    os << *net;
    return os;
}


/* @brief: set organism id
 * set both org_id & net_id
 */
template <typename T>
void Organism<T>::setID(const netID &oid) {
    org_id = oid;
    net->setID(oid);
}


/* @brief: duplicate this organism
 * with a new id and which generation
 * method use new operator, should delete the returned object
 * maually or in wrapped up classes
 */
template <typename T>
Organism<T>* Organism<T>::duplicate(const netID &n, const eSpinn_size &g) {
    auto new_net = net->duplicate(n);
    return (new Organism(new_net, g));
}


/* @brief: get the next available neuron id
 */
template <typename T>
const neuronID Organism<T>::get_next_neuron_id() const {
    neuronID n_id = 0;
    for (auto &n : net->neurons) {
        if (n_id < n->getID())
            n_id = n->getID();
    }
    return n_id + 1;
}


/* @brief: get the next available connection id
 */
template <typename T>
const connID Organism<T>::get_next_conn_id() const {
    connID c_id = 0;
    for (auto &c : net->connections) {
        if (c_id < c->getID())
            c_id = c->getID();
    }
    return c_id + 1;
}


/* @brief: calculate the compatibility distance
 */
template <typename T>
double Organism<T>::calCompatDistance(OrganismBase *orgbase) {

    auto org = dynamic_cast<decltype(this)>(orgbase);
    if (!org) {
        std::cerr << "Comparing two different organisms!" <<std::endl;
        return neat::compat_threshold + 1;
    }

    int num_disjoint = 0, num_excess = 0, num_match = 0, ddiff_total = 0;
    double wdiff_total = 0.0;
    
    auto c1 = net->connections.begin();
    auto c2 = org->net->connections.begin();
    
    // iterate the pointers until both reach ends
    // record the num of match and the weight difference of the match gene
    // record the num of disjoint and excess
    while ((c1 != net->connections.end()) || (c2 != org->net->connections.end())) {
        if (c1 == net->connections.end()) {
            ++c2;
            ++num_excess;
        } else if (c2 == org->net->connections.end()) {
            ++c1;
            ++num_excess;
        } else {
            auto c1_id = (*c1)->getID();
            auto c2_id = (*c2)->getID();
            if (c1_id == c2_id) {
                ++num_match;
                wdiff_total += std::abs((*c1)->getWeight() - (*c2)->getWeight());
                ddiff_total += std::abs(int((*c1)->getDelay() - (*c2)->getDelay()));
                
                ++c1;
                ++c2;
            } else if (c1_id < c2_id) {
                ++c1;
                ++num_disjoint;
            } else {
                ++c2;
                ++num_disjoint;
            }
        }
    }

    double ldiff = .0;
    if (net->get_outp_type() == typeid(SigmNeuron) && 
        org->net->get_outp_type() == typeid(SigmNeuron))
    {
        auto n1 = net->outp_neurons.begin();
        auto n2 = org->net->outp_neurons.begin();
        for (auto i = 0; i < net->get_outp_size(); ++i) {
            auto sn1 = dynamic_cast<SigmNeuron*>(*n1);
            auto sn2 = dynamic_cast<SigmNeuron*>(*n2);
            ldiff += std::abs(sn1->getLambda() - sn2->getLambda());
            ++n1;
            ++n2;
        }
        ldiff /= net->get_outp_size();
    }

    /* distance = coef1 * num_dis + coef2 * num_exc + coef3 * avg_weight_diff
     *          + coef4 * avg_delay_diff + coef5 * lambda_diff
     */
    return (neat::disjoint_coeff * num_disjoint
            + neat::excess_coeff * num_excess
            + neat::weightdiff_coeff * wdiff_total/num_match
            + neat::delaydiff_coeff * ddiff_total/num_match
            + neat::lambdadiff_coeff * ldiff);
}


/* @brief: randomize connection weights */
template <typename T>
void Organism<T>::randomizeWeights() {
    for (auto &c : net->connections) {
        c->setWeight(randWeight());
    }
}


/* @brief: randomize connection plastic terms */
template <typename T>
void Organism<T>::randomize_plastic_terms() {
    for (auto &c : net->connections) {
        for (eSpinn_size i = 0; i < 2; ++i) {
            c->set_plastic_term(rand_plastic_term(), i);
        }
    }
}


/* @brief: duplicate plastic rule */
template <typename T>
void Organism<T>::duplicate_plastic_rule(const Organism<T> *org) {
    net->duplicate_plastic_rule(org->getNet());
}


/* @brief: evolve network
 * evolve network topology and connection weights (and delay, TODO later)
 */
template <typename T>
void Organism<T>::evolve(neuronID &next_nid, connID &next_cid, 
    std::vector<Innovation*> &innov, const bool &evolving_plastic_terms)
{
    if (evolving_plastic_terms) {
        mutate_plastic_terms();
    }
    else {
        if (rand() < neat::add_ff_node_prob)
            add_neuron_in2out(next_cid, innov);
        else if (rand() < neat::add_node_prob)
            addNeuron(next_nid, next_cid, innov);
        else if (rand() < neat::add_conn_prob)
            addConnection(next_cid, innov);
        else {
            mutateWeights();
            mutateLambda();
        }
    }
}


/* @brief: mutate connection plastic terms */
template <typename T>
void Organism<T>::mutate_plastic_terms() {
    #ifdef ESPINN_VERBOSE
    std::cout << "Mutating connection plastic terms..." << std::endl;
    #endif
    static std::mt19937 e;
    static std::normal_distribution<double> n(0, 0.05);

    for (auto &c : net->connections) {
        for (eSpinn_size i = 0; i < 2; ++i) {
            if (rand() < neat::mutate_plasticity_prob) {
                // mutation is either creep mutation (adding a small value) or
                // uniform mutation (random reset)
                if (rand() < neat::plasticity_creep_mutate_prob) {
                    // use normal distribution
                    double rand_val = n(e);
                    c->increase_plastic_term(rand_val, i);
                    c->cap_plastic_terms();
                }
                else {
                    // random reset
                    c->set_plastic_term(rand_plastic_term(), i);
                }
            }
        }
    } // end of for connections
}


/* @brief: mutate connection weight */
template <typename T>
void Organism<T>::mutateWeights() {
    #ifdef ESPINN_VERBOSE
    std::cout << "Mutating network connection weights..." << std::endl;
    #endif
    static std::mt19937 e;
    // static std::default_random_engine e;
    static std::normal_distribution<double> n(0, 0.1);

    for (auto &c : net->connections) {
        if (rand() < neat::mutate_weight_prob) {
            // mutation is either creep mutation (adding a small value) or
            // uniform mutation (random reset)
            if (rand() < neat::creep_mutate_prob) {
                // use normal distribution
                double rand_val = n(e);
                // avoid infinite loop when rand_val == 0
                eSpinn_size loop = 0;
                while((std::abs(rand_val) < neat::mutate_weight_min) && (loop < 5)){
                    // weight change shall not be too small
                    rand_val *= rand(2, 5);
                    ++loop;
                }
                c->increaseWeight(rand_val);
                // cap weight between [-MAX_WEIGHT, MAX_WEIGHT]
                c->capWeight();
            }
            else {
                // random reset
                c->setWeight(randWeight());
            }
        }
    }
}


/* @brief: mutate sigmoid neuron lambda */
template <typename T>
void Organism<T>::mutateLambda() {
    #ifdef ESPINN_VERBOSE
    std::cout << "Mutating sigmoid neurons lambda..." << std::endl;
    #endif
    static std::mt19937 e;
    static std::normal_distribution<double> d(0, 0.2);

    for (auto &n : net->neurons) {
        if (typeid(*n) != typeid(SigmNeuron)) {
            continue;
        }
        auto sigmn = dynamic_cast<SigmNeuron*>(n);
        if (rand() < neat::mutate_lambda_prob) {
            // mutation is either creep mutation (adding a small value) or
            // uniform mutation (random reset)
            if (rand() < neat::creep_mutate_prob) {
                // use normal distribution
                double rand_val = d(e);
                // avoid infinite loop when rand_val == 0
                eSpinn_size loop = 0;
                while((std::abs(rand_val) < neat::mutate_lambda_min) && (loop < 5)){
                    // weight change shall not be too small
                    rand_val *= rand(2, 5);
                    ++loop;
                }
                sigmn->increaseLambda(rand_val);
                // cap lambda between [0, MAX_LAMBDA]
                if (sigmn->getLambda() > params::MAX_LAMBDA)
                    sigmn->setLambda(params::MAX_LAMBDA);
                else if (sigmn->getLambda() < params::MIN_LAMBDA)
                    sigmn->setLambda(params::MIN_LAMBDA);
            }
            else {
                // random reset
                sigmn->setLambda(randLambda());
            }
        }
    }
}


/* @brief: crossover
 * random pick or average configurations
 * of the shared connections which both parents have
 * before calling this method,
 * the child org will inherit exactly the same configurations of the mom
 * then in this method it will crossover with the dad
 */
template <typename T>
void Organism<T>::crossover(OrganismBase* dadbase) {

    auto dad = dynamic_cast<decltype(this)>(dadbase);
    if (!dad) {
        std::cout << "Crossover with a different-typed organism!" <<std::endl;
        return;
    }

    auto c1 = net->connections.begin();
    auto c2 = dad->net->connections.begin();
    // iterate the pointers until either one reaches the end
    while ((c1 != net->connections.end()) && (c2 != dad->net->connections.end())) {
        auto c1_id = (*c1)->getID();
        auto c2_id = (*c2)->getID();
        if (c1_id == c2_id) {
            // average weight
            (*c1)->setWeight( 0.5*( (*c1)->getWeight()+(*c2)->getWeight() ) );
            // random pick delay, hebb_type
            if (rand() < 0.5) {
                (*c1)->setDelay( (*c2)->getDelay() );
            }
            if (rand() < 0.5) {
                (*c1)->set_hebb_type( (*c2)->get_hebb_type() );
            }
            // average plastic module
            (*c1)->set_plastic_term( 
                0.5*( (*c1)->get_plastic_term(0)+(*c2)->get_plastic_term(0) ), 0);
            (*c1)->set_plastic_term( 
                0.5*( (*c1)->get_plastic_term(1)+(*c2)->get_plastic_term(1) ), 1);
            // no need to cap
            // (*c1)->cap_plastic_terms();

            // update: enable status is the same as mom
            // corresponding connection of child is more likely to be disabled
            // if either parent's connection is disabled
            // if ( !(*c1)->isEnable() || !(*c2)->isEnable() ) {
            //     if (rand() < 0.75)
            //         (*c1)->setEnable(false);
            // }

            ++c1;
            ++c2;
        } else if (c1_id < c2_id) {
            ++c1;
        } else {
            ++c2;
        }
    }

}


/* @brief: add a neuron
 * split an existing connection into two and add a new neuron in between
 * first find an existing connection to mutate
 * create a neuron & connections
 * then check if this mutation has already existed
 * assign neuron id and connection ids
 * then insert neuron into vector hid_neurons
 * the insert position is based on the sequences that hidden neurons are activated
 * finally, assign hidden neurons sequence again
 */
template <typename T>
void Organism<T>::addNeuron(neuronID &next_nid, connID &next_cid, 
    std::vector<Innovation*> &innov) 
{
    #ifdef ESPINN_VERBOSE
    std::cout << "Adding a neuron..." << std::endl;
    #endif
    // find an existing connection
    // if the connection is disabled, find another
    std::vector<Connection*>::iterator conn_mut;
    int count = 0; // avoid infinite loop
    do {
        conn_mut = net->connections.begin();
        auto shift = rand(0, net->connections.size()-1); // debug 2
        std::advance(conn_mut, shift);
        ++count;
    } while ((count < 20) && !(*conn_mut)->isEnable());
    // if all are disabled, do nothing
    if (!(*conn_mut)->isEnable())
        return;
    auto in_node = (*conn_mut)->getInode();
    auto out_node = (*conn_mut)->getOnode();

    // create a neuron & connections
    auto new_neuron = net->create_hid_neuron(0);
    Connection *new_conn1 = nullptr, *new_conn2 = nullptr;
    if (new_neuron->is_spike_neuron())
        new_conn1 = new SpikeConnection(0, in_node, new_neuron,
            (*conn_mut)->getWeight(), (*conn_mut)->getDelay());
    else
        new_conn1 = new Connection(0, in_node, new_neuron,
            (*conn_mut)->getWeight(), (*conn_mut)->getDelay());
    if (out_node->is_spike_neuron())
        new_conn2 = new SpikeConnection(0, new_neuron, out_node,
            (*conn_mut)->getWeight(), (*conn_mut)->getDelay());
    else
        new_conn2 = new Connection(0, new_neuron, out_node,
            (*conn_mut)->getWeight(), (*conn_mut)->getDelay());

    // add connections to neurons
    new_neuron->add_inConn(new_conn1);
    new_neuron->add_outConn(new_conn2);
    in_node->remove_outConn(*conn_mut);
    in_node->add_outConn(new_conn1);
    out_node->remove_inConn(*conn_mut);
    out_node->add_inConn(new_conn2);

    // check if the innovation (adding a node) has already existed in the vector innov
    bool found = false;
    for (auto &cur_innov : innov) {
        // the innovation already exists when:
        // it is a new node case,
        // it has the same innode and outnode, and
        // it is applied to split the same connection
        if ((cur_innov->i_type == neat::NEWNODE) &&
            ((*conn_mut)->getInodeID() == cur_innov->inodeid) &&
            ((*conn_mut)->getOnodeID() == cur_innov->onodeid) &&
            ((*conn_mut)->getID() == cur_innov->old_connid)) 
        {
            // assign the existing ids
            new_neuron->setID(cur_innov->new_nodeid);
            new_conn1->setID(cur_innov->new_connid);
            new_conn2->setID(cur_innov->new_connid2);
            found = true;
            break;
        }
    }
    if (!found) {
        // assign new ids & record new innovation
        new_neuron->setID(next_nid);
        new_conn1->setID(next_cid);
        new_conn2->setID(next_cid+1);
        innov.emplace_back(
            new Innovation((*conn_mut)->getInodeID(), (*conn_mut)->getOnodeID(), 
            (*conn_mut)->getID(), next_nid, next_cid, next_cid+1));
        ++next_nid;
        next_cid += 2;
    }

    // insert neuron
    // if inode seq < onode seq
    // inode is activated before onode
    // then insert the new neuron before onode
    // if not, inode is activated after onode, or they are the same node
    // then insert the new neuron after inode
    auto insert_pos = net->hid_neurons.begin();
    if ((*conn_mut)->getInodeSeq() < (*conn_mut)->getOnodeSeq()) {
        // if inode seq < onode seq
        // be careful that onode can be output neurons
        // also be careful that hid_neurons can be empty
        while (insert_pos != net->hid_neurons.end() && 
            *insert_pos != out_node) {
            ++insert_pos;
        }
    }
    else {
        // if inode seq >= onode seq
        // then inode & onode are in hidden layer
        // no need to check if inode is input neuron
        // we should be able to find the insert position
        while (insert_pos != net->hid_neurons.end() &&
            *insert_pos != in_node) {
            ++insert_pos;
        }
        if (insert_pos != net->hid_neurons.end())
            // should execute this
            ++insert_pos;
    }
    net->hid_neurons.insert(insert_pos, new_neuron);
    auto neurons_ins_pos = net->neurons.end();
    std::advance(neurons_ins_pos, -net->get_outp_size());
    net->neurons.insert(neurons_ins_pos, new_neuron);

    // remove old connection and add new connections
    delete *conn_mut;
    net->connections.erase(conn_mut);
    auto conn_ins_pos = net->connections.begin();
    while (conn_ins_pos != net->connections.end() && 
        (*conn_ins_pos)->getID() < new_conn1->getID()) {
        ++conn_ins_pos;
    }
    conn_ins_pos = net->connections.insert(conn_ins_pos, new_conn2);
    net->connections.insert(conn_ins_pos, new_conn1);

    // assign sequence to activate
    net->assign_hid_seq();
}


/* @brief: add a hidden neuron that connects input and output neurons
 * get the next neuron id and check if it's recorded in the global innovation
 * assign next connection id
 * create a hidden neuron and push it to network
 * fully connect the neuron from input layer and to output layer
 * insert newly created connections
 */
template <typename T>
void Organism<T>::add_neuron_in2out(
    connID &next_cid_global, std::vector<Innovation*> &innov) 
{
    #ifdef ESPINN_VERBOSE
    std::cout << "Adding a neuron from input to output layer..." << std::endl;
    #endif
    connID next_cid = 0;
    auto next_nid = get_next_neuron_id();

    // check if the innovation has already existed in the global innov
    bool found = false;
    for (auto &cur_innov : innov) {
        // the innovation already exists when:
        // it is a NEWNODE_IN2OUT case,
        // the node id is the same as the network's next node id
        if ((cur_innov->i_type == neat::NEWNODE_IN2OUT) &&
            (cur_innov->new_nodeid == next_nid))
        {
            // get id of the first connection that connects this node
            next_cid = cur_innov->new_connid;
            found = true;
            break;
        }
    }
    if (!found) {
        // assign new connection ids & record new innovation
        next_cid = next_cid_global;
        innov.emplace_back( new Innovation(next_nid, next_cid) );
    }

    auto inode = net->inp_neurons.begin();
    auto onode = net->outp_neurons.begin();
    int inode_size = net->get_inp_size();
    int onode_size = net->get_outp_size();

    // create a neuron
    auto new_neuron = net->create_hid_neuron(next_nid);
    net->hid_neurons.push_back(new_neuron);
    auto neurons_ins_pos = net->neurons.end();
    std::advance(neurons_ins_pos, -onode_size);
    net->neurons.insert(neurons_ins_pos, new_neuron);

    auto insert_pos = net->connections.begin();
    while (insert_pos != net->connections.end() &&
        (*insert_pos)->getID() < next_cid) {
        ++insert_pos;
    }
    // add connections
    if (new_neuron->is_spike_neuron()) {
        for (auto i = 0; i < inode_size; ++i) {
            auto new_conn = new SpikeConnection(
                next_cid+i, *(inode+i), new_neuron, .0, randDelay() );
            // get returned iterator because the original will become invalid
            // when the vector is reallocated
            insert_pos = net->connections.insert(insert_pos, new_conn);
            ++insert_pos;
            new_neuron->add_inConn(new_conn);
            (*(inode+i))->add_outConn(new_conn);
        }
    }
    else {
        for (auto i = 0; i < inode_size; ++i) {
            auto new_conn = new Connection(
                next_cid+i, *(inode+i), new_neuron, .0, randDelay() );
            insert_pos = net->connections.insert(insert_pos, new_conn);
            ++insert_pos;
            new_neuron->add_inConn(new_conn);
            (*(inode+i))->add_outConn(new_conn);
        }
    }

    next_cid += inode_size;
    if ((*onode)->is_spike_neuron()) {
        for (auto i = 0; i < onode_size; ++i) {
            auto new_conn = new SpikeConnection(
                next_cid+i, new_neuron, *(onode+i), .0, randDelay() );
            insert_pos = net->connections.insert(insert_pos, new_conn);
            ++insert_pos;
            new_neuron->add_outConn(new_conn);
            (*(onode+i))->add_inConn(new_conn);
        }
    }
    else {
        for (auto i = 0; i < onode_size; ++i) {
            auto new_conn = new Connection(
                next_cid+i, new_neuron, *(onode+i), .0, randDelay() );
            insert_pos = net->connections.insert(insert_pos, new_conn);
            ++insert_pos;
            new_neuron->add_outConn(new_conn);
            (*(onode+i))->add_inConn(new_conn);
        }
    }
    next_cid += onode_size;
    if (next_cid > next_cid_global)
        next_cid_global = next_cid;
}


/* @brief: check if the connection already exists */
template <typename T>
const bool Organism<T>::connection_exists(
    const neuronID &iid, const neuronID &oid) const 
{
    for (const auto &c : net->connections) {
        if (c->getInodeID() == iid && c->getOnodeID() == oid)
            return true;
    }
    return false;
}


/* @brief: add a connection
 * find inode & onode to construct a new connection
 * first make sure the connection has not existed yet
 * then check if the innovation is novel
 */
template <typename T>
void Organism<T>::addConnection(connID &next_cid, std::vector<Innovation*> &innov)
{
    #ifdef ESPINN_VERBOSE
    std::cout << "Adding a connection..." << std::endl;
    #endif
    std::vector<Neuron*>::iterator inode, onode;
    eSpinn_size tries = 20; // avoid infinite search

    int inode_size = net->get_inp_size();
    int hnode_size = net->get_hid_size();
    int node_size = net->get_neuron_size();
    int ishift(0), oshift(0);
    bool found(false);

    // find input node & output node to construct a connection
    // we allow connection to itself
    // however, connection should not be from output layer, or to input layer,
    // or from input layer to output layer
    // therefore, if hidden nodes size is 0, return
    if (!hnode_size)
        return;

    for (auto t = 0; t < tries; ++t) {
        inode = net->neurons.begin();
        onode = net->neurons.begin();
        // inode cannot be from output layer
        ishift = rand(0, inode_size + hnode_size - 1);
        // if inode is in input layer
        // then onode must be hidden layer
        if (ishift < inode_size) {
            oshift = rand(inode_size, inode_size + hnode_size - 1);
        }
        else {
            oshift = rand(inode_size, node_size - 1);
        }

        std::advance(inode, ishift);
        std::advance(onode, oshift);
        if (!connection_exists((*inode)->getID(), (*onode)->getID())) {
            found = true;
            break;
        }
    }

    if (!found)
        return;

    // if found, create a connection
    Connection *new_conn(nullptr);
    if ((*onode)->is_spike_neuron())
        new_conn = new SpikeConnection(next_cid, *inode, *onode,
            .0, randDelay());
    else
        new_conn = new Connection(next_cid, *inode, *onode,
            .0, randDelay());
    (*inode)->add_outConn(new_conn);
    (*onode)->add_inConn(new_conn);


    // check if the innovation (adding a conn) has already existed 
    found = false;
    for (const auto &cur_innov : innov) {
        // the innovation already exists when:
        // it is a new connection case,
        // it has the same innode and outnode
        if ((cur_innov->i_type == neat::NEWCONN) &&
            ((*inode)->getID() == cur_innov->inodeid) &&
            ((*onode)->getID() == cur_innov->onodeid)) 
        {
            // assign the existing id
            new_conn->setID(cur_innov->new_connid);
            found = true;
            break;
        }
    }
    if (!found) {
        // assign new id & record new innovation
        new_conn->setID(next_cid); // can skip this
        innov.emplace_back(
            new Innovation((*inode)->getID(), (*onode)->getID(), 
            next_cid, new_conn->getWeight(), new_conn->getType()));
        ++next_cid;
    }

    // find the position to insert new_conn
    auto insert_pos = net->connections.begin();
    while (insert_pos != net->connections.end() && 
        (*insert_pos)->getID() < new_conn->getID()) {
        ++insert_pos;
    }
    net->connections.insert(insert_pos, new_conn);
}


/* @brief: save network topology to file */
template <typename T>
void Organism<T>::save(const std::string &ofile) {
    #ifndef NDEBUG
    std::cout << "Saving organism to file " << ofile << std::endl;
    #endif
    std::ofstream ofs(ofile);
    if (!ofs) {
        std::cerr << BnR_ERROR << "Can't open file " << ofile << std::endl;
        return;
    }
    ofs << *this;
    ofs.close();
}


BOOST_CLASS_EXPORT(eSpinn::Species)
BOOST_CLASS_EXPORT(eSpinn::Organism<SigmNetwork>)
BOOST_CLASS_EXPORT(eSpinn::Organism<LinrNetwork>)
BOOST_CLASS_EXPORT(eSpinn::Organism<IzhiNetwork>)
BOOST_CLASS_EXPORT(eSpinn::Organism<LifNetwork>)
BOOST_CLASS_EXPORT(eSpinn::Organism<HybridNetwork>)
BOOST_CLASS_EXPORT(eSpinn::Organism<HybLinNetwork>)
BOOST_CLASS_EXPORT(eSpinn::SpikeConnection)

/* @brief: archive organism to file */
template <typename T>
void Organism<T>::archive(const std::string &ofile) {
    #ifndef NDEBUG
    std::cout << "Archiving organism to file " << ofile << std::endl;
    #endif
    std::ofstream ofs(ofile);
    if (!ofs) {
        std::cerr << BnR_ERROR << "Can't open file " << ofile << std::endl;
        return;
    }
    boost::archive::text_oarchive oa(ofs);
    oa & *this;
    ofs.close();
}


/* @brief: construct organism from file */
template <typename T>
void Organism<T>::load(const std::string &ifile) {
    #ifndef NDEBUG
    std::cout << "Loading organism from file " << ifile << std::endl;
    #endif
    std::ifstream ifs(ifile);
    if (!ifs) {
        std::cerr << BnR_ERROR << "Can't open file " << ifile << std::endl;
        return;
    }
    boost::archive::text_iarchive ia(ifs);
    ia & *this;
    ifs.close();
}


/* @brief: explicit instantiation */
template class eSpinn::Organism<SigmNetwork>;
template class eSpinn::Organism<LinrNetwork>;
template class eSpinn::Organism<IzhiNetwork>;
template class eSpinn::Organism<LifNetwork>;
template class eSpinn::Organism<HybridNetwork>;
template class eSpinn::Organism<HybLinNetwork>;
