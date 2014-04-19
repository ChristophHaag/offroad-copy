/*
Offroad - 2008 Edition
Copyright (C) 2014  Milan Timko
http://www.milantimko.info

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/gpl-3.0.txt>.
*/

#ifndef PLANE3D_H
#define PLANE3D_H
#include "Vec3D.h"

class Plane3D
{
  float _plane[4];

  public:
    Plane3D();
    Plane3D(const Plane3D& other);

    void operator=(const Plane3D& v);
    float& operator[](int i);

    void Set(const Vec3D& normal, const Vec3D& point);
    void Set(const Vec3D& v1, const Vec3D& v2, const Vec3D& point);
    void SetPoints(const Vec3D& p1, const Vec3D& p2, const Vec3D& p3);

    float Distance(const Vec3D& point) const;   // distance of given point from plane
    float Distance2(const Vec3D& point) const;

    bool Intersection(Vec3D& result, const Vec3D& lineStart, const Vec3D& lineDir, float maxDist = -1.0f) const;

    float operator*(const Vec3D& vec) const;
};

#endif // PLANE3D_H
