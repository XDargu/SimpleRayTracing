#include "raytracing.h"

#include "camera.h"
#include "hittable.h"
#include "hittableList.h"
#include "material.h"
#include "sphere.h"

int main()
{
    HittableList world;
    
    auto materialGround = make_shared<Lambertian>(Color(0.8, 0.8, 0.0));
    auto materialCenter = make_shared<Lambertian>(Color(0.1, 0.2, 0.5));
    auto materialLeft = make_shared<Dielectric>(1.5);
    auto material_bubble = make_shared<Dielectric>(1.00 / 1.5);
    auto materialRight = make_shared<Metal>(Color(0.8, 0.6, 0.2), 1.0);

    world.Add(make_shared<Sphere>(Point3(0.0, -100.5, -1.0), 100.0, materialGround));
    world.Add(make_shared<Sphere>(Point3(0.0, 0.0, -1.2), 0.5, materialCenter));
    world.Add(make_shared<Sphere>(Point3(-1.0, 0.0, -1.0), 0.5, materialLeft));
    world.Add(make_shared<Sphere>(Point3(-1.0, 0.0, -1.0), 0.4, material_bubble));
    world.Add(make_shared<Sphere>(Point3(1.0, 0.0, -1.0), 0.5, materialRight));


    Camera cam;

    cam.aspectRatio = 16.0 / 9.0;
    cam.imageWidth = 400;
    cam.samplesPerPixel = 10;
    cam.maxDepth = 50;

    cam.Render(world);
}