#pragma once

class Vec3
{
public:
    double e[3];

    Vec3() : e{ 0,0,0 } {}
    Vec3(double e0, double e1, double e2) : e{ e0, e1, e2 } {}

    double x() const { return e[0]; }
    double y() const { return e[1]; }
    double z() const { return e[2]; }

    Vec3 operator-() const { return Vec3(-e[0], -e[1], -e[2]); }
    double operator[](int i) const { return e[i]; }
    double& operator[](int i) { return e[i]; }

    Vec3& operator+=(const Vec3& v)
    {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }

    Vec3& operator*=(double t)
    {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    Vec3& operator/=(double t)
    {
        return *this *= 1 / t;
    }

    double Length() const
    {
        return std::sqrt(LengthSquared());
    }

    double LengthSquared() const
    {
        return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
    }

    static Vec3 Random()
    {
        return Vec3(Random::Double(), Random::Double(), Random::Double());
    }

    static Vec3 Random(double min, double max)
    {
        return Vec3(Random::Double(min, max), Random::Double(min, max), Random::Double(min, max));
    }
};

// Point3 is just an alias for Vec3, but useful for geometric clarity in the code.
using Point3 = Vec3;


// Vector Utility Functions
inline std::ostream& operator<<(std::ostream& out, const Vec3& v)
{
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline Vec3 operator+(const Vec3& u, const Vec3& v)
{
    return Vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline Vec3 operator-(const Vec3& u, const Vec3& v)
{
    return Vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline Vec3 operator*(const Vec3& u, const Vec3& v)
{
    return Vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline Vec3 operator*(double t, const Vec3& v)
{
    return Vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

inline Vec3 operator*(const Vec3& v, double t)
{
    return t * v;
}

inline Vec3 operator/(const Vec3& v, double t)
{
    return (1 / t) * v;
}

inline double Dot(const Vec3& u, const Vec3& v)
{
    return u.e[0] * v.e[0]
        + u.e[1] * v.e[1]
        + u.e[2] * v.e[2];
}

inline Vec3 Cross(const Vec3& u, const Vec3& v)
{
    return Vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
        u.e[2] * v.e[0] - u.e[0] * v.e[2],
        u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline Vec3 UnitVector(const Vec3& v)
{
    return v / v.Length();
}

namespace Random
{
    inline Vec3 UnitVector()
    {
        while (true)
        {
            const Vec3 p = Vec3::Random(-1, 1);
            const double lensq = p.LengthSquared();
            if (1e-160 < lensq && lensq <= 1)
                return p / sqrt(lensq);
        }
    }

    inline Vec3 OnHemisphere(const Vec3& normal)
    {
        const Vec3 on_unit_sphere = UnitVector();
        if (Dot(on_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
            return on_unit_sphere;
        else
            return -on_unit_sphere;
    }
}