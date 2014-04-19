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
 * File:   Pacejka.h
 * Author: milan
 *
 * Created on 13. říjen 2009, 18:17
 */

#ifndef _PACEJKA_H
#define	_PACEJKA_H

#include "3Dmath/Vec3D.h"

class Pacejka{
    // zjednoduseny model
    double Ay, By, Py;
    double Ax, Bx, Px;

    double Fz;
    double slipPercentage;
    double sideSlip;

    double SimpleFy();
    double SimpleFx();

public:
    Pacejka(double Ax, double Bx, double Px, double Ay, double By, double Py);
    Vec3D sila(double tlak, double slipRatio, double slipAngle, double x, double y, double surfaceFriction);
};


#endif	/* _PACEJKA_H */

