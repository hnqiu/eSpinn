/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#include "Network.h"
using namespace eSpinn;


/* @brief: print class info 
 * do the actual printing here
 */
template <typename Ti, typename Th, typename To>
std::ostream& Network<Ti, Th, To>::print(std::ostream &os) const {
    os << std::boolalpha << "network #";
    NetworkBase::print(os);

    for (auto &n : neurons) {
        os << std::endl << (*n);
    }
    for (auto &c : connections) {
        os << std::endl << (*c);
    }
    os << std::noboolalpha;
    return os;
}


/* @brief: build a 3-layer network
 * neurons are fully connected
 */
template <typename Ti, typename Th, typename To>
Network<Ti, Th, To>::Network(const netID &nid, 
    const eSpinn_size &in_num, 
    const eSpinn_size &hid_num, 
    const eSpinn_size &out_num
    ) : 
    NetworkBase(nid),
    neurons(), inp_neurons(), hid_neurons(), outp_neurons(),
    connections(), outputs(), comment("a 3-layer network") 
{
    #ifndef NDEBUG
    std::cout << std::endl << "Building " << comment << " with " 
        << in_num << " inputs (type #" << Ti::getClassType() << "), " 
        << hid_num << " hiddens (type #" << Th::getClassType() << ") and " 
        << out_num << " outputs (type #"<< To::getClassType() << ")." << std::endl;
    #endif

    /* create neurons */
    eSpinn_size i = 0;
    for (i = 0; i < in_num; ++i) {
        auto innode = new Ti(i, L_INPUT);
        innode->setSeq(0); // this variable is used when adding hidden neurons
        neurons.emplace_back(innode);
        inp_neurons.emplace_back(innode);
    }
    for(i = 0; i < hid_num; i++) {
        auto hidnode = new Th(i+in_num, L_HIDDEN);
        hidnode->setSeq(i+1); // sequence of hidden neurons begins with 1
        neurons.emplace_back(hidnode);
        hid_neurons.emplace_back(hidnode);
    }
    for (i = 0; i < out_num; ++i) {
        auto outnode = new To(i+in_num+hid_num, L_OUTPUT);
        outnode->setSeq(-1); // set sequence of output neurons to MAX_UINT
        neurons.emplace_back(outnode);
        outp_neurons.emplace_back(outnode);
    }

    // inp_neurons may be empty!
    if (get_inp_size()) {
        inp_neurons.back()->setType(BIAS);
    }

    /* create connections */
    i = 0;
    Connection *conn;
    switch (hid_num) {
        /* no hidden neurons 
         * connect inp_neurons to outp_neurons
         */
        case 0: 
            for (auto &innode : inp_neurons) {
                for (auto &outnode : outp_neurons) {
                    /* set as SpikeConnection if the connection links to SpikeNeurons */
                    if (isSPIKING(To::getClassType()))
                        conn = new SpikeConnection(i++, innode, outnode);
                    else
                        conn = new Connection(i++, innode, outnode);
                    connections.emplace_back(conn);

                    innode->add_outConn(conn);
                    outnode->add_inConn(conn);
                }
            }
            // connections from bias to outputs should not be deleted during evolution
            for (auto &c : connections) {
                if (c->getInode()->getType() == BIAS)
                    c->setEnable(false);
            }
            break;
    
        /* otherwise
         * connect inp_neurons to hid_neurons &
         * hid_neurons to outp_neurons
         */
        default: 
            for (auto &innode : inp_neurons) {
                for (auto &hidnode : hid_neurons) {
                    /* set as SpikeConnection if the connection links to SpikeNeurons */
                    if (isSPIKING(Th::getClassType()))
                        conn = new SpikeConnection(i++, innode, hidnode);
                    else
                        conn = new Connection(i++, innode, hidnode);
                    connections.emplace_back(conn);

                    innode->add_outConn(conn);
                    hidnode->add_inConn(conn);
                }
            }
            for (auto &hidnode : hid_neurons) {
                for (auto &outnode : outp_neurons) {
                    /* set as SpikeConnection if the connection links to SpikeNeurons */
                    if (isSPIKING(To::getClassType()))
                        conn = new SpikeConnection(i++, hidnode, outnode);
                    else
                        conn = new Connection(i++, hidnode, outnode);
                    connections.emplace_back(conn);

                    hidnode->add_outConn(conn);
                    outnode->add_inConn(conn);
                }
            }
            // add connections from bias to outputs
            // if there is no input neurons, break
            if (!get_inp_size())
                break;
            const auto bias = inp_neurons.back();
            for (auto &outnode : outp_neurons) {
                // set as SpikeConnection if the connection links to SpikeNeurons
                if (isSPIKING(To::getClassType()))
                    conn = new SpikeConnection(i++, bias, outnode);
                else
                    conn = new Connection(i++, bias, outnode);
                conn->setEnable(false);
                connections.emplace_back(conn);

                bias->add_outConn(conn);
                outnode->add_inConn(conn);
            }
            break;
    }
}


/* copy constructor 
 * first create copies of neurons and connections
 * do NOT just copy the vector - they are pointers
 * copy the instances the pointers point at
 * then add connections to neurons and add nodes to connections
 */
template<typename Ti, typename Th, typename To>
Network<Ti, Th, To>::Network(const Network &net) : NetworkBase(net), 
    inp_neurons(std::vector<Ti*>()),
    hid_neurons(std::vector<Th*>()), outp_neurons(std::vector<To*>()), 
    connections(std::vector<Connection*>()), 
    outputs(), comment("copying an existing network") 
{
    #ifdef ESPINN_VERBOSE
    std::cout << std::endl << comment << std::endl;
    #endif

    /* copy neurons */
    for (auto &n : net.inp_neurons) {
        inp_neurons.push_back(n->duplicate());
    }
    for (auto &n : net.hid_neurons) {
        hid_neurons.push_back(n->duplicate());
    }
    for (auto &n : net.outp_neurons) {
        outp_neurons.push_back(n->duplicate());
    }
    // Copy constructor of Neuron will initialize its members:
    // in_conn & out_conn as empty vectors
    // which is OK: a neuron does not necessarily have connections

    /* copy pointers to all_neurons */
    neurons.insert(neurons.end(), inp_neurons.begin(), inp_neurons.end()); 
    neurons.insert(neurons.end(), hid_neurons.begin(), hid_neurons.end());
    neurons.insert(neurons.end(), outp_neurons.begin(), outp_neurons.end());

    /* copy connections */
    for (auto &c : net.connections) {
        connections.push_back(c->duplicate());
    }
    // Copy constructor of Connection will initialize its member in_node & out_node
    // However, they are pointing to the original Neurons, 
    // which belong to the constructor parameter &net,
    // so we need to reconfigure it later

    /* set connections and neurons */
    for (auto &c : connections) {
        bool ifound(false), ofound(false);
        for (auto &n : neurons) {
            if (n->getID() == c->getInodeID()) {
                c->setInode(n); // re-assign this->node to Connection in_node
                n->add_outConn(c);
                ifound = true;
            }
            if (n->getID() == c->getOnodeID()) {
                c->setOnode(n); // re-assign this->node to Connection in_node
                n->add_inConn(c);
                ofound = true;
            }
            if (ifound && ofound)
                break;
        }
        if (!ifound || !ofound) {
            std::cerr << BnR_ERROR 
            << "Failed to find connection neurons!" << std::endl; // TODO
        }
    }
}


/* @brief: destructor
 * neurons & connections are deleted here
 */
template<typename Ti, typename Th, typename To>
Network<Ti, Th, To>::~Network() {
    #ifdef ESPINN_VERBOSE
    std::cout << "Deleting network #" << getID() << std::endl;
    #endif
    for (auto &n : neurons)
        delete n;
    for (auto &c : connections)
        delete c;
    neurons.clear();
    inp_neurons.clear();
    hid_neurons.clear();
    outp_neurons.clear();
    connections.clear();
}


/* @brief: duplicate this network with a new id */
template<typename Ti, typename Th, typename To>
Network<Ti, Th, To>* Network<Ti, Th, To>::duplicate(const netID n) {
    auto new_net = new Network(*this);
    new_net->setID(n);
    return new_net;
}


/* @brief: settings after loading network using serialization
 * add connections to neurons & copy pointers to neurons
 */
template<typename Ti, typename Th, typename To>
void Network<Ti, Th, To>::after_load() {
    comment = "network loaded from file";
    #ifdef ESPINN_SERIALIZE_VERBOSE
    std::cout << comment << std::endl;
    #endif
    // add connections
    for (auto &c : connections) {
        c->in_node->add_outConn(c);
        c->out_node->add_inConn(c);
    }

    // copy pointers to neurons
    neurons.insert(neurons.end(), inp_neurons.begin(), inp_neurons.end()); 
    neurons.insert(neurons.end(), hid_neurons.begin(), hid_neurons.end());
    neurons.insert(neurons.end(), outp_neurons.begin(), outp_neurons.end());
}


/* @brief: get neuron size */
template <typename Ti, typename Th, typename To>
std::vector<Neuron*>::size_type Network<Ti, Th, To>::get_neuron_size() const {
    return neurons.size();
}

/* @brief: get input neuron size */
template <typename Ti, typename Th, typename To>
typename std::vector<Ti*>::size_type Network<Ti, Th, To>::get_inp_size() const {
    return inp_neurons.size();
}

/* @brief: get hidden neuron size */
template <typename Ti, typename Th, typename To>
typename std::vector<Th*>::size_type Network<Ti, Th, To>::get_hid_size() const {
    return hid_neurons.size();
}

/* @brief: get output neuron size */
template <typename Ti, typename Th, typename To>
typename std::vector<To*>::size_type Network<Ti, Th, To>::get_outp_size() const {
    return outp_neurons.size();
}


/* @brief: get connection size */
template <typename Ti, typename Th, typename To>
std::vector<Connection*>::size_type Network<Ti, Th, To>::get_connection_size() const {
    return connections.size();
}


/* @brief: get input neuron type id */
template <typename Ti, typename Th, typename To>
const std::type_info& Network<Ti, Th, To>::get_inp_type() const {
    return typeid(Ti);
}

/* @brief: get hidden neuron type id */
template <typename Ti, typename Th, typename To>
const std::type_info& Network<Ti, Th, To>::get_hid_type() const {
    return typeid(Th);
}

/* @brief: get output neuron type id */
template <typename Ti, typename Th, typename To>
const std::type_info& Network<Ti, Th, To>::get_outp_type() const {
    return typeid(To);
}


/* @brief: get connection weights */
template<typename Ti, typename Th, typename To>
const std::vector<double> Network<Ti, Th, To>::get_connection_weights() const {
    std::vector<double> w;
    for (auto &c : connections) {
        w.push_back(c->getWeight());
    }
    return w;
}

/* @brief: back up connection weights */
template<typename Ti, typename Th, typename To>
void Network<Ti, Th, To>::backup_connection_weights() {
    for (auto &c : connections) {
        c->backupWeight();
    }
}

/* @brief: restore connection weights */
template<typename Ti, typename Th, typename To>
void Network<Ti, Th, To>::restore_connection_weights() {
    for (auto &c : connections) {
        c->restoreWeight();
    }
}


/* @brief: get connection Hebbian type */
template<typename Ti, typename Th, typename To>
std::vector<HebbianType> Network<Ti, Th, To>::get_connection_hebb_type() {
    std::vector<HebbianType> conn_hebb;
    for (auto &c : connections) {
        conn_hebb.push_back(c->get_hebb_type());
    }
    return conn_hebb;
}


/* @brief: set connection Hebbian type */
template<typename Ti, typename Th, typename To>
void Network<Ti, Th, To>::set_connection_hebb_type(const HebbianType &h) {
    for (auto &c : connections) {
        c->set_hebb_type(h);
    }
}


/* @brief: duplicate copy plastic rule */
template <typename Ti, typename Th, typename To>
void Network<Ti, Th, To>::duplicate_plastic_rule(const Network<Ti, Th, To> *net) {
    if (!has_same_topology(net)) {
        std::cout << "Topology not met when copying plastic rule to net #"
            << getID() << std::endl;
        return;
    }

    std::cout << "Copying plastic rule to net #" << getID() << std::endl;
    for (eSpinn_size i = 0; i < get_connection_size(); ++i) {
        connections[i]->copy_plasticity_module(net->connections[i]);
        connections[i]->set_hebb_type(net->connections[i]->get_hebb_type());
    }
}


/* @brief: check if the two networks have the same topology */
template <typename Ti, typename Th, typename To>
bool Network<Ti, Th, To>::has_same_topology(const Network<Ti, Th, To> *net) {
    // return false if size not met
    // don't need to compare inp size & outp size because they won't change
    if (get_hid_size() != net->get_hid_size())
        return false;
    if (get_connection_size() != net->get_connection_size())
        return false;

    // compare conn ids
    // iterate the pointers until both reach ends
    // return false if id not met
    for (eSpinn_size i = 0; i < get_connection_size(); ++i) {
        if ( connections[i]->getID() != net->connections[i]->getID() )
            return false;
    }

    return true;
}


/* @brief: create a hidden neuron
 * based on its type 
 */
template<typename Ti, typename Th, typename To>
Th *const Network<Ti, Th, To>::create_hid_neuron(const neuronID &nid) const {
    return new Th(nid, L_HIDDEN);
}


/* @brief: assign hidden neurons sequence
 * n_seq is the order to activate 
 */
template<typename Ti, typename Th, typename To>
void Network<Ti, Th, To>::assign_hid_seq() {
    eSpinn_size s = 1;
    for (auto &hid : hid_neurons) {
        hid->setSeq(s++);
    }
}


/* @brief: load network inputs
 */
template <typename Ti, typename Th, typename To>
void Network<Ti, Th, To>::load_inputs(const double *p, const eSpinn_size n) {
    #ifdef ESPINN_VERBOSE
    std::cout << "Loading inputs (size = " << n << ")" << std::endl;
    #endif
    try {
        if (inp_neurons.size() != n) {
            throw std::logic_error("Input size not match with neuron size");
        }
        for (auto &node : inp_neurons) {
            node->load_input(p++);
        }
    }
    catch(const std::logic_error& e) {
        std::cerr << e.what() << '\n';
    }
}


/* @brief: load network inputs */
template <typename Ti, typename Th, typename To>
void Network<Ti, Th, To>::load_inputs(const std::vector<double> &p) {
    #ifdef ESPINN_VERBOSE
    std::cout << "Loading inputs (size = " << p.size() << ")" << std::endl;
    #endif
    try {
        if (inp_neurons.size() != p.size()) {
            throw std::logic_error("Input size not match with neuron size");
        }
        auto i = p.begin();
        for (auto &node : inp_neurons) {
            node->load_input(&(*i++));
        }
    }
    catch(const std::logic_error& e) {
        std::cerr << e.what() << '\n';
    }
}


// apart from the default run() function
// we also specialize 3 other run() in different network types
// in these functions we call overloaded forward() functions based on neuron types
// and then push results to network outputs
// the compiler will compile the specialized functions for 
// each template, which has been explicitly instantiated at the end of this file
// if the instantiation is not one of them, calling run() will call the default one


/* @brief: run network for ONE time slot
 * default run - SigmNetwork & LinrNetwork
 */
template <typename Ti, typename Th, typename To>
const std::vector<double>& Network<Ti, Th, To>::run() {
    #ifdef ESPINN_VERBOSE
    std::cout << "...Network Running..." << std::endl;
    #endif
    for (auto &n : inp_neurons) {
        n->forward();
    }
    for (auto &n : hid_neurons) {
        n->forward();
    }
    for (auto &n : outp_neurons) {
        n->forward();
    }

    load_outputs();
    return outputs;
}


/* @brief: run network for ONE time slot
 * specialized run() - IzhiNetwork
 * remember to declare it in header file
 */
template <>
const std::vector<double>& IzhiNetwork::run() {
    #ifdef ESPINN_VERBOSE
    std::cout << "...IzhiNet Running..." << std::endl;
    #endif
    for (auto &n : inp_neurons) {
        n->forward();
    }
    for (eSpinn_size t = 0; t < params::TIMESTEP; ++t) {
        for (auto &n : hid_neurons) {
            n->forward();
        }
        for (auto &n : outp_neurons) {
            n->forward();
        }
    }

    load_outputs();
    return outputs;
}


/* @brief: run network for ONE time slot
 * specialized run() - LifNetwork
 * use the spike status as output
 * remember to declare it in header file
 */
template <>
const std::vector<double>& LifNetwork::run() {
    #ifdef ESPINN_VERBOSE
    std::cout << "...LifNet Running..." << std::endl;
    #endif
    for (auto &n : inp_neurons) {
        n->forward();
    }
    for (auto &n : hid_neurons) {
        n->forward();
    }
    for (auto &n : outp_neurons) {
        n->forward();
    }

    outputs.clear();
    for (auto &n : outp_neurons) {
        outputs.push_back(n->getSpike());
    }
    return outputs;
}


/* @brief: run network for ONE time slot
 * specialized run() - HybridNetwork
 * remember to declare it in header file
 */
template<> 
const std::vector<double>& HybridNetwork::run() {
    #ifdef ESPINN_VERBOSE
    std::cout << "...HybridNet Running..." << std::endl;
    #endif
    for (auto &n : inp_neurons) {
        n->forward();
    }
    if (get_hid_size()) { // reduce time if no hid nodes
        for (eSpinn_size t = 0; t < params::TIMESTEP; ++t) {
            for (auto &n : hid_neurons) {
                n->forward();
            }
        }
        for (auto &n : hid_neurons) {
            n->transmit_rate();
            n->plasticify_preConn();
        }
    }

    for (auto &n : outp_neurons) {
        n->forward();
    }

    load_outputs();
    return outputs;
}


/* @brief: run network for ONE time slot
 * specialized run() - HybLinNetwork
 * remember to declare it in header file
 */
template<> 
const std::vector<double>& HybLinNetwork::run() {
    #ifdef ESPINN_VERBOSE
    std::cout << "...HybLinNet Running..." << std::endl;
    #endif
    for (auto &n : inp_neurons) {
        n->forward();
    }
    if (get_hid_size()) { // reduce time if no hid nodes
        for (eSpinn_size t = 0; t < params::TIMESTEP; ++t) {
            for (auto &n : hid_neurons) {
                n->forward();
            }
        }
        for (auto &n : hid_neurons) {
            n->transmit_rate();
            n->plasticify_preConn();
        }
    }

    for (auto &n : outp_neurons) {
        n->forward();
    }

    load_outputs();
    return outputs;
}


/* @brief: load network outputs from output neurons */
template <typename Ti, typename Th, typename To>
void Network<Ti, Th, To>::load_outputs() {
    outputs.clear();
    for (auto &n : outp_neurons) {
        outputs.push_back(n->getOut());
    }
}


/* @brief: save network topology to file */
template <typename Ti, typename Th, typename To>
void Network<Ti, Th, To>::save(const std::string &ofile) {
    //
    #ifndef NDEBUG
    std::cout << "Saving network to file " << ofile << std::endl;
    #endif
    std::ofstream ofs(ofile);
    if (!ofs) {
        std::cerr << BnR_ERROR << "Can't open file " << ofile << std::endl;
        return;
    }
    ofs << *this;
    ofs.close();
}


/* @brief: explicit instantiation */
template class eSpinn::Network<Sensor, SigmNeuron, SigmNeuron>;
template class eSpinn::Network<Sensor, SigmNeuron, LinrNeuron>;
template class eSpinn::Network<Sensor, IzhiNeuron, SigmNeuron>;
template class eSpinn::Network<Sensor, IzhiNeuron, LinrNeuron>;
template class eSpinn::Network<Sensor, IzhiNeuron, IzhiNeuron>;
template class eSpinn::Network<Sensor, LifNeuron, LifNeuron>;

