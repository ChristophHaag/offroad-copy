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


#include "Mat3D.h"

#include <iostream>
#include <stdlib.h>

Mat3D::Mat3D(){
    clear();
}

Mat3D::Mat3D(const Mat3D& m){
    e1 = m.e1;
    e2 = m.e2;
    e3 = m.e3;
}

Mat3D::Mat3D(const Vec3D& e1, const Vec3D& e2, const Vec3D& e3){
    this->e1 = e1;
    this->e2 = e2;
    this->e3 = e3;
}

Mat3D::Mat3D(const double* array){
    e1 = Vec3D(array);
    e2 = Vec3D(array+3);
    e3 = Vec3D(array+6);
}

void Mat3D::clear(){
    e1.clear();
    e2.clear();
    e3.clear();
    e1.setX(1.0);
    e2.setY(1.0);
    e3.setZ(1.0);
}


void Mat3D::operator =(const Mat3D& m){
    e1 = m.e1;
    e2 = m.e2;
    e3 = m.e3;
}

Mat3D Mat3D::operator *(const double m) const{
    return Mat3D(
            e1 * m,
            e2 * m,
            e3 * m);
}

void Mat3D::operator *=(const double m){
        e1 *= m;
        e2 *= m;
        e3 *= m;
}


Vec3D Mat3D::operator *(const Vec3D& v) const{
    Mat3D temp = transpose();
    return Vec3D(v.dot(temp.e1), v.dot(temp.e2), v.dot(temp.e3));
}


Mat3D Mat3D::operator *(const Mat3D& m) const{
    Mat3D temp = m.transpose();
    Vec3D v1, v2, v3;
    v1.setX(e1.dot(temp.e1));
    v1.setY(e1.dot(temp.e2));
    v1.setZ(e1.dot(temp.e3));
    v2.setX(e2.dot(temp.e1));
    v2.setY(e2.dot(temp.e2));
    v2.setZ(e2.dot(temp.e3));
    v3.setX(e3.dot(temp.e1));
    v3.setY(e3.dot(temp.e2));
    v3.setZ(e3.dot(temp.e3));
    return Mat3D(v1, v2, v3);
}

void Mat3D::operator *=(const Mat3D& m){
    Mat3D temp = m.transpose();
    Vec3D v1, v2, v3;
    v1.setX(e1.dot(temp.e1));
    v1.setY(e1.dot(temp.e2));
    v1.setZ(e1.dot(temp.e3));
    v2.setX(e2.dot(temp.e1));
    v2.setY(e2.dot(temp.e2));
    v2.setZ(e2.dot(temp.e3));
    v3.setX(e3.dot(temp.e1));
    v3.setY(e3.dot(temp.e2));
    v3.setZ(e3.dot(temp.e3));
    e1 = v1;
    e2 = v2;
    e3 = v3;
}

Mat3D Mat3D::operator +(const Mat3D& m) const{
    return Mat3D(e1 + m.e1, e2 + m.e2, e3 + m.e3);
}

void Mat3D::operator +=(const Mat3D& m){
    e1 += m.e1;
    e2 += m.e2;
    e3 += m.e3;
}

Mat3D Mat3D::operator -() const{
    return Mat3D(-e1, -e2, -e3);
}


void Mat3D::convertToOpenGL(double* m) const
{
    m[0] = e1.getX();
    m[1] = e1.getY();
    m[2] = e1.getZ();
    m[3] = 0.0;

    m[4] = e2.getX();
    m[5] = e2.getY();
    m[6] = e2.getZ();
    m[7] = 0.0;

    m[8] = e3.getX();
    m[9] = e3.getY();
    m[10] = e3.getZ();
    m[11] = 0.0;

    m[12] = 0.0;
    m[13] = 0.0;
    m[14] = 0.0;
    m[15] = 1.0;
}



Mat3D Mat3D::transpose() const{
    return Mat3D(
            Vec3D(e1.getX(), e2.getX(), e3.getX()),
            Vec3D(e1.getY(), e2.getY(), e3.getY()),
            Vec3D(e1.getZ(), e2.getZ(), e3.getZ()));

}

Mat3D Mat3D::invert() const{
    double determinant = det();
    if (singular()){
        std::cout << "Error: Cannot invert singular matrix!" << std::endl;
        exit(1);
    }

    determinant = 1.0 / determinant;

    Vec3D ie1, ie2, ie3;

    ie1.setX(+(e2.getY() * e3.getZ() - e3.getY() * e2.getZ()));
    ie1.setY(-(e1.getY() * e3.getZ() - e3.getY() * e1.getZ()));
    ie1.setZ(+(e1.getY() * e2.getZ() - e2.getY() * e1.getZ()));

    ie2.setX(-(e2.getX() * e3.getZ() - e3.getX() * e2.getZ()));
    ie2.setY(+(e1.getX() * e3.getZ() - e3.getX() * e1.getZ()));
    ie2.setZ(-(e1.getX() * e2.getZ() - e2.getX() * e1.getZ()));

    ie3.setX(+(e2.getX() * e3.getY() - e3.getX() * e2.getY()));
    ie3.setY(-(e1.getX() * e3.getY() - e3.getX() * e1.getY()));
    ie3.setZ(+(e1.getX() * e2.getY() - e2.getX() * e1.getY()));

    ie1 *= determinant;
    ie2 *= determinant;
    ie3 *= determinant;

    return Mat3D(ie1, ie2, ie3);
}


double Mat3D::det() const{
    return  e1.getX() * e2.getY() * e3.getZ() +
            e1.getY() * e2.getZ() * e3.getX() +
            e1.getZ() * e2.getX() * e3.getY() -
            e1.getZ() * e2.getY() * e3.getX() -
            e1.getY() * e2.getX() * e3.getZ() -
            e1.getX() * e2.getZ() * e3.getY();
}

bool Mat3D::singular() const{
    const double ZERO = 0.000001;
    double determinant = det();
    if ((determinant < ZERO) && (determinant > - ZERO)) return true;
    return false;
}

void Mat3D::rotateX(const double angle){
    e1 = e1.rotateX(angle);
    e2 = e2.rotateX(angle);
    e3 = e3.rotateX(angle);
}

void Mat3D::rotateY(const double angle){
    e1 = e1.rotateY(angle);
    e2 = e2.rotateY(angle);
    e3 = e3.rotateY(angle);
}

void Mat3D::rotateZ(const double angle){
    e1 = e1.rotateZ(angle);
    e2 = e2.rotateZ(angle);
    e3 = e3.rotateZ(angle);
}


void Mat3D::print() const{
    e1.print();
    e2.print();
    e3.print();
}


void Mat3D::toArray(double* mat) const
{
    mat[0] = e1.getX();
    mat[1] = e1.getY();
    mat[2] = e1.getZ();
    mat[3] = e2.getX();
    mat[4] = e2.getY();
    mat[5] = e2.getZ();
    mat[6] = e3.getX();
    mat[7] = e3.getY();
    mat[8] = e3.getZ();
}
