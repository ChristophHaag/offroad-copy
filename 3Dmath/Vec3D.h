/*
 * File:   Vec3D.h
 * Author: Milan Timko
 * email: milantimko@seznam.cz
 * Created on 27. únor 2009, 15:47
 */

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

#ifndef _XYZW_
#define _XYZW_
const int X = 0;
const int Y = 1;
const int Z = 2;
const int W = 3;
#endif


#ifndef _VEC3D_H
#define	_VEC3D_H

#define Power(x) ((x) * (x))

#define ToRadians(x) ((x) * M_PI / 180.0f)
#define ToDegrees(x) ((x) * 180.0f * M_PI)


class Vec3D {
    private:
        double x;
        double y;
        double z;

    public:
        Vec3D();
        Vec3D(const double* array);
        Vec3D(double x, double y, double z);
        Vec3D(const Vec3D &v);

        void operator=(const Vec3D &v);
        Vec3D operator+(const Vec3D &v2)const;
        Vec3D operator-(const Vec3D &v2)const;
        Vec3D operator-()const;
        Vec3D operator*(const double m)const;
        Vec3D operator*(const Vec3D &v2)const;
        void operator+=(const Vec3D &v2);
        void operator-=(const Vec3D &v2);
        void operator*=(const double m);

        void clear();
        Vec3D& normalize();
        double length() const;
        double length2D() const;
        void print() const;
        Vec3D cross(const Vec3D& v2) const;
        double dot(const Vec3D& v2) const;
        double distance(const Vec3D &v2) const;
        double distance2(const Vec3D &v2) const;
        double distance2D(const Vec3D &v2) const;
        double distance2D2(const Vec3D &v2) const;
        Vec3D center(const Vec3D& v2) const;

        double angle(const Vec3D& v2) const;
        double angle2D() const;    // angle on Z 2d plane (returns nubmer in range from 0 to 2*PI)
        double angle2D(const Vec3D& v2) const; // angle between two vectors on 2D plane (returns number between -PI to PI)
        Vec3D rotateX(const double angle) const;
        Vec3D rotateY(const double angle) const;
        Vec3D rotateZ(const double angle) const;

        //double* toArray() const;
        void toArray(double* v) const;

        // get
        double inline getX() const{return x;}
        double inline getY() const{return y;}
        double inline getZ() const{return z;}
        // set
        void inline setX(const double x) {this->x = x;}
        void inline setY(const double y) {this->y = y;}
        void inline setZ(const double z) {this->z = z;}
        void inline set(const double x, const double y, const double z) {this->x = x; this->y = y; this->z = z;}

        double& operator[](int i);
        double operator[](int i) const;
};







#endif	/* _VEC3D_H */

