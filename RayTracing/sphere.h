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
    {
        const Vec3 rvec = Vec3(radius, radius, radius);
        bbox = AABB(staticCenter - rvec, staticCenter + rvec);
    }

    // Moving Sphere
    Sphere(const Point3& center1, const Point3& center2, double radius, shared_ptr<Material> mat)
        : center(center1, center2 - center1)
        , radius(std::fmax(0, radius))
        , mat(mat)
    {
        const Vec3 rvec = Vec3(radius, radius, radius);
        AABB box1(center.at(0) - rvec, center.at(0) + rvec);
        AABB box2(center.at(1) - rvec, center.at(1) + rvec);
        bbox = AABB(box1, box2);
    }

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
        GetSphere_UV(outwardNormal, rec.u, rec.v);
        rec.mat = mat;

        return true;
    }

    virtual AABB BoundingBox() const { return bbox; }

private:

    static void GetSphere_UV(const Point3& p, double& u, double& v)
    {
        // p: a given point on the sphere of radius one, centered at the origin.
        // u: returned value [0,1] of angle around the Y axis from X=-1.
        // v: returned value [0,1] of angle from Y=-1 to Y=+1.
        //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
        //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
        //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

        const double theta = std::acos(-p.y());
        const double phi = std::atan2(-p.z(), p.x()) + pi;

        u = phi / (2 * pi);
        v = theta / pi;
    }

private:
    Ray center;
    double radius;
    shared_ptr<Material> mat;
    AABB bbox;
};