#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <cmath>
#include <iostream>
#include <random>
#include <SFML/Graphics.hpp>

using Vector3f = sf::Vector3f;
using Vector2f = sf::Vector2f;
using Vector2i = sf::Vector2i;
using Vector2u = sf::Vector2u;
using FloatRect = sf::FloatRect;

inline std::ostream &operator<<(std::ostream &os, const Vector3f &v)
{
    os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}

float vecDotProd(const Vector3f &a, const Vector3f &b);
void vecNormalize(Vector3f &v);
float vecMagnitude(const Vector3f &v);
float vecMagnitude2(const Vector3f &v);
float vecDistance(const Vector3f &a, const Vector3f &b);
float vecDistance2(const Vector3f &a, const Vector3f &b);

bool intersetPlane(const Vector3f &n, const Vector3f &p0,
                   const Vector3f &l0, const Vector3f &l,
                   Vector3f *i);

inline void srand(int seed)
{
    std::srand(seed);
}

inline float randf()
{
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

inline int randi(int start, int end)
{
    float v = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    return std::round((float)start + (v * (float)(end - start)));
}

#endif // __VECTOR_H__