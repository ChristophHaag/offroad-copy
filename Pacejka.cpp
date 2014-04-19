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



#include "Pacejka.h"
#include <math.h>

#include <iostream>
using namespace std;

const double MIN_V = 10.0;      // rychlost auta, pri ktere zacne naplno pusobit pacejka (v m/s)


Vec3D Pacejka::sila(double tlak, double slipRatio, double slipAngle, double x, double y, double surfaceFriction){
    const double TEREN = surfaceFriction;

    Fz = tlak / 1000.0;                     // Fz musi byt v kN
    if (Fz < 0.0) Fz = 0.0;
    slipPercentage = slipRatio;
    sideSlip = -slipAngle * 180.0 / M_PI;

    //cout << "slip: " << slipPercentage << endl;
    double fx = SimpleFx();
    double fy = SimpleFy();

    //cout << "fx: " << fx << endl;
    //cout << "fy: " << fy << endl << endl;

    fy = fy * sqrtf(1.0 - (fx*fx));

    fx = fx * Fz * TEREN;
    fy = fy * Fz * TEREN * 0.7f;

    return Vec3D(fx, fy, 0.0);
}




Pacejka::Pacejka(double Ax, double Bx, double Px, double Ay, double By, double Py){
    this->Ax = Ax;
    this->Bx = Bx;
    this->Px = Px;

    this->Ay = Ay;
    this->By = By;
    this->Py = Py;
}




double Pacejka::SimpleFy(){
    double fy;
    fy = (By * sideSlip) / (1.0 + powf(fabs(Ay * sideSlip), Py));
    return fy;
}



double Pacejka::SimpleFx(){
    double fx;
    fx = (Bx * slipPercentage) / (1.0 + powf(fabs(Ax * slipPercentage), Px));
    return fx;
}
