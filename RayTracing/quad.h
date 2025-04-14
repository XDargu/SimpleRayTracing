#pragma once

#include "hittable.h"
#include "hittableList.h"

class Quad : public Hittable
{
public:
    Quad(const Point3& Q, const Vec3& u, const Vec3& v, shared_ptr<Material> mat)
        : Q(Q)
        , u(u)
        , v(v)
        , mat(mat)
    {
        const Vec3 n = Cross(u, v);
        normal = UnitVector(n);
        D = Dot(normal, Q);
        w = n / Dot(n, n);

        SetBoundingBox();
    }

    virtual void SetBoundingBox()
    {
        // Compute the bounding box of all four vertices.
        const AABB bboxDiagonal1 = AABB(Q, Q + u + v);
        const AABB bboxDiagonal2 = AABB(Q + u, Q + v);
        bbox = AABB(bboxDiagonal1, bboxDiagonal2);
    }

    AABB BoundingBox() const override { return bbox; }

    bool Hit(const Ray& r, const Interval& ray_t, HitRecord& rec) const override
    {
        const double denom = Dot(normal, r.direction());

        // No hit if the ray is parallel to the plane.
        if (std::fabs(denom) < 1e-8)
            return false;

        // Return false if the hit point parameter t is outside the ray interval.
        const double t = (D - Dot(normal, r.origin())) / denom;
        if (!ray_t.Contains(t))
            return false;

        // Determine if the hit point lies within the planar shape using its plane coordinates.
        const Point3 intersection = r.at(t);
        const Vec3 planarHitptVector = intersection - Q;
        const double alpha = Dot(w, Cross(planarHitptVector, v));
        const double beta = Dot(w, Cross(u, planarHitptVector));

        if (!IsInterior(alpha, beta, rec))
            return false;

        // Ray hits the 2D shape; set the rest of the hit record and return true.
        rec.t = t;
        rec.p = intersection;
        rec.mat = mat;
        rec.SetFaceNormal(r, normal);

        return true;
    }

    virtual bool IsInterior(double a, double b, HitRecord& rec) const
    {
        const Interval unitInterval = Interval(0, 1);
        // Given the hit point in plane coordinates, return false if it is outside the
        // primitive, otherwise set the hit record UV coordinates and return true.

        if (!unitInterval.Contains(a) || !unitInterval.Contains(b))
            return false;

        rec.u = a;
        rec.v = b;
        return true;
    }

private:
    Point3 Q;
    Vec3 u, v;
    Vec3 w;
    shared_ptr<Material> mat;
    AABB bbox;
    Vec3 normal;
    double D;
};

inline shared_ptr<HittableList> Box(const Point3& a, const Point3& b, shared_ptr<Material> mat)
{
    // Returns the 3D box (six sides) that contains the two opposite vertices a & b.

    auto sides = make_shared<HittableList>();

    // Construct the two opposite vertices with the minimum and maximum coordinates.
    const Point3 min = Point3(std::fmin(a.x(), b.x()), std::fmin(a.y(), b.y()), std::fmin(a.z(), b.z()));
    const Point3 max = Point3(std::fmax(a.x(), b.x()), std::fmax(a.y(), b.y()), std::fmax(a.z(), b.z()));

    const Vec3 dx = Vec3(max.x() - min.x(), 0, 0);
    const Vec3 dy = Vec3(0, max.y() - min.y(), 0);
    const Vec3 dz = Vec3(0, 0, max.z() - min.z());

    sides->Add(make_shared<Quad>(Point3(min.x(), min.y(), max.z()), dx, dy, mat)); // front
    sides->Add(make_shared<Quad>(Point3(max.x(), min.y(), max.z()),-dz, dy, mat)); // right
    sides->Add(make_shared<Quad>(Point3(max.x(), min.y(), min.z()),-dx, dy, mat)); // back
    sides->Add(make_shared<Quad>(Point3(min.x(), min.y(), min.z()), dz, dy, mat)); // left
    sides->Add(make_shared<Quad>(Point3(min.x(), max.y(), max.z()), dx,-dz, mat)); // top
    sides->Add(make_shared<Quad>(Point3(min.x(), min.y(), min.z()), dx, dz, mat)); // bottom

    return sides;
}