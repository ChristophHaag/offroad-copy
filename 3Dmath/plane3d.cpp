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

#include "plane3d.h"
#include <math.h>


Plane3D::Plane3D()
{
  _plane[0] = 0.0f;
  _plane[1] = 0.0f;
  _plane[2] = 1.0f;
  _plane[3] = 0.0f;
}

Plane3D::Plane3D(const Plane3D& other)
{
  *this = other;
}


void Plane3D::operator=(const Plane3D& v)
{
  for (int i = 0; i < 4; i++)
    _plane[i] = v._plane[i];
}


float& Plane3D::operator[](int i)
{
  return _plane[i];
}


void Plane3D::Set(const Vec3D& normal, const Vec3D& point)
{
  _plane[0] = normal.getX();
  _plane[1] = normal.getY();
  _plane[2] = normal.getZ();
  _plane[3] = - point.dot(normal);
}


void Plane3D::Set(const Vec3D& v1, const Vec3D& v2, const Vec3D& point)
{
  Vec3D normal = v1.cross(v2);
  Set(normal, point);
}

void Plane3D::SetPoints(const Vec3D& p1, const Vec3D& p2, const Vec3D& p3)
{
  Vec3D v1 = p2 - p1;
  Vec3D v2 = p3 - p2;
  v1.normalize();
  v2.normalize();
  Set(v1, v2, p1);
}

float Plane3D::Distance(const Vec3D& point) const
{
  float dst = _plane[3];
  Vec3D p = point;
  for (int i = 0; i < 3; i++)
  {
    dst += _plane[i] * p[i];
  }
  return dst;
}

float Plane3D::Distance2(const Vec3D& point) const
{
  float dst = Distance(point);
  return dst*dst;
}

float Plane3D::operator*(const Vec3D& vec) const
{
  return _plane[0] * vec.getX() + _plane[1] * vec.getY() + _plane[2] * vec.getZ();
}

bool Plane3D::Intersection(Vec3D& result, const Vec3D& lineStart, const Vec3D& lineDir, float maxDist) const
{
  float num = (*this) * lineStart + _plane[3];
  float denom = (*this) * lineDir;

  if (fabs(denom) < 0.0001f) return false;  // line is paralel with plane

  float t = - num/denom;

  if (maxDist > 0.0f && t > maxDist) return false;  // intersection is too far
  result = lineStart + lineDir * t;
  return true;
}

