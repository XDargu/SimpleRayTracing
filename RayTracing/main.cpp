#include "raytracing.h"

#include "bvh.h"
#include "camera.h"
#include "hittable.h"
#include "hittableList.h"
#include "material.h"
#include "Sphere.h"

int main()
{
    HittableList world;
    
    auto ground_material = make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
    world.Add(make_shared<Sphere>(Point3(0, -1000, 0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = Random::Double();
            Point3 center(a + 0.9 * Random::Double(), 0.2, b + 0.9 * Random::Double());

            if ((center - Point3(4, 0.2, 0)).Length() > 0.9) {
                shared_ptr<Material> Sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    const Color albedo = Color::Random() * Color::Random();
                    Sphere_material = make_shared<Lambertian>(albedo);
                    const Vec3 center2 = center + Vec3(0, Random::Double(0, .5), 0);
                    world.Add(make_shared<Sphere>(center, center2, 0.2, Sphere_material));
                }
                else if (choose_mat < 0.95) {
                    // metal
                    const Color albedo = Color::Random(0.5, 1);
                    const double fuzz = Random::Double(0, 0.5);
                    Sphere_material = make_shared<Metal>(albedo, fuzz);
                    world.Add(make_shared<Sphere>(center, 0.2, Sphere_material));
                }
                else {
                    // glass
                    Sphere_material = make_shared<Dielectric>(1.5);
                    world.Add(make_shared<Sphere>(center, 0.2, Sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<Dielectric>(1.5);
    world.Add(make_shared<Sphere>(Point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<Lambertian>(Color(0.4, 0.2, 0.1));
    world.Add(make_shared<Sphere>(Point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<Metal>(Color(0.7, 0.6, 0.5), 0.0);
    world.Add(make_shared<Sphere>(Point3(4, 1, 0), 1.0, material3));

    world = HittableList(make_shared<BVH_Node>(world));

    Camera cam;

    cam.aspectRatio = 16.0 / 9.0;
    cam.imageWidth = 400;
    cam.samplesPerPixel = 10;
    cam.maxDepth = 50;

    cam.vfov = 20;
    cam.lookFrom = Point3(13, 2, 3);
    cam.lookAt = Point3(0, 0, 0);
    cam.up = Vec3(0, 1, 0);

    cam.defocusAngle = 0.6;
    cam.focusDist = 10.0;

    cam.Render(world);
}