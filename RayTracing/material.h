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

class Dielectric : public Material
{
public:
    Dielectric(double refractionIndex) : refractionIndex(refractionIndex) {}

    bool Scatter(const Ray& r_in, const HitRecord& rec, Color& attenuation, Ray& scattered) const override
    {
        attenuation = Color(1.0, 1.0, 1.0);
        const double ri = rec.frontFace ? (1.0 / refractionIndex) : refractionIndex;

        const Vec3 unit_direction = UnitVector(r_in.direction());
        const Vec3 refracted = Refract(unit_direction, rec.normal, ri);

        const double cosTheta = std::fmin(Dot(-unit_direction, rec.normal), 1.0);
        const double sinTheta = std::sqrt(1.0 - cosTheta * cosTheta);

        const bool cannotRefract = ri * sinTheta > 1.0;
        Vec3 direction;

        if (cannotRefract || Reflectance(cosTheta, ri) > Random::Double())
            direction = Reflect(unit_direction, rec.normal);
        else
            direction = Refract(unit_direction, rec.normal, ri);

        scattered = Ray(rec.p, direction);
        return true;
    }

    static double Reflectance(double cosine, double refractionIndex)
    {
        // Use Schlick's approximation for reflectance.
        double r0 = (1 - refractionIndex) / (1 + refractionIndex);
        r0 = r0 * r0;
        return r0 + (1 - r0) * std::pow((1 - cosine), 5);
    }


private:
    // Refractive index in vacuum or air, or the ratio of the material's refractive index over
    // the refractive index of the enclosing media
    double refractionIndex;
};