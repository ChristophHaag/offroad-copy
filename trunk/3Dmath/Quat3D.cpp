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


#include "Quat3D.h"
#include "Mat3D.h"
#include "Vec3D.h"
#include <math.h>






Quat3D::Quat3D(){
    clear();
}


Quat3D::Quat3D(const Quat3D& q){
    x = q.x;
    y = q.y;
    z = q.z;
    w = q.w;
}

Quat3D::Quat3D(const double x, const double y, const double z, const double w){
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
}


void Quat3D::clear(){
    x = 0.0;
    y = 0.0;
    z = 0.0;
    w = 1.0;
}

void Quat3D::operator =(const Quat3D& q){
    x = q.x;
    y = q.y;
    z = q.z;
    w = q.w;
}

Mat3D Quat3D::computeMatrix(){
  double l2 = x*x + y*y + z*z + w*w;
  if (l2 < 0.9999 || l2 > 1.0001) {
    double n = (l2 + 1.0) / (2.0 * l2);
    x *= n;
    y *= n;
    z *= n;
    w *= n;
    l2 *= powf(n, 2);
  }

  double x2, y2, z2, xy, wz, xz, wy, yz, wx;

  x2 = x * x;
  y2 = y * y;
  z2 = z * z;
  xy = x * y;
  wz = w * z;
  xz = x * z;
  wy = w * y;
  yz = y * z;
  wx = w * x;


  Vec3D e1, e2, e3;
  double vel = 2.0 / l2;

  e1.setX(1.0 - vel * (y2 + z2));
  e1.setY(vel * (xy - wz));
  e1.setZ(vel * (xz + wy));

  e2.setX(vel * (xy + wz));
  e2.setY(1.0 - vel * (z2 + x2));
  e2.setZ(vel * (yz - wx));

  e3.setX(vel * (xz - wy));
  e3.setY(vel * (yz + wx));
  e3.setZ(1.0 - vel * (x2 + y2));

  return Mat3D(e1, e2, e3).transpose();
}


double Quat3D::length() const{
    return sqrt(x * x + y * y + z * z + w * w);
}


void Quat3D::normalize(){
    double l = 1.0 / length();
    x *= l;
    y *= l;
    z *= l;
    w *= l;
}

void Quat3D::Set(const Vec3D& axis, float angle)
{
  w = cos(angle * 0.5f);
  x = axis.getX() * sin(angle * 0.5f);
  y = axis.getY() * sin(angle * 0.5f);
  z = axis.getZ() * sin(angle * 0.5f);
}

void Quat3D::rotate(const Vec3D& rot, double step){
    Quat3D q = Quat3D(*this);
    w -= step * ( q.x * rot.getX() + q.y * rot.getY() + q.z * rot.getZ());
    x += step * (q.w * rot.getX() - q.z * rot.getY() + q.y * rot.getZ());
    y += step * (q.z * rot.getX() + q.w * rot.getY() - q.x * rot.getZ());
    z += step * (q.x * rot.getY() - q.y * rot.getX() + q.w * rot.getZ());
}

void Quat3D::Set(const Vec3D& rotation)
{
  double cosHeading = cos(rotation.getZ()*0.5f);
  double sinHeading = sin(rotation.getZ()*0.5f);
  double cosPitch = cos(rotation.getY()*0.5f);
  double sinPitch = sin(rotation.getY()*0.5f);
  double cosRoll = cos(rotation.getX()*0.5f);
  double sinRoll = sin(rotation.getX()*0.5f);

  w = cosRoll * cosPitch * cosHeading + sinRoll * sinPitch * sinHeading;
  x = sinRoll * cosPitch * cosHeading - cosRoll * sinPitch * sinHeading;
  y = cosRoll * sinPitch * cosHeading + sinRoll * cosPitch * sinHeading;
  z = cosRoll * cosPitch * sinHeading - sinRoll * sinPitch * cosHeading;
}





