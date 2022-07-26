/* Copyright (C) 2017-2020 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#include "PybindWrapper.h"

/* @brief: pybind module
 * this is the code to bind C++ types in Python
 */
PYBIND11_MODULE(PyeSpinn, m) {
    using namespace eSpinn;

    /* @brief: bind eSpinn types */
    pybind11::enum_<neuronType>(m, "Type")
        .value("UNDEFINED", UNDEFINED)
        .value("SENSOR", SENSOR)
        .value("SIGMOID", SIGMOID)
        .value("BIAS", BIAS)
        .value("SPIKING", SPIKING)
        .value("LIF", LIF)
        .value("IZHIKEVICH", IZHIKEVICH)
        .export_values()
    ;
    pybind11::enum_<neuronLayer>(m, "Layer")
        .value("INPUT", L_INPUT)
        .value("HIDDEN", L_HIDDEN)
        .value("OUTPUT", L_OUTPUT)
        .export_values()
    ;

    /* @brief: class Neuron binding */
    pybind11::class_<Neuron>(m, "Neuron")
        .def(pybind11::init<const neuronID &, const neuronLayer &>(),
            pybind11::arg("nid")=0, pybind11::arg("nl")=L_INPUT) // default arg
        .def("getID", &Neuron::getID)
        .def("setID", &Neuron::setID)
        .def_property_readonly("id", &Neuron::getID)
        .def("getSeq", &Neuron::getSeq)
        .def("setSeq", &Neuron::setSeq)
        .def("getLayer", &Neuron::getLayer)
        .def("setLayer", &Neuron::setLayer)
        .def_property("layer", &Neuron::getLayer, &Neuron::setLayer)
        .def("__repr__", // bind utility function
            [](const Neuron &n) {
                return "<eSpinn.Neuron #" + std::to_string(n.getID())
                    + " sq" + std::to_string(n.getSeq())
                    + " l" + std::to_string(n.getLayer()) + ">";
            }
        )
    ;

    /* @brief: class Sensor binding */
    pybind11::class_<Sensor, Neuron>(m, "Sensor")
        .def(pybind11::init<const neuronID &, const neuronLayer &>(),
            pybind11::arg("nid")=0, pybind11::arg("nl")=L_INPUT)
        .def("__repr__",
            [](const Sensor &n) {
                return "<eSpinn.Sensor #" + std::to_string(n.getID())
                    + " sq" + std::to_string(n.getSeq())
                    + " l" + std::to_string(n.getLayer()) + ">";
            }
        )
    ;

    /* @brief: class SigmNeuron binding */
    pybind11::class_<SigmNeuron, Neuron>(m, "SigmNeuron")
        .def(pybind11::init<const neuronID &, const neuronLayer &>(),
            pybind11::arg("nid")=0, pybind11::arg("nl")=L_INPUT)
        .def("__repr__",
            [](const SigmNeuron &n) {
                return "<eSpinn.SigmNeuron #" + std::to_string(n.getID())
                    + " sq" + std::to_string(n.getSeq())
                    + " l" + std::to_string(n.getLayer()) + ">";
            }
        )
    ;

    /* @brief: class SpikeNeuron binding */
    pybind11::class_<SpikeNeuron, Neuron>(m, "SpikeNeuron")
        .def(pybind11::init<const neuronID &, const neuronLayer &>(),
            pybind11::arg("nid")=0, pybind11::arg("nl")=L_INPUT)
        .def("__repr__",
            [](const SpikeNeuron &n) {
                return "<eSpinn.SpikeNeuron #" + std::to_string(n.getID())
                    + " sq" + std::to_string(n.getSeq())
                    + " l" + std::to_string(n.getLayer()) + ">";
            }
        )
    ;

    /* @brief: class IzhiNeuron binding */
    pybind11::class_<IzhiNeuron, Neuron>(m, "IzhiNeuron")
        .def(pybind11::init<const neuronID &, const neuronLayer &>(),
            pybind11::arg("nid")=0, pybind11::arg("nl")=L_INPUT)
        .def("__repr__",
            [](const IzhiNeuron &n) {
                return "<eSpinn.IzhiNeuron #" + std::to_string(n.getID())
                    + " sq" + std::to_string(n.getSeq())
                    + " l" + std::to_string(n.getLayer()) + ">";
            }
        )
    ;

    /* @brief: class LifNeuron binding */
    pybind11::class_<LifNeuron, Neuron>(m, "LifNeuron")
        .def(pybind11::init<const neuronID &, const neuronLayer &>(),
            pybind11::arg("nid")=0, pybind11::arg("nl")=L_INPUT)
        .def("__repr__",
            [](const LifNeuron &n) {
                return "<eSpinn.LifNeuron #" + std::to_string(n.getID())
                    + " sq" + std::to_string(n.getSeq())
                    + " l" + std::to_string(n.getLayer()) + ">";
            }
        )
    ;

    /* @brief: class Connection binding */
    pybind11::class_<Connection>(m, "Connection")
        .def(pybind11::init<const connID &, Neuron *const, Neuron *const,
            const double &, const synDel &>(),
            pybind11::arg("cid")=0,
            pybind11::arg("inn")=nullptr, pybind11::arg("outn")=nullptr,
            pybind11::arg("w")=.0, pybind11::arg("d")=params::NO_DELAY)
        .def("getID", &Connection::getID)
        .def("setID", &Connection::setID)
        .def_property_readonly("id", &Connection::getID)
        .def("getInodeID", &Connection::getInodeID)
        .def("getOnodeID", &Connection::getOnodeID)
        .def("getInode", &Connection::getInode)
        .def("setInode", &Connection::setInode)
        .def_property("in_node", &Connection::getInode, &Connection::setInode)
        .def("getOnode", &Connection::getOnode)
        .def("setOnode", &Connection::setOnode)
        .def_property("on_node", &Connection::getOnode, &Connection::setOnode)
        .def_property("w", &Connection::getWeight, &Connection::setWeight)
        .def("__repr__", // bind utility function
            [](const Connection &c) {
                std::string info("<eSpinn.Connection #"
                    + std::to_string( c.getID() ) );
                if ( c.getInode() && c.getOnode() ) {
                    info += " (#" + std::to_string( c.getInodeID() )
                    + " --> #" + std::to_string( c.getOnodeID() ) + ")";
                }
                return info + ">";
            }
        )
    ;

    /* @brief: class Sensor binding */
    pybind11::class_<SpikeConnection, Connection>(m, "SpikeConnection")
        .def(pybind11::init<const connID &, Neuron *const, Neuron *const,
            const double &, const synDel &>(),
            pybind11::arg("cid")=0,
            pybind11::arg("inn")=nullptr, pybind11::arg("outn")=nullptr,
            pybind11::arg("w")=.0, pybind11::arg("d")=params::NO_DELAY)
        .def("__repr__",
            [](const SpikeConnection &c) {
                std::string info("<eSpinn.SpikeConnection #"
                    + std::to_string( c.getID() ) );
                if ( c.getInode() && c.getOnode() ) {
                    info += " (#" + std::to_string( c.getInodeID() )
                    + " --> #" + std::to_string( c.getOnodeID() ) + ")";
                }
                return info + ">";
            }
        )
    ;

    /* @brief: class NetworkBase binding */
    pybind11::class_<NetworkBase, PyNetwork>(m, "NetworkBase")
        .def(pybind11::init<const netID &>())
        .def("getID", &NetworkBase::getID)
        .def("setID", &NetworkBase::setID)
        .def_property("id", &NetworkBase::getID, &NetworkBase::setID)
        .def("__repr__",
            [](const NetworkBase &n) {
                return "<eSpinn.NetworkBase #" + std::to_string(n.getID()) + ">";
            }
        )
    ;

    /* @brief: class Network binding */
    decl_Net<SigmNetwork>(m, "Sigm");
    decl_Net<LinrNetwork>(m, "Linr");
    decl_Net<IzhiNetwork>(m, "Izhi");
    decl_Net<LifNetwork>(m, "Lif");
    decl_Net<HybridNetwork>(m, "Hybrid");
    decl_Net<HybLinNetwork>(m, "HybLin");

    /* @brief: class OrganismBase binding */
    pybind11::class_<OrganismBase, PyOrganism>(m, "OrganismBase")
        .def(pybind11::init<const netID &, const eSpinn_size&>())
        .def("__repr__",
            [](const OrganismBase &o) {
                return "<eSpinn.OrganismBase #" + std::to_string(o.getID())
                    + " (gen = " + std::to_string(o.getGen()) + ")>";
            }
        )
        .def_property("fit", &OrganismBase::getFit, &OrganismBase::setFit)
        .def("save", &OrganismBase::save)
    ;

    /* @brief: class Organism binding */
    decl_org<SigmNetwork>(m, "Sigm");
    decl_org<LinrNetwork>(m, "Linr");
    decl_org<IzhiNetwork>(m, "Izhi");
    decl_org<LifNetwork>(m, "Lif");
    decl_org<HybridNetwork>(m, "Hybrid");
    decl_org<HybLinNetwork>(m, "HybLin");

    /* @brief: class Species binding */
    pybind11::class_<Species>(m, "Species")
        .def(pybind11::init<const speciesID &>())
        .def(pybind11::init<const speciesID &, const eSpinn_size &>())
        .def("__repr__",
            [](const Species &s) {
                return "<eSpinn.Species #" + std::to_string(s.getID())
                    + " (size = " + std::to_string(s.size()) + ")>";
            }
        )
    ;

    /* @brief: class Population binding */
    pybind11::class_<Population>(m, "Population")
        .def(pybind11::init<OrganismBase *const, const eSpinn_size &,
            const eSpinn_size &, const bool>(),
            pybind11::arg("o"), pybind11::arg("num"),
            pybind11::arg("g")=1, pybind11::arg("randomize")=true)
        .def(pybind11::init<const eSpinn_size &>(),
            pybind11::arg("g")=0)
        .def("__repr__",
            [](const Population &p) {
                return "<eSpinn.Population gen #" + std::to_string(p.getGen())
                    + ", size = " + std::to_string(p.size()) + ">";
            }
        )
        .def("init", &Population::init)
        .def("load", &Population::load)
        .def("archive", &Population::archive)

        .def("gen", &Population::getGen)
        .def("size", &Population::size)
        .def("issolved", &Population::issolved)
        .def("set_solved", &Population::set_solved)
        .def("epoch", &Population::epoch)

        .def("get_champ_fit", &Population::get_champ_fit)
        .def("get_champ_org", &Population::get_champ_org,
            pybind11::return_value_policy::reference)
        .def_readwrite("orgs", &Population::orgs)
        .def_readwrite("species", &Population::species)
    ;

    /* @brief: class Injector binding */
    pybind11::class_<Injector>(m, "Injector")
        .def(pybind11::init<const eSpinn_size &>())
        .def("__repr__",
            [](const Injector &inj) {
                return "<eSpinn.Injector (size = " 
                    + std::to_string(inj.width()) + ")>";
            }
        )
        .def("setNormFactors",
            (void (Injector::*)(const double &,
                                const double &,
                                const eSpinn_size &)
            )
            &Injector::setNormFactors, "set norm factors for ONE data type"
        )
        .def("load_data",
            (void (Injector::*)(const eSpinn_size &, const double &) )
            &Injector::load_data, "norm & load the nth input data"
        )
        .def("get_data_set",
            [](const Injector &inj) {
                std::vector<double> data_set(inj.get_data_set(),
                    inj.get_data_set()+inj.width()+1 ); // +bias
                return data_set;
            },
            "get current norm data"
        )
        .def("archive", &Injector::archive)
    ;

    m.def("createInjector", &createInjector, "Create injector from file");

    /* @brief: class Injector binding */
    pybind11::class_<Logger>(m, "Logger")
        .def(pybind11::init<const std::size_t &>(),
            pybind11::arg("cap")=1)
        .def("append_to_file", &Logger::append_to_file)
    ;
}
