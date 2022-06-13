#ifndef DIFFERECTIFIER_HPP_
#define DIFFERECTIFIER_HPP_

#include <cstddef>

float** differectifier(float** in_signals,
                     size_t samples,
                     unsigned int nbands = 6,
                     bool inplace = false);

#endif