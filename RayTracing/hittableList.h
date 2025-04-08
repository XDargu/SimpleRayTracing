#pragma once

#include "Hittable.h"

#include <vector>

class HittableList : public Hittable
{
public:
    std::vector<shared_ptr<Hittable>> objects;

    HittableList() {}
    HittableList(shared_ptr<Hittable> object) { Add(object); }

    void Clear() { objects.clear(); }

    void Add(shared_ptr<Hittable> object)
    {
        objects.push_back(object);
    }

    bool Hit(const Ray& r, const Interval& ray_t, HitRecord& rec) const override
    {
        HitRecord temp_rec;
        bool hitAnything = false;
        double closestSoFar = ray_t.max;

        for (const shared_ptr<Hittable>& object : objects)
        {
            if (object->Hit(r, Interval(ray_t.min, closestSoFar), temp_rec))
            {
                hitAnything = true;
                closestSoFar = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hitAnything;
    }
};