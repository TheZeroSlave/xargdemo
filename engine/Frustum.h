#pragma once
#include <vector>
#include "types.h"

class Frustum
{
public:
    static std::vector<Vec3> getPointsFromMatrix(const Matrix& ivViewProj);
};