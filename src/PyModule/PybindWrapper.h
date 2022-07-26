/* Copyright (C) 2017-2020 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#pragma once

#include "eSpinn.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace eSpinn {
    /* @brief: Pybind implementation of NetworkBase */
    class PyNetwork : public NetworkBase {
        // @brief: inherit constructors
        using NetworkBase::NetworkBase;
    public:
        /* @brief: override get_neuron_size */
        std::vector<Neuron*>::size_type get_neuron_size() const override {
            PYBIND11_OVERLOAD_PURE(
                std::vector<Neuron*>::size_type,
                NetworkBase,
                get_neuron_size
            )
        }
        /* @brief: override get_connection_size */
        std::vector<Connection*>::size_type
            get_connection_size() const override
        {
            PYBIND11_OVERLOAD_PURE(
                std::vector<Connection*>::size_type,
                NetworkBase,
                get_connection_size
            )
        }
        /* @brief: override get_connection_weights */
        const std::vector<double> get_connection_weights() const override {
            PYBIND11_OVERLOAD_PURE(
                const std::vector<double>,
                NetworkBase,
                get_connection_weights
            )
        }
        /* @brief: override load_inputs */
        void load_inputs(const double *p, const eSpinn_size n) override {
            PYBIND11_OVERLOAD_PURE(
                void,
                NetworkBase,
                load_inputs,
                p, n
            )
        }
        /* @brief: override load_inputs */
        void load_inputs(const std::vector<double> &p) override {
            PYBIND11_OVERLOAD_PURE(
                void,
                NetworkBase,
                load_inputs,
                p
            )
        }
        /* @brief: override run */
        const std::vector<double>& run() override {
            PYBIND11_OVERLOAD_PURE(
                const std::vector<double>&,
                NetworkBase,
                run
            )
        }

    };


    /* @brief: Declare Pybind module for template class Network<Ti, Th, To> */
    template <typename T>
    void decl_Net(pybind11::module &m, const std::string &typestr) {
        std::string pyclass = typestr + "Network";
        pybind11::class_<T, NetworkBase> (m, pyclass.c_str())
            .def(pybind11::init<const netID &, const eSpinn_size &,
                const eSpinn_size &, const eSpinn_size &>(),
                pybind11::arg("nid")=0, pybind11::arg("in_num")=0,
                pybind11::arg("hid_num")=0, pybind11::arg("out_num")=0)

            .def("__repr__",
                [pyclass](const T &n) {
                    return "<eSpinn." + pyclass
                        + " #" + std::to_string(n.getID())
                        + " (i" + std::to_string(n.get_inp_size())
                        + "-h" + std::to_string(n.get_hid_size())
                        + "-o" + std::to_string(n.get_outp_size()) + ")>";
                }
            )
            .def("getID", &T::getID)
            .def("setID", &T::setID)
            .def_property("id", &T::getID, &T::setID)

            .def("load_inputs", (void(T::*)(const std::vector<double> &))
                &T::load_inputs, "load inputs from vector"
            )
            .def("run", &T::run)
        ;
    }


    /* @brief: Pybind implementation of OrganismBase */
    class PyOrganism : public OrganismBase {
        // @brief: inherit constructors
        using OrganismBase::OrganismBase;
    public:
        /* @brief: override duplicate */
        OrganismBase* duplicate(const netID &n, const eSpinn_size &g) override {
            PYBIND11_OVERLOAD_PURE(
                OrganismBase*,
                OrganismBase,
                duplicate,
                n, g
            )
        }
        /* @brief: override get_next_neuron_id */
        const neuronID get_next_neuron_id() const override {
            PYBIND11_OVERLOAD_PURE(
                const neuronID,
                OrganismBase,
                get_next_neuron_id
            )
        }
        /* @brief: override get_next_conn_id */
        const connID get_next_conn_id() const override {
            PYBIND11_OVERLOAD_PURE(
                const connID,
                OrganismBase,
                get_next_conn_id
            )
        }
        /* @brief: override get_next_conn_id */
        double calCompatDistance(OrganismBase *org) override {
            PYBIND11_OVERLOAD_PURE(
                double,
                OrganismBase,
                calCompatDistance,
                org
            )
        }
    };


    /* @brief: Declare Pybind module for template class Organism<T> */
    template <typename T>
    void decl_org(pybind11::module &m, const std::string &typestr) {
        std::string pyclass = typestr + "Organism";
        pybind11::class_<Organism<T>, OrganismBase>(m, pyclass.c_str())
            .def(pybind11::init<const netID &, const eSpinn_size &,
                const eSpinn_size &, const eSpinn_size &,
                const eSpinn_size &>())
            .def(pybind11::init<const netID &, const eSpinn_size &,
                const eSpinn_size &, const eSpinn_size &>())
            .def("__repr__",
                [pyclass](const Organism<T> &o) {
                    return "<eSpinn." + pyclass
                        + " #" + std::to_string(o.getID())
                        + " (gen " + std::to_string(o.getGen())
                        + "): fit = " + std::to_string(o.getFit()) + ">";
                }
            )
            .def("net", &Organism<T>::getNet,
                pybind11::return_value_policy::reference)
            .def("save", &Organism<T>::save)
        ;
    }
}

