#pragma once

#include "aabb.h"
#include "hittable.h"
#include "hittableList.h"

#include <algorithm>

class BVH_Node : public Hittable
{
public:
    BVH_Node(HittableList list)
        : BVH_Node(list.objects, 0, list.objects.size())
    {
        // There's a C++ subtlety here. This constructor (without span indices) creates an
        // implicit copy of the hittable list, which we will modify. The lifetime of the copied
        // list only extends until this constructor exits. That's OK, because we only need to
        // persist the resulting bounding volume hierarchy.
        // Note that this is copying the *pointers* not the object themselves
    }

    BVH_Node(std::vector<shared_ptr<Hittable>>& objects, size_t start, size_t end)
    {
        // Build the bounding box of the span of source objects.
        bbox = AABB::empty;
        for (size_t objectIdx = start; objectIdx < end; objectIdx++)
        {
            bbox = AABB(bbox, objects[objectIdx]->BoundingBox());
        }

        const int axis = bbox.LongestAxis();

        auto comparator = (axis == 0) ? BoxCompare_X
            : (axis == 1) ? BoxCompare_Y
            : BoxCompare_Z;

        const size_t objectSpan = end - start;

        if (objectSpan == 1)
        {
            left = right = objects[start];
        }
        else if (objectSpan == 2)
        {
            left = objects[start];
            right = objects[start + 1];
        }
        else
        {
            std::sort(std::begin(objects) + start, std::begin(objects) + end, comparator);

            const size_t mid = start + objectSpan / 2;
            left = make_shared<BVH_Node>(objects, start, mid);
            right = make_shared<BVH_Node>(objects, mid, end);
        }

        bbox = AABB(left->BoundingBox(), right->BoundingBox());
    }

    bool Hit(const Ray& r, const Interval& ray_t, HitRecord& rec) const override
    {
        if (!bbox.Hit(r, ray_t))
            return false;

        const bool hitLeft = left->Hit(r, ray_t, rec);
        const bool hitRight = right->Hit(r, Interval(ray_t.min, hitLeft ? rec.t : ray_t.max), rec);

        return hitLeft || hitRight;
    }

    AABB BoundingBox() const override { return bbox; }

private:

    static bool BoxCompare(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b, int axisIndex)
    {
        auto a_axisInterval = a->BoundingBox().AxisInterval(axisIndex);
        auto b_axisInterval = b->BoundingBox().AxisInterval(axisIndex);
        return a_axisInterval.min < b_axisInterval.min;
    }

    static bool BoxCompare_X(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b) {
        return BoxCompare(a, b, 0);
    }

    static bool BoxCompare_Y(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b) {
        return BoxCompare(a, b, 1);
    }

    static bool BoxCompare_Z(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b) {
        return BoxCompare(a, b, 2);
    }

private:
    shared_ptr<Hittable> left;
    shared_ptr<Hittable> right;
    AABB bbox;
};