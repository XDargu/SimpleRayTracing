#pragma once

#include "hittable.h"
#include "material.h"

class Camera
{
public:

    double aspectRatio = 16.0 / 9.0; // Ratio of image width over height
    int    imageWidth = 400;         // Rendered image width in pixel count
    int    samplesPerPixel = 10;     // Count of random samples for each pixel
    int    maxDepth = 10;            // Maximum number of ray bounces into scene

    void Render(const Hittable& world)
    {
        Initialize();

        std::cout << "P3\n" << imageWidth << ' ' << imageHeight << "\n255\n";

        for (int j = 0; j < imageHeight; j++)
        {
            std::clog << "\rScanlines remaining: " << (imageHeight - j) << ' ' << std::flush;
            for (int i = 0; i < imageWidth; i++)
            {
                Color pixelColor(0, 0, 0);
                for (int sample = 0; sample < samplesPerPixel; sample++)
                {
                    const Ray r = GetRay(i, j);
                    pixelColor += RayColor(r, maxDepth, world);
                }
                WriteColor(std::cout, pixelColor * pixelSamplesScale);
            }
        }

        std::clog << "\rDone.                 \n";
    }

private:

    int    imageHeight;        // Rendered image height
    double pixelSamplesScale;  // Color scale factor for a sum of pixel samples
    Point3 center;             // Camera center
    Point3 pixel00Loc;         // Location of pixel 0, 0
    Vec3   pixelDelta_u;       // Offset to pixel to the right
    Vec3   pixelDelta_v;       // Offset to pixel below

    void Initialize()
    {
        // Calculate the image height, and ensure that it's at least 1.
        imageHeight = int(imageWidth / aspectRatio);
        imageHeight = (imageHeight < 1) ? 1 : imageHeight;

        pixelSamplesScale = 1.0 / samplesPerPixel;

        // Camera
        const double focalLength = 1.0;
        const double viewportHeight = 2.0;
        const double viewportWidth = viewportHeight * (double(imageWidth) / imageHeight);
        center = Point3(0, 0, 0);

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        const Vec3 viewport_u = Vec3(viewportWidth, 0, 0);
        const Vec3 viewport_v = Vec3(0, -viewportHeight, 0);

        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        pixelDelta_u = viewport_u / imageWidth;
        pixelDelta_v = viewport_v / imageHeight;

        // Calculate the location of the upper left pixel.
        const Vec3 viewportUpperLeft = center
            - Vec3(0, 0, focalLength) - viewport_u / 2 - viewport_v / 2;
        pixel00Loc = viewportUpperLeft + 0.5 * (pixelDelta_u + pixelDelta_v);
    }

    Ray GetRay(int i, int j) const
    {
        // Construct a camera ray originating from the origin and directed at randomly sampled
        // point around the pixel location i, j.

        const Vec3 offset = SampleSquare();
        const Vec3 pixel_sample = pixel00Loc
            + ((i + offset.x()) * pixelDelta_u)
            + ((j + offset.y()) * pixelDelta_v);

        const Vec3 ray_origin = center;
        const Vec3 ray_direction = pixel_sample - ray_origin;

        return Ray(ray_origin, ray_direction);
    }

    static Vec3 SampleSquare()
    {
        // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
        return Vec3(Random::Double() - 0.5, Random::Double() - 0.5, 0);
    }

    Color RayColor(const Ray& r, int depth, const Hittable& world) const
    {
        // If we've exceeded the ray bounce limit, no more light is gathered.
        if (depth <= 0)
            return Color(0, 0, 0);

        HitRecord rec;
        if (world.Hit(r, Interval(0.001, infinity), rec))
        {
            Ray scattered;
            Color attenuation;
            if (rec.mat->Scatter(r, rec, attenuation, scattered))
                return attenuation * RayColor(scattered, depth - 1, world);

            return Color(0, 0, 0);
        }

        const Vec3 unitDirection = UnitVector(r.direction());
        auto a = 0.5 * (unitDirection.y() + 1.0);
        return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
    }
};