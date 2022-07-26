/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#include "test.h"


/* @brief: test utilities */
int eSpinn::test_utilities() {
    std::cout << emphasizeText("emphText") << std::endl;
    std::cout << "Random integer: " << rand(0, 9) << std::endl;
    std::cout << "Random real value: " << rand(.0, 1.0) << std::endl;
    double x;
    std::cout << "Calculating square root using fast sqrt. \nType in a float: ";
    std::cin >> x;
    std::cout << "square root of " << x << " is " << fast_sqrt(x) << std::endl;

    return 0;
}


/* @brief: test circular buffer */
int eSpinn::test_circularbuffer() {
    CircularBuffer<double> buf(5);
    for (int i = 0; i < 20; ++i) {
        buf.push(i+.1);
        std::cout << buf << std::endl;
    }
    return 0;
}
