#pragma once

#include "interval.h"
#include "vec3.h"

using Color = Vec3;

inline double LinearToGamma(double linearComponent)
{
    if (linearComponent > 0)
        return std::sqrt(linearComponent);

    return 0;
}

void WriteColor(std::ostream& out, const Color& pixelColor)
{
    // Apply a linear to gamma transform for gamma 2
    const double r = LinearToGamma(pixelColor.x());
    const double g = LinearToGamma(pixelColor.y());
    const double b = LinearToGamma(pixelColor.z());

    // Translate the [0,1] component values to the byte range [0,255].
    static const Interval intensity(0.000, 0.999);
    const int rbyte = int(256 * intensity.Clamp(r));
    const int gbyte = int(256 * intensity.Clamp(g));
    const int bbyte = int(256 * intensity.Clamp(b));

    // Write out the pixel color components.
    out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}