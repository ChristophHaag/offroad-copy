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


/*
 * File:   Mat3D.h
 * Author: Milan Timko
 * email: milantimko@seznam.cz
 * Created on 28. únor 2009, 14:38
 */

#ifndef _MAT3D_H
#define	_MAT3D_H

#include "Vec3D.h"

class Mat3D{
    private:
    Vec3D e1;
    Vec3D e2;
    Vec3D e3;

    public:

    Mat3D();
    Mat3D(const Vec3D& e1, const Vec3D& e2, const Vec3D& e3);
    Mat3D(const Mat3D& m);
    Mat3D(const double* array);

    void clear();
    Mat3D transpose() const;
    Mat3D invert() const;
    bool singular() const;
    double det() const;

    void rotateX(const double angle);
    void rotateY(const double angle);
    void rotateZ(const double angle);

    void print() const;

    void convertToOpenGL(double* mat) const;

    void operator=(const Mat3D& m);
    Mat3D operator*(const double m) const;
    Mat3D operator*(const Mat3D& m) const;
    Vec3D operator*(const Vec3D& v) const;

    void operator*=(const double m);
    void operator*=(const Mat3D& m);

    Mat3D operator+(const Mat3D& m) const;
    void operator+=(const Mat3D& m);

    Mat3D operator-() const;

    const Vec3D& getE1() const {return e1;}
    const Vec3D& getE2() const {return e2;}
    const Vec3D& getE3() const {return e3;}

    //double* toArray();
    void toArray(double* mat) const;


};




#endif	/* _MAT3D_H */

