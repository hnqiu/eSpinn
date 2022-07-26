/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#pragma once


#include "eSpinn_def.h"
#include "neat_def.h"
#include <iostream>
#include <boost/serialization/access.hpp>

/* @brief: Innovation 
 * Global innovation for tracking evolution history
 * initialization list: 
 */
namespace eSpinn {
    class Innovation
    {
        /* @brief: declare serialization library as friend
         * used to grant to the serialization library access to class members
         */
        friend class boost::serialization::access;

	    /* @brief: template member serialize function
         * called when using i/oarchive to load/write class members via serialization
         * make serialize() private to avoid being called directly 
         * when called, an instance of this class will first be constructed
         * since Innovation has no default constructor, we need to 
         * override function templates load_construct_data and save_construct_data
         */
        template <typename Archive>
        void serialize(Archive &ar, const unsigned int version) {
            ar & inodeid & onodeid & old_connid;
            ar & new_nodeid & new_connid & new_connid2;
            ar & new_weight & new_conn_type;
        }
        /* @brief: overloaded << 
         * print class info
         */
        friend std::ostream& operator<<(std::ostream &os, const Innovation &inno);
    public:
        /* data */
        const neat::innoType i_type;

        neuronID inodeid, onodeid; // input & output node of the connection
        connID old_connid; // the old connection, used in NEWNODE case

        neuronID new_nodeid; // newly added neuron
        connID new_connid, new_connid2; // newly added connections
        double new_weight;
        connType new_conn_type;

        /* @brief: constructor when adding a new node
         * unused parameters: new_weight, new_conn_type
         */
        Innovation(const neuronID &in_id, const neuronID &out_id, 
            const connID &old_cid, const neuronID new_nid, 
            const connID &new_cid, const connID &new_cid2
            ) :
            i_type(neat::NEWNODE), inodeid(in_id), onodeid(out_id),
            old_connid(old_cid), new_nodeid(new_nid), 
            new_connid(new_cid), new_connid2(new_cid2), 
            new_weight(.0), new_conn_type(DEFAULTCONN) { }

        /* @brief: constructor of adding a new node from input to output layer
         * added neuron is fully connected
         * @parameters:
         * new_nodeid: id of the newly created neuron
         * new_connid: id of the first connection that links the added neuron
         */
        Innovation(const neuronID new_nid, const connID &new_cid) :
            i_type(neat::NEWNODE_IN2OUT), inodeid(0), onodeid(0),
            old_connid(0), new_nodeid(new_nid), 
            new_connid(new_cid), new_connid2(0), 
            new_weight(.0), new_conn_type(DEFAULTCONN) { }

        /* @brief: constructor when adding a new connection
         * unused parameters: old_connid, new_nodeid, new_connid2
         */
        Innovation(const neuronID &in_id, const neuronID &out_id, 
            const connID &newcid, const double w, const connType t
            ) :
            i_type(neat::NEWCONN), inodeid(in_id), onodeid(out_id),
            old_connid(0), new_nodeid(0), 
            new_connid(newcid), new_connid2(0), 
            new_weight(w), new_conn_type(t) { }

        /* @brief: constructor initializing innovation type
         * cannot use synthesized default constructor
         * "= default" specification will define it as "deleted" because 
         * there is no initializer for const member i_type
         */
        Innovation(const neat::innoType &itype) : i_type(itype),
            inodeid(0), onodeid(0), old_connid(0),
            new_nodeid(0), new_connid(0), new_connid2(0),
            new_weight(.0), new_conn_type(DEFAULTCONN) 
        { }

        /* @brief: default destructor */
        ~Innovation() {
            #ifdef ESPINN_VERBOSE
            std::cout << "Destructing Innovation, type = " << i_type
            << ", from node #" << inodeid << " to #" << onodeid << std::endl;
            #endif
        }
    };
}


namespace boost {
namespace serialization {
    template <typename archive>
    inline void save_construct_data(
        archive &ar, const eSpinn::Innovation *t, const unsigned int file_version)
    {
        // save data required to construct instance
        ar << t->i_type;
    }

    template <typename archive>
    inline void load_construct_data(
        archive &ar, eSpinn::Innovation *t, const unsigned int file_version)
    {
        // retrieve data from archive required for constructor
        eSpinn::neat::innoType itype;
        ar >> itype;
        // invoke inplace constructor to initialize instance of Innovation
        ::new(t)eSpinn::Innovation(itype);
    }
}
}
