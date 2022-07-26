/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#include "test.h"


/* @brief: load data from file to class Gate
 * and inject normalized input data to Injector
 */
int eSpinn::test_gate() {
    auto gate = load_data_from_files(
        {FILE_IN0, FILE_IN1}, {FILE_OUT0, FILE_OUT1});
    gate->set_normalizing_factors(FILE_DATA_RANGE);
    gate->init();

    auto injected_data = gate->get_injector_data_set(3);
    std::cout << "Normalized input data at timestep #3 are ";
    for (auto i = 0; i < gate->getIwidth(); ++i) {
        std::cout << injected_data[i] << " ";
    }
    std::cout << std::endl;

    auto ejected_data = gate->get_ejector_data_set(3);
    std::cout << "Normalized output data at timestep #3 are ";
    for (auto i = 0; i < gate->getOwidth(); ++i) {
        std::cout << ejected_data[i] << " ";
    }
    std::cout << std::endl;

    delete gate;
    return 0;
}


/* @brief: load and normalize data to class Injector
 */
int eSpinn::test_injector() {
    const double vals[] = {2, 3};
    const double MIN[] = {0, 1}, MAX[] = {2, 5};

    auto injector = new Injector(size_of(vals));
    injector->setNormFactors(MIN, MAX, size_of(MIN));
    injector->load_data(vals, size_of(vals));
    std::cout << *injector << std::endl;
    auto injecting_data = injector->get_data_set();
    std::cout << "Normalized data are " << injecting_data << ": ";
    for (auto i = 0; i < injector->width()+1; ++i) {
        std::cout << injecting_data[i] << " ";
    }
    std::cout << std::endl;

    Injector inj(std::move(*injector));
    std::cout << inj << std::endl;
    std::cout << "Data moved to " << inj.get_data_set() << ": ";
    for (auto i = 0; i < inj.width()+1; ++i) {
        std::cout << inj.get_data_set()[i] << " ";
    }
    std::cout << std::endl;
    auto file_inj("./asset/archive/inj.arch");
    inj.archive(file_inj);

    delete injector;

    auto new_inj = createInjector(file_inj);
    std::cout << new_inj << std::endl;
    std::cout << "Data set (" << new_inj.get_data_set() << "): ";
    for (auto i = 0; i < new_inj.width()+1; ++i) {
        std::cout << new_inj.get_data_set()[i] << " ";
    }
    std::cout << std::endl;
    return 0;
}
