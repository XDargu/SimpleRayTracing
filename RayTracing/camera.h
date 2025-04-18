#pragma once

#include "hittable.h"
#include "material.h"
#include <thread>

class Camera
{
public:

    double aspectRatio = 16.0 / 9.0; // Ratio of image width over height
    int    imageWidth = 400;         // Rendered image width in pixel count
    int    samplesPerPixel = 10;     // Count of random samples for each pixel
    int    maxDepth = 10;            // Maximum number of ray bounces into scene
    Color  background;               // Scene background color

    double vfov = 90;                    // Vertical view angle (field of view)
    Point3 lookFrom = Point3(0, 0, 0);   // Point camera is looking from
    Point3 lookAt = Point3(0, 0, -1);    // Point camera is looking at
    Vec3   up = Vec3(0, 1, 0);           // Camera-relative "up" direction

    double defocusAngle = 0;  // Variation angle of rays through each pixel
    double focusDist = 10;    // Distance from camera lookfrom point to plane of perfect focus

    void Render(const Hittable& world)
    {
        const auto processorCount = std::thread::hardware_concurrency();
        auto start = clock();

        Initialize();

        std::cout << "P3\n" << imageWidth << ' ' << imageHeight << "\n255\n";

        std::vector<Color> pixels(imageWidth * imageHeight);
        constexpr bool useAsync = true;

        if (!useAsync)
        {
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

                    pixels[j * imageHeight + i] = pixelColor * pixelSamplesScale;
                }
            }
        }
        else
        {
            std::atomic<int> processed = 0;
            std::vector<std::thread> threads(processorCount);

            // Split the amount of scanlines
            const int linesPerProcessor = imageHeight / processorCount;

            for (int p = 0; p < processorCount; ++p)
            {
                const int start = linesPerProcessor * p;
                const int end = p == processorCount - 1 ? imageHeight : linesPerProcessor * (p + 1);

                threads[p] = std::thread([this, &pixels, &world, &processed, start, end]
                {
                    for (int j = start; j < end; j++)
                    {
                        std::clog << "\rScanlines remaining: " << (imageHeight - processed) << "          " << std::flush;
                        for (int i = 0; i < imageWidth; i++)
                        {
                            Color pixelColor(0, 0, 0);

                            for (int sample = 0; sample < samplesPerPixel; sample++)
                            {
                                const Ray r = GetRay(i, j);
                                pixelColor += RayColor(r, maxDepth, world);
                            }

                            pixels[j * imageHeight + i] = pixelColor * pixelSamplesScale;
                        }
                        ++processed;
                    }
                });
            }

            for (unsigned int i = 0; i < threads.size(); ++i)
            {
                threads.at(i).join();
            }
        }

        for (const Color& color : pixels)
        {
            WriteColor(std::cout, color);
        }

        auto elapsed = double(clock() - start) / CLOCKS_PER_SEC;
        std::clog << "\rDone. Total time: " << elapsed << "s                  \n";
    }

private:

    int    imageHeight;        // Rendered image height
    double pixelSamplesScale;  // Color scale factor for a sum of pixel samples
    Point3 center;             // Camera center
    Point3 pixel00Loc;         // Location of pixel 0, 0
    Vec3   pixelDelta_u;       // Offset to pixel to the right
    Vec3   pixelDelta_v;       // Offset to pixel below
    Vec3   u, v, w;            // Camera frame basis vectors
    Vec3   defocusDisk_u;      // Defocus disk horizontal radius
    Vec3   defocusDisk_v;      // Defocus disk vertical radius

    void Initialize()
    {
        // Calculate the image height, and ensure that it's at least 1.
        imageHeight = int(imageWidth / aspectRatio);
        imageHeight = (imageHeight < 1) ? 1 : imageHeight;

        pixelSamplesScale = 1.0 / samplesPerPixel;

        center = lookFrom;

        // Determine viewport dimensions.
        const double theta = DegToRad(vfov);
        const double h = std::tan(theta / 2);
        const double viewportHeight = 2 * h * focusDist;
        const double viewportWidth = viewportHeight * (double(imageWidth) / imageHeight);
        
        // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
        w = UnitVector(lookFrom - lookAt);
        u = UnitVector(Cross(up, w));
        v = Cross(w, u);

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        const Vec3 viewport_u = viewportWidth * u;    // Vector across viewport horizontal edge
        const Vec3 viewport_v = viewportHeight * -v;  // Vector down viewport vertical edge

        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        pixelDelta_u = viewport_u / imageWidth;
        pixelDelta_v = viewport_v / imageHeight;

        // Calculate the location of the upper left pixel.
        const Vec3 viewportUpperLeft = center - (focusDist * w) - viewport_u / 2 - viewport_v / 2;
        pixel00Loc = viewportUpperLeft + 0.5 * (pixelDelta_u + pixelDelta_v);

        // Calculate the camera defocus disk basis vectors.
        const double defocusRadius = focusDist * std::tan(DegToRad(defocusAngle / 2));
        defocusDisk_u = u * defocusRadius;
        defocusDisk_v = v * defocusRadius;
    }

    Ray GetRay(int i, int j) const
    {
        // Construct a camera ray originating from the defocus disk and directed at a randomly
        // sampled point around the pixel location i, j.

        const Vec3 offset = SampleSquare();
        const Vec3 pixelSample = pixel00Loc
            + ((i + offset.x()) * pixelDelta_u)
            + ((j + offset.y()) * pixelDelta_v);

        const Vec3 rayOrigin = (defocusAngle <= 0) ? center : DefocusDiskSample();
        const Vec3 rayDirection = pixelSample - rayOrigin;
        const double rayTime = Random::Double();

        return Ray(rayOrigin, rayDirection, rayTime);
    }

    static Vec3 SampleSquare()
    {
        // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
        return Vec3(Random::Double() - 0.5, Random::Double() - 0.5, 0);
    }

    Point3 DefocusDiskSample() const
    {
        // Returns a random point in the camera defocus disk.
        auto p = Random::InUnitDisk();
        return center + (p[0] * defocusDisk_u) + (p[1] * defocusDisk_v);
    }

    Color RayColor(const Ray& r, int depth, const Hittable& world) const
    {
        // If we've exceeded the ray bounce limit, no more light is gathered.
        if (depth <= 0)
            return Color(0, 0, 0);

        HitRecord rec;


        // If the ray hits nothing, return the background color.
        if (!world.Hit(r, Interval(0.001, infinity), rec))
            return background;

        Ray scattered;
        Color attenuation;
        const Color colorFromEmission = rec.mat->Emitted(rec.u, rec.v, rec.p);

        if (!rec.mat->Scatter(r, rec, attenuation, scattered))
            return colorFromEmission;

        const Color colorFromScatter = attenuation * RayColor(scattered, depth - 1, world);

        return colorFromEmission + colorFromScatter;
    }
};