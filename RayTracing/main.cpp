#include "raytracing.h"

#include "camera.h"
#include "hittable.h"
#include "hittableList.h"
#include "material.h"
#include "sphere.h"

int main()
{
    HittableList world;
    
    auto R = std::cos(pi / 4);

    auto material_ground = make_shared<Lambertian>(Color(0.8, 0.8, 0.0));
    auto material_center = make_shared<Lambertian>(Color(0.1, 0.2, 0.5));
    auto material_left = make_shared<Dielectric>(1.50);
    auto material_bubble = make_shared<Dielectric>(1.00 / 1.50);
    auto material_right = make_shared<Metal>(Color(0.8, 0.6, 0.2), 1.0);

    world.Add(make_shared<Sphere>(Point3(0.0, -100.5, -1.0), 100.0, material_ground));
    world.Add(make_shared<Sphere>(Point3(0.0, 0.0, -1.2), 0.5, material_center));
    world.Add(make_shared<Sphere>(Point3(-1.0, 0.0, -1.0), 0.5, material_left));
    world.Add(make_shared<Sphere>(Point3(-1.0, 0.0, -1.0), 0.4, material_bubble));
    world.Add(make_shared<Sphere>(Point3(1.0, 0.0, -1.0), 0.5, material_right));


    Camera cam;

    cam.aspectRatio = 16.0 / 9.0;
    cam.imageWidth = 400;
    cam.samplesPerPixel = 10;
    cam.maxDepth = 50;

    cam.vfov = 20;
    cam.lookFrom = Point3(-2, 2, 1);
    cam.lookAt = Point3(0, 0, -1);
    cam.up = Vec3(0, 1, 0);

    cam.Render(world);
}