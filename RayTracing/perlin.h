#pragma once

#include <algorithm>

class Perlin
{
public:
    Perlin()
    {
        for (int i = 0; i < pointCount; i++)
        {
            randvec[i] = UnitVector(Vec3::Random(-1, 1));
        }

        PerlinGeneratePerm(perm_x);
        PerlinGeneratePerm(perm_y);
        PerlinGeneratePerm(perm_z);
    }

    double Noise(const Point3& p) const
    {
        const double u = p.x() - std::floor(p.x());
        const double v = p.y() - std::floor(p.y());
        const double w = p.z() - std::floor(p.z());

        const int i = int(std::floor(p.x()));
        const int j = int(std::floor(p.y()));
        const int k = int(std::floor(p.z()));
        Vec3 c[2][2][2];

        for (int di = 0; di < 2; di++)
            for (int dj = 0; dj < 2; dj++)
                for (int dk = 0; dk < 2; dk++)
                {
                    c[di][dj][dk] = randvec[
                        perm_x[(i + di) & 255] ^
                            perm_y[(j + dj) & 255] ^
                            perm_z[(k + dk) & 255]
                    ];
                }

        return PerlinInterp(c, u, v, w);
    }

private:
    static const int pointCount = 256;
    Vec3 randvec[pointCount];
    int perm_x[pointCount];
    int perm_y[pointCount];
    int perm_z[pointCount];

    static void PerlinGeneratePerm(int* p)
    {
        for (int i = 0; i < pointCount; i++)
            p[i] = i;

        Permute(p, pointCount);
    }

    static void Permute(int* p, int n)
    {
        for (int i = n - 1; i > 0; i--)
        {
            const int target = Random::Int(0, i);
            std::swap(p[i], p[target]);
        }
    }

    static double PerlinInterp(const Vec3 c[2][2][2], double u, double v, double w)
    {
        const double  uu = u * u * (3 - 2 * u);
        const double  vv = v * v * (3 - 2 * v);
        const double  ww = w * w * (3 - 2 * w);
        double accum = 0.0;

        for (int i = 0; i < 2; i++)
            for (int j = 0; j < 2; j++)
                for (int k = 0; k < 2; k++)
                {
                    const Vec3 weight_v(u - i, v - j, w - k);
                    accum += (i * uu + (1 - i) * (1 - uu))
                        * (j * vv + (1 - j) * (1 - vv))
                        * (k * ww + (1 - k) * (1 - ww))
                        * Dot(c[i][j][k], weight_v);
                }

        return accum;
    }
};