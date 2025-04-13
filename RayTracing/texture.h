#pragma once

class Texture
{
public:
    virtual ~Texture() = default;

    virtual Color Value(double u, double v, const Point3& p) const = 0;
};

class SolidColor : public Texture
{
public:
    SolidColor(const Color& albedo) : albedo(albedo)
    {}

    SolidColor(double red, double green, double blue)
        : SolidColor(Color(red, green, blue))
    {}

    Color Value(double u, double v, const Point3& p) const override
    {
        return albedo;
    }

private:
    Color albedo;
};

class CheckerTexture : public Texture
{
public:
    CheckerTexture(double scale, shared_ptr<Texture> even, shared_ptr<Texture> odd)
        : invScale(1.0 / scale)
        , even(even)
        , odd(odd)
    {}

    CheckerTexture(double scale, const Color& c1, const Color& c2)
        : CheckerTexture(scale, make_shared<SolidColor>(c1), make_shared<SolidColor>(c2))
    {}

    Color Value(double u, double v, const Point3& p) const override
    {
        const int xInteger = int(std::floor(invScale * p.x()));
        const int yInteger = int(std::floor(invScale * p.y()));
        const int zInteger = int(std::floor(invScale * p.z()));

        const bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;

        return isEven ? even->Value(u, v, p) : odd->Value(u, v, p);
    }

private:
    double invScale;
    shared_ptr<Texture> even;
    shared_ptr<Texture> odd;
};