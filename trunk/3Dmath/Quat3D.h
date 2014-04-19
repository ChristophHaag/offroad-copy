/*
 * File:   Quat3D.h
 * Author: milan
 *
 * Created on 1. březen 2009, 18:37
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

#ifndef _QUAT3D_H
#define	_QUAT3D_H


#include "Mat3D.h"

class Quat3D{
    double x;
    double y;
    double z;
    double w;

    public:

    Quat3D();
    Quat3D(const Quat3D& q);
    Quat3D(const double x, const double y, const double z, const double w);

    void clear();

    void operator=(const Quat3D& q);
    Mat3D computeMatrix();

    void rotate(const Vec3D& rot, double step);

    void Set(const Vec3D& axis, float angle);
    void Set(const Vec3D& rotation);

    double X() const {return x;}
    double Y() const {return y;}
    double Z() const {return z;}
    double W() const {return w;}

    double length() const;
    void normalize();


};





#endif	/* _QUAT3D_H */

