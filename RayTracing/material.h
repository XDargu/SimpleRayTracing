#pragma once

#include "hittable.h"

class Material
{
public:
    virtual ~Material() = default;

    virtual bool Scatter(const Ray& r_in, const HitRecord& rec, Color& attenuation, Ray& scattered) const
    {
        return false;
    }
};

class Lambertian : public Material
{
public:
    Lambertian(const Color& albedo) : albedo(albedo) {}

    bool Scatter(const Ray& r_in, const HitRecord& rec, Color& attenuation, Ray& scattered) const override
    {
        Vec3 scatterDirection = rec.normal + Random::UnitVector();

        // Catch degenerate scatter direction
        if (scatterDirection.isNearZero())
            scatterDirection = rec.normal;

        scattered = Ray(rec.p, scatterDirection);
        attenuation = albedo;
        return true;
    }

private:
    Color albedo;
};

class Metal : public Material
{
public:
    Metal(const Color& albedo, double fuzz)
        : albedo(albedo)
        , fuzz(fuzz < 1 ? fuzz : 1)
    {}

    bool Scatter(const Ray& r_in, const HitRecord& rec, Color& attenuation, Ray& scattered) const override
    {
        Vec3 reflected = Reflect(r_in.direction(), rec.normal);
        reflected = UnitVector(reflected) + (fuzz * Random::UnitVector());
        scattered = Ray(rec.p, reflected);
        attenuation = albedo;
        return (Dot(scattered.direction(), rec.normal) > 0);
    }

private:
    Color albedo;
    double fuzz;
};