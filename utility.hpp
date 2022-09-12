#ifndef UTILITY_H
#define UTILITY_H

#include <cmath>
#include <limits>
#include <memory>
#include <cstdlib>
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <hittable.hpp>
#include "OBJ_Loader.hpp"
#include <cassert>
// Usings

using std::shared_ptr;
using std::make_shared;
using std::sqrt;

typedef unsigned int unit32_t;
// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;
extern const float EPSILON = 0.0000001;
// Utility Functions

inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

// Random Number Generator
inline double random_double() {
    return rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
    return min + (max-min)*random_double();
}

inline double clamp(double x, double min, double max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

// Common Headers

#include "ray.hpp"
#include <glm/glm.hpp>

#endif