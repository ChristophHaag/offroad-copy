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


#ifndef FRUSTUM_H
#define FRUSTUM_H

#include "3Dmath/Mat3D.h"
#include "3Dmath/Vec3D.h"
#include "3Dmath/plane3d.h"

class Frustum
{
    Vec3D _position;
    Vec3D _corners[4];
    Vec3D _cornersWorld[4];

    Vec3D _dirs[4];

    Plane3D _planes[5];   // without near plane

    void RecalculatePlanes(Vec3D position, Vec3D* corners);

    float _zFar;
    float _angle;
    float _aspectRatio;

  public:
    bool IsIn(const Vec3D& point) const;  // returns if given point is inside frustum
    bool IsIn(const Vec3D& center, float radius) const; // returns if given bounding sphere is inside frustum

    void DisplayChanged(float zFar, float angle, float aspectRatio);

    void Step(const Vec3D& position, const Mat3D& orientation);
    Vec3D Position() const {return _position;}

    const Vec3D& GetDir(int i) const {return _dirs[i];}

    Frustum();
};

#endif // FRUSTUM_H
