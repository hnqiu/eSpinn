/* Copyright (C) 2017-2019 Huanneng Qiu.
 * Licensed under the Apache-2.0 license. See LICENSE for details.
 */


#include "Utilities.h"

using namespace eSpinn;


/* fast inverse square root
 * return 1/sqrt(x)
 * using magic number - https://en.wikipedia.org/wiki/Fast_inverse_square_root
 */
const float eSpinn::fast_rsqrt(const float &x) {
	const float xhalf = x * 0.5f;
	union {
		float f;
		unsigned int i;
	} u;
    u.f = x; // set f to value of x
	u.i = 0x5f3759df - (u.i >> 1); // gives initial guess
	u.f *= (1.5f - (xhalf * u.f * u.f)); // Newton step, repeating increases accuracy
    u.f *= (1.5f - (xhalf * u.f * u.f));
    u.f *= (1.5f - (xhalf * u.f * u.f));
	return u.f;
}

/* fast square root
 * return sqrt(x) using magic number
 * for other methods 
 * https://www.codeproject.com/Articles/69941/Best-Square-Root-Method-Algorithm-Function-Precisi
 */
const float eSpinn::fast_sqrt(const float &x) {
    return x * fast_rsqrt(x);
}


/* return a smart pointer to a vector
 * of random real values between [min, max]
 * using normal distribution
 */
std::shared_ptr<std::vector<double>> eSpinn::rand_normal(const double &mean, const double &dev, const eSpinn_size &s) {
    static std::mt19937 e;
    std::normal_distribution<double> u(mean, dev);
    auto vals = std::make_shared<std::vector<double>>();
    for (eSpinn_size i = 0; i < s; ++i) {
        vals->push_back(u(e));
    }
    return vals;
}


/* @brief: save vector to file */
bool eSpinn::archive(const std::vector<double> &v, const std::string &ofile) {
    std::ofstream ofs(ofile);
    if (!ofs) {
        std::cerr << BnR_ERROR << "can't open file " << ofile << std::endl;
        return false;
    }
    ofs << v;
    ofs.close();
    return true;
}
