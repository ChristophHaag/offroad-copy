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



#include "frustum.h"
#include <math.h>
#include <stdio.h>

Frustum::Frustum()
{
  _zFar = 360.0f;
  _angle = 50.0f;
  _aspectRatio = 1.6f;

  DisplayChanged(_zFar, _angle, _aspectRatio);

  RecalculatePlanes(_position, _corners);
}

void Frustum::RecalculatePlanes(Vec3D position, Vec3D* corners)
{
  // zfar
  _planes[0].SetPoints(corners[0], corners[1], corners[2]);

  // bottom
  _planes[1].SetPoints(position, corners[1], corners[0]);

  // right
  _planes[2].SetPoints(position, corners[2], corners[1]);

  // top
  _planes[3].SetPoints(position, corners[3], corners[2]);

  // left
  _planes[4].SetPoints(position, corners[0], corners[3]);
}


bool Frustum::IsIn(const Vec3D& point) const
{
  for (int i = 0; i < 5; i++)
  {
    if (_planes[i].Distance(point) < 0.0f) return false;
  }
  return true;
}


bool Frustum::IsIn(const Vec3D& center, float radius) const
{
  for (int i = 0; i < 5; i++)
  {
    if (_planes[i].Distance(center) + radius < 0.0f) return false;
  }
  return true;
}

void Frustum::DisplayChanged(float zFar, float angle, float aspectRatio)
{
  _zFar = zFar*0.995f;
  _angle = angle;
  _aspectRatio = aspectRatio;

  float tanHalfAngle = tanf(0.5f * _angle * M_PI / 180.0f);
  float hWidth = tanHalfAngle * _zFar * (_aspectRatio);
  float hHeight = tanHalfAngle * _zFar;
  _corners[0] = Vec3D(_zFar, hWidth, -hHeight);
  _corners[1] = Vec3D(_zFar, -hWidth, -hHeight);
  _corners[2] = Vec3D(_zFar, -hWidth, hHeight);
  _corners[3] = Vec3D(_zFar, hWidth, hHeight);
}

void Frustum::Step(const Vec3D& position, const Mat3D& orientation)
{
  _position = position;

  for (int i = 0; i < 4; i++)
  {
    _cornersWorld[i] = _position + orientation * _corners[i];
    _dirs[i] = orientation * _corners[i];
  }
  RecalculatePlanes(_position, _cornersWorld);
}

