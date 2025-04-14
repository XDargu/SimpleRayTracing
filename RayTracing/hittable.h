#pragma once

#include "aabb.h"

class Material;

struct HitRecord
{
    Point3 p;
    Vec3 normal;
    shared_ptr<Material> mat;
    double t;
    double u;
    double v;
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
    virtual AABB BoundingBox() const = 0;
};

class Translate : public Hittable
{
public:
    Translate(shared_ptr<Hittable> object, const Vec3& offset)
        : object(object)
        , offset(offset)
    {
        bbox = object->BoundingBox() + offset;
    }

    bool Hit(const Ray& r, const Interval& ray_t, HitRecord& rec) const override
    {
        // Move the ray backwards by the offset
        const Ray offset_r(r.origin() - offset, r.direction(), r.time());

        // Determine whether an intersection exists along the offset ray (and if so, where)
        if (!object->Hit(offset_r, ray_t, rec))
            return false;

        // Move the intersection point forwards by the offset
        rec.p += offset;

        return true;
    }

    AABB BoundingBox() const override { return bbox; }

private:
    shared_ptr<Hittable> object;
    Vec3 offset;
    AABB bbox;
};

class Rotate_Y : public Hittable
{
public:

    Rotate_Y(shared_ptr<Hittable> object, double angle)
        : object(object)
    {
        const double radians = DegToRad(angle);
        sinTheta = std::sin(radians);
        cosTheta = std::cos(radians);
        bbox = object->BoundingBox();

        Point3 min(infinity, infinity, infinity);
        Point3 max(-infinity, -infinity, -infinity);

        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                for (int k = 0; k < 2; k++)
                {
                    const double x = i * bbox.x.max + (1 - i) * bbox.x.min;
                    const double y = j * bbox.y.max + (1 - j) * bbox.y.min;
                    const double z = k * bbox.z.max + (1 - k) * bbox.z.min;

                    const double newx = cosTheta * x + sinTheta * z;
                    const double newz = -sinTheta * x + cosTheta * z;

                    const Vec3 tester(newx, y, newz);

                    for (int c = 0; c < 3; c++) {
                        min[c] = std::fmin(min[c], tester[c]);
                        max[c] = std::fmax(max[c], tester[c]);
                    }
                }
            }
        }

        bbox = AABB(min, max);
    }

    bool Hit(const Ray& r, const Interval& ray_t, HitRecord& rec) const override
    {
        // Transform the ray from world space to object space.

        const Point3 origin = Point3(
            (cosTheta * r.origin().x()) - (sinTheta * r.origin().z()),
            r.origin().y(),
            (sinTheta * r.origin().x()) + (cosTheta * r.origin().z())
        );

        const Vec3 direction = Vec3(
            (cosTheta * r.direction().x()) - (sinTheta * r.direction().z()),
            r.direction().y(),
            (sinTheta * r.direction().x()) + (cosTheta * r.direction().z())
        );

        const Ray rotated_r(origin, direction, r.time());

        // Determine whether an intersection exists in object space (and if so, where).
        if (!object->Hit(rotated_r, ray_t, rec))
            return false;

        // Transform the intersection from object space back to world space.
        rec.p = Point3(
            (cosTheta * rec.p.x()) + (sinTheta * rec.p.z()),
            rec.p.y(),
            (-sinTheta * rec.p.x()) + (cosTheta * rec.p.z())
        );

        rec.normal = Vec3(
            (cosTheta * rec.normal.x()) + (sinTheta * rec.normal.z()),
            rec.normal.y(),
            (-sinTheta * rec.normal.x()) + (cosTheta * rec.normal.z())
        );

        return true;
    }

    AABB BoundingBox() const override { return bbox; }

private:
    shared_ptr<Hittable> object;
    double sinTheta;
    double cosTheta;
    AABB bbox;
};