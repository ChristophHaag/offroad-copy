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



#include "Vec3D.h"
#include <iostream>
#include <math.h>


//  -========================================================-
//                      *** Vec3D ***
//  -========================================================-

// constructors
Vec3D::Vec3D(){
    clear();
}


Vec3D::Vec3D(const double* array){
    x = array[X];
    y = array[Y];
    z = array[Z];
}

Vec3D::Vec3D(const Vec3D& v){
    x = v.getX();
    y = v.getY();
    z = v.getZ();
}

Vec3D::Vec3D(double x, double y, double z){
    this->x = x;
    this->y = y;
    this->z = z;
}

// operators
void Vec3D::operator =(const Vec3D& v){
    x = v.getX();
    y = v.getY();
    z = v.getZ();
}

Vec3D Vec3D::operator +(const Vec3D& v2)const {
    return Vec3D(
            x + v2.getX(),
            y + v2.getY(),
            z + v2.getZ());
}

Vec3D Vec3D::operator -(const Vec3D& v2)const {
    return Vec3D(
            x - v2.getX(),
            y - v2.getY(),
            z - v2.getZ());
}

Vec3D Vec3D::operator -() const{
    return Vec3D(-x, -y, -z);
}

Vec3D Vec3D::operator *(const double m)const {
    return Vec3D(
            x * m,
            y * m,
            z * m);
}


Vec3D Vec3D::operator *(const Vec3D& v2)const {
    return Vec3D(x * v2.getX(), y * v2.getY(),z * v2.getZ());
}


void Vec3D::operator *=(const double m){
    x = x * m;
    y = y * m;
    z = z * m;
}

void Vec3D::operator +=(const Vec3D& v2){
    x = x + v2.getX();
    y = y + v2.getY();
    z = z + v2.getZ();
}

void Vec3D::operator -=(const Vec3D& v2){
    x = x - v2.getX();
    y = y - v2.getY();
    z = z - v2.getZ();
}

// other
void Vec3D::print() const{
    std::cout << "(" << x << ", " << y << ", " << z << ")" << std::endl;
}


double Vec3D::distance(const Vec3D& v2) const{
    return sqrt((x - v2.x) * (x - v2.x) + (y - v2.y) * (y - v2.y) + (z - v2.z) * (z - v2.z));
}

double Vec3D::distance2(const Vec3D& v2) const{
    return (x - v2.x) * (x - v2.x) + (y - v2.y) * (y - v2.y) + (z - v2.z) * (z - v2.z);
}

double Vec3D::distance2D(const Vec3D &v2) const
{
    return sqrt(Power(v2.getX() - x) + Power(v2.getY() - y));
}

double Vec3D::distance2D2(const Vec3D &v2) const
{
    return Power(v2.x - x) + Power(v2.y - y);
}

Vec3D Vec3D::center(const Vec3D& v2) const{
    return Vec3D((x + v2.getX()) / 2.0, (y + v2.getY()) / 2.0, (z + v2.getZ()) / 2.0);

}

double Vec3D::length() const{
    return sqrt(x*x + y*y + z*z);
}

double Vec3D::length2D() const
{
  return sqrt(x*x + y*y);
}


#define ZERO 0.00000001
Vec3D& Vec3D::normalize(){
   double l = length();
   if (l < ZERO) return *this;
   x = x / l;
   y = y / l;
   z = z / l;
   return *this;
}

Vec3D Vec3D::cross(const Vec3D& v2) const{
    return Vec3D(
            y * v2.z - z * v2.y,
            z * v2.x - x * v2.z,
            x * v2.y - y * v2.x);
}

double Vec3D::dot(const Vec3D& v2) const{
    return x * v2.x + y * v2.y + z * v2.z;
}


double Vec3D::angle(const Vec3D& v2) const{
  double dot = (*this).dot(v2);
  double l1 = length();
  double l2 = v2.length();
  double result = 0.0;
  if ((l1 == 0.0) || (l2 == 0.0)) return 0.0;
  double temp = dot / (l1 * l2);
  if (temp >= 1.0) return 0.0;
  if (temp <= -1.0) return M_PI;
  result = acos(temp);
  if (result > M_PI) return M_PI;
  if (result < -M_PI) return -M_PI;
  return result;
}

Vec3D Vec3D::rotateX(const double angle) const{
    return Vec3D(
            x,
            y * cos(angle) - z * sin(angle),
            y * sin(angle) + z * cos(angle));
}

double Vec3D::angle2D(const Vec3D& v2) const
{
  double angle = v2.angle2D() - angle2D();

  if (angle > M_PI)
    angle -= 2.0f * M_PI;

  if (angle < - M_PI)
    angle += 2.0f * M_PI;

  return angle;
}

double Vec3D::angle2D() const
{
  if (fabs(y) < ZERO)
  {
    if (x >= 0.0f)
      return 0.0f;
    else
      return M_PI;
  }

  if (fabs(x) < ZERO)
  {
    if (y > 0.0f)
      return M_PI * 0.5f;
    else
      return M_PI + M_PI * 0.5f;
  }

  double angle = atan(fabs(y/x));

  // first quadrant
  if (x > 0.0f && y > 0.0f)
    return angle;

  // second quadrant
  if (x < 0.0f && y > 0.0f)
    return M_PI - angle;

  // third quadrant
  if (x < 0.0f && y < 0.0f)
    return M_PI + angle;

  // fourth quadrant
  if (x > 0.0f && y < 0.0f)
    return 2.0f * M_PI - angle;

  return 0.0f;
}

Vec3D Vec3D::rotateY(const double angle) const{
    return Vec3D(
            x * cos(angle) + z * sin(angle),
            y,
            -x * sin(angle) + z * cos(angle));
}

Vec3D Vec3D::rotateZ(const double angle) const{
    return Vec3D(
            x * cos(angle) - y * sin(angle),
            x * sin(angle) + y * cos(angle),
            z);
}

void Vec3D::clear(){
    x = 0.0;
    y = 0.0;
    z = 0.0;
}

double& Vec3D::operator[](int i)
{
  return *(&x + i);
}

double Vec3D::operator[](int i) const
{
  return *(&x + i);
}


void Vec3D::toArray(double* v) const
{
  v[X] = x;
  v[Y] = y;
  v[Z] = z;
}

