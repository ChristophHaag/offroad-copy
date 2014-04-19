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
 * File:   suspension.h
 * Author: milan
 *
 * Created on 16. červenec 2009, 10:45
 */

#ifndef _SUSPENSION_H
#define	_SUSPENSION_H

#include "3Dmath/Vec3D.h"
#include "3Dmath/Mat3D.h"

class suspension{
    double TVRDOST;
    double TLUMENI_ODSKOK;
    double TLUMENI_NARAZ;
    double DELKA;

    double sila;     // sila, kterou pruzina pusobi

public:
    suspension(double tvrdost, double tlumeni_odskok, double tlumeni_naraz, double delka);
    double dejSilu() const {return sila;}
    void vypocetSily(double x, double v);
};


#endif	/* _SUSPENSION_H */

