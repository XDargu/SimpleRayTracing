#pragma once

#include "hittable.h"
#include "texture.h"

class Material
{
public:
    virtual ~Material() = default;

    virtual bool Scatter(const Ray& r_in, const HitRecord& rec, Color& attenuation, Ray& scattered) const
    {
        return false;
    }

    virtual Color Emitted(double u, double v, const Point3& p) const
    {
        return Color(0, 0, 0);
    }
};

class Lambertian : public Material
{
public:
    Lambertian(const Color& albedo)
        : tex(make_shared<SolidColor>(albedo))
    {}

    Lambertian(shared_ptr<Texture> tex)
        : tex(tex)
    {}

    bool Scatter(const Ray& r_in, const HitRecord& rec, Color& attenuation, Ray& scattered) const override
    {
        Vec3 scatterDirection = rec.normal + Random::UnitVector();

        // Catch degenerate scatter direction
        if (scatterDirection.isNearZero())
            scatterDirection = rec.normal;

        scattered = Ray(rec.p, scatterDirection, r_in.time());
        attenuation = tex->Value(rec.u, rec.v, rec.p);;
        return true;
    }

private:
    shared_ptr<Texture> tex;
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
        scattered = Ray(rec.p, reflected, r_in.time());
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

        scattered = Ray(rec.p, direction, r_in.time());
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

class DiffuseLight : public Material
{
public:
    DiffuseLight(shared_ptr<Texture> tex) : tex(tex) {}
    DiffuseLight(const Color& emit) : tex(make_shared<SolidColor>(emit)) {}

    Color Emitted(double u, double v, const Point3& p) const override
    {
        return tex->Value(u, v, p);
    }

private:
    shared_ptr<Texture> tex;
};

class Isotropic : public Material
{
public:
    Isotropic(const Color& albedo) : tex(make_shared<SolidColor>(albedo)) {}
    Isotropic(shared_ptr<Texture> tex) : tex(tex) {}

    bool Scatter(const Ray& r_in, const HitRecord& rec, Color& attenuation, Ray& scattered) const override
    {
        scattered = Ray(rec.p, Random::UnitVector(), r_in.time());
        attenuation = tex->Value(rec.u, rec.v, rec.p);
        return true;
    }

private:
    shared_ptr<Texture> tex;
};