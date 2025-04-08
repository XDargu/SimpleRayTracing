#pragma once

#include "interval.h"
#include "vec3.h"

using Color = Vec3;

void WriteColor(std::ostream& out, const Color& pixelColor)
{
    const double r = pixelColor.x();
    const double g = pixelColor.y();
    const double b = pixelColor.z();

    // Translate the [0,1] component values to the byte range [0,255].
    static const Interval intensity(0.000, 0.999);
    const int rbyte = int(256 * intensity.Clamp(r));
    const int gbyte = int(256 * intensity.Clamp(g));
    const int bbyte = int(256 * intensity.Clamp(b));

    // Write out the pixel color components.
    out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}