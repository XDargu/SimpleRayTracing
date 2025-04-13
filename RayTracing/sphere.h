#pragma once

#include "hittable.h"

class Sphere : public Hittable
{
public:
    // Stationary Sphere
    Sphere(const Point3& staticCenter, double radius, shared_ptr<Material> mat)
        : center(staticCenter, Vec3(0, 0, 0))
        , radius(std::fmax(0, radius))
        , mat(mat)
    {}

    // Moving Sphere
    Sphere(const Point3& center1, const Point3& center2, double radius, shared_ptr<Material> mat)
        : center(center1, center2 - center1)
        , radius(std::fmax(0, radius))
        , mat(mat)
    {}

    bool Hit(const Ray& r, const Interval& ray_t, HitRecord& rec) const override
    {
        const Point3 currentCenter = center.at(r.time());
        const Vec3 oc = currentCenter - r.origin();
        const double a = r.direction().LengthSquared();
        const double h = Dot(r.direction(), oc);
        const double c = oc.LengthSquared() - radius * radius;

        const double discriminant = h * h - a * c;
        if (discriminant < 0)
            return false;

        const double sqrtd = std::sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        double root = (h - sqrtd) / a;
        if (!ray_t.Surrounds(root))
        {
            root = (h + sqrtd) / a;
            if (!ray_t.Surrounds(root))
                return false;
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        Vec3 outwardNormal = (rec.p - currentCenter) / radius;
        rec.SetFaceNormal(r, outwardNormal);
        rec.mat = mat;

        return true;
    }

private:
    Ray center;
    double radius;
    shared_ptr<Material> mat;
};