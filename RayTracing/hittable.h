#pragma once

struct HitRecord
{
    Point3 p;
    Vec3 normal;
    double t;
    bool frontFace;

    void SetFaceNormal(const Ray& r, const Vec3& outwardNormal)
    {
        // Sets the hit record normal vector.
        // NOTE: the parameter `outwardNormal` is assumed to have unit length.

        frontFace = Dot(r.direction(), outwardNormal) < 0;
        normal = frontFace ? outwardNormal : -outwardNormal;
    }
};

class Hittable
{
public:
    virtual ~Hittable() = default;

    virtual bool Hit(const Ray& r, const Interval& ray_t, HitRecord& rec) const = 0;
};