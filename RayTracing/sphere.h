#pragma once

#include "hittable.h"

class Sphere : public Hittable
{
public:
    Sphere(const Point3& center, double radius, shared_ptr<Material> mat)
        : center(center)
        , radius(std::fmax(0, radius))
        , mat(mat)
    {}

    bool Hit(const Ray& r, const Interval& ray_t, HitRecord& rec) const override
    {
        const Vec3 oc = center - r.origin();
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
        rec.normal = (rec.p - center) / radius;
        Vec3 outwardNormal = (rec.p - center) / radius;
        rec.SetFaceNormal(r, outwardNormal);
        rec.mat = mat;

        return true;
    }

private:
    Point3 center;
    double radius;
    shared_ptr<Material> mat;
};