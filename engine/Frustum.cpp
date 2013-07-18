#include "DXUT.h"
#include "Frustum.h"

std::vector<Vec3> Frustum::getPointsFromMatrix(const Matrix& invViewProj)
{
    std::vector<Vec3> points;
    points.resize(8);

    points[0] = Vec3(-1, 1, 0);
    points[1] = Vec3(1, 1, 0);
    points[2] = Vec3(-1, -1, 0);
    points[3] = Vec3(1, -1, 0);
    points[4] = Vec3(-1, 1, 1);
    points[5] = Vec3(1, 1, 1);
    points[6] = Vec3(-1, -1, 1);
    points[7] = Vec3(1, -1, 1);

    D3DXVec3TransformCoordArray(&points[0], sizeof(Vec3), &points[0], sizeof(Vec3), &invViewProj, points.size());
    return points;
}