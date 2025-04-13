#pragma once

#include <cmath>
#include <random>
#include <iostream>
#include <limits>
#include <memory>


// C++ Std Usings

using std::make_shared;
using std::shared_ptr;

// Constants

constexpr double infinity = std::numeric_limits<double>::infinity();
constexpr double pi = 3.1415926535897932385;

// Utility Functions

constexpr inline double DegToRad(double degrees)
{
    return degrees * pi / 180.0;
}

namespace Random
{
    inline double Double()
    {
        static std::uniform_real_distribution<double> distribution(0.0, 1.0);
        static std::mt19937 generator;
        return distribution(generator);
    }

    inline double Double(double min, double max)
    {
        // Returns a random real in [min,max).
        return min + (max - min) * Double();
    }

    inline int Int(int min, int max)
    {
        // Returns a random integer in [min,max].
        return int(Double(min, max + 1));
    }
}

// Common Headers

#include "color.h"
#include "interval.h"
#include "ray.h"
#include "vec3.h"