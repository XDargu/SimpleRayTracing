#include "raytracing.h"

#include "bvh.h"
#include "camera.h"
#include "hittable.h"
#include "hittableList.h"
#include "material.h"
#include "quad.h"
#include "sphere.h"
#include "texture.h"

void Quads()
{
    HittableList world;

    // Materials
    auto leftRed     = make_shared<Lambertian>(Color(1.0, 0.2, 0.2));
    auto backGreen   = make_shared<Lambertian>(Color(0.2, 1.0, 0.2));
    auto rightBlue   = make_shared<Lambertian>(Color(0.2, 0.2, 1.0));
    auto upperOrange = make_shared<Lambertian>(Color(1.0, 0.5, 0.0));
    auto lowerTeal   = make_shared<Lambertian>(Color(0.2, 0.8, 0.8));

    // Quads
    world.Add(make_shared<Quad>(Point3(-3, -2, 5), Vec3(0, 0,-4), Vec3(0, 4, 0), leftRed));
    world.Add(make_shared<Quad>(Point3(-2, -2, 0), Vec3(4, 0, 0), Vec3(0, 4, 0), backGreen));
    world.Add(make_shared<Quad>(Point3( 3, -2, 1), Vec3(0, 0, 4), Vec3(0, 4, 0), rightBlue));
    world.Add(make_shared<Quad>(Point3(-2,  3, 1), Vec3(4, 0, 0), Vec3(0, 0, 4), upperOrange));
    world.Add(make_shared<Quad>(Point3(-2, -3, 5), Vec3(4, 0, 0), Vec3(0, 0,-4), lowerTeal));

    Camera cam;

    cam.aspectRatio = 1.0;
    cam.imageWidth = 400;
    cam.samplesPerPixel = 10;
    cam.maxDepth = 50;

    cam.vfov = 80;
    cam.lookFrom = Point3(0, 0, 9);
    cam.lookAt = Point3(0, 0, 0);
    cam.up = Vec3(0, 1, 0);

    cam.defocusAngle = 0;

    cam.Render(world);
}

void PerlinSpheres()
{
    HittableList world;

    auto pertext = make_shared<NoiseTexture>(4);
    world.Add(make_shared<Sphere>(Point3(0, -1000, 0), 1000, make_shared<Lambertian>(pertext)));
    world.Add(make_shared<Sphere>(Point3(0, 2, 0), 2, make_shared<Lambertian>(pertext)));

    Camera cam;

    cam.aspectRatio = 16.0 / 9.0;
    cam.imageWidth = 400;
    cam.samplesPerPixel = 10;
    cam.maxDepth = 50;

    cam.vfov = 20;
    cam.lookFrom = Point3(13, 2, 3);
    cam.lookAt = Point3(0, 0, 0);
    cam.up = Vec3(0, 1, 0);

    cam.defocusAngle = 0;

    cam.Render(world);
}

void Earth()
{
    auto earthTexture = make_shared<ImageTexture>("earthmap.jpg");
    auto earthSurface = make_shared<Lambertian>(earthTexture);
    auto globe = make_shared<Sphere>(Point3(0, 0, 0), 2, earthSurface);

    Camera cam;

    cam.aspectRatio = 16.0 / 9.0;
    cam.imageWidth = 400;
    cam.samplesPerPixel = 10;
    cam.maxDepth = 50;

    cam.vfov = 20;
    cam.lookFrom = Point3(0, 0, 12);
    cam.lookAt = Point3(0, 0, 0);
    cam.up = Vec3(0, 1, 0);

    cam.defocusAngle = 0;

    cam.Render(HittableList(globe));
}

void BouncingSpheres()
{
    HittableList world;

    shared_ptr<Texture> checker = make_shared<CheckerTexture>(0.32, Color(.2, .3, .1), Color(.9, .9, .9));
    shared_ptr<Material> ground_material = make_shared<Lambertian>(checker);

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

void CheckeredSpheres()
{
    HittableList world;

    auto checker = make_shared<CheckerTexture>(0.32, Color(.2, .3, .1), Color(.9, .9, .9));

    world.Add(make_shared<Sphere>(Point3(0, -10, 0), 10, make_shared<Lambertian>(checker)));
    world.Add(make_shared<Sphere>(Point3(0, 10, 0), 10, make_shared<Lambertian>(checker)));

    Camera cam;

    cam.aspectRatio = 16.0 / 9.0;
    cam.imageWidth = 400;
    cam.samplesPerPixel = 10;
    cam.maxDepth = 50;

    cam.vfov = 20;
    cam.lookFrom = Point3(13, 2, 3);
    cam.lookAt = Point3(0, 0, 0);
    cam.up = Vec3(0, 1, 0);

    cam.defocusAngle = 0;

    cam.Render(world);
}

int main()
{
    switch (5)
    {
        case 1: BouncingSpheres();  break;
        case 2: CheckeredSpheres(); break;
        case 3: Earth();            break;
        case 4: PerlinSpheres();    break;
        case 5: Quads();    break;
    }
}