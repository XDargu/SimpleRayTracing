#pragma once

#include "hittable.h"
#include "material.h"
#include "texture.h"

class ConstantMedium : public Hittable
{
public:
    ConstantMedium(shared_ptr<Hittable> boundary, double density, shared_ptr<Texture> tex)
        : boundary(boundary)
        , negInvDensity(-1 / density)
        , phaseFunction(make_shared<Isotropic>(tex))
    {}

    ConstantMedium(shared_ptr<Hittable> boundary, double density, const Color& albedo)
        : boundary(boundary)
        , negInvDensity(-1 / density)
        , phaseFunction(make_shared<Isotropic>(albedo))
    {}

    bool Hit(const Ray& r, const Interval& ray_t, HitRecord& rec) const override
    {
        HitRecord rec1, rec2;

        if (!boundary->Hit(r, Interval::universe, rec1))
            return false;

        if (!boundary->Hit(r, Interval(rec1.t + 0.0001, infinity), rec2))
            return false;

        if (rec1.t < ray_t.min) rec1.t = ray_t.min;
        if (rec2.t > ray_t.max) rec2.t = ray_t.max;

        if (rec1.t >= rec2.t)
            return false;

        if (rec1.t < 0)
            rec1.t = 0;

        const double rayLength = r.direction().Length();
        const double distance_inside_boundary = (rec2.t - rec1.t) * rayLength;
        const double hit_distance = negInvDensity * std::log(Random::Double());

        if (hit_distance > distance_inside_boundary)
            return false;

        rec.t = rec1.t + hit_distance / rayLength;
        rec.p = r.at(rec.t);

        rec.normal = Vec3(1, 0, 0);  // arbitrary
        rec.frontFace = true;        // also arbitrary
        rec.mat = phaseFunction;

        return true;
    }

    AABB BoundingBox() const override { return boundary->BoundingBox(); }

private:
    shared_ptr<Hittable> boundary;
    double negInvDensity;
    shared_ptr<Material> phaseFunction;
};
