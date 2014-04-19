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



#include "suspension.h"
#include <math.h>

#include <iostream>
using namespace std;

const double TVRDOST_DELKA = 150000.0;      // tvrdost pruziny, kdyz dojde k prekroceni delky

suspension::suspension(double tvrdost, double tlumeni_odskok, double tlumeni_naraz, double delka){
    TVRDOST = tvrdost;
    TLUMENI_ODSKOK = tlumeni_odskok;
    TLUMENI_NARAZ = tlumeni_naraz;
    DELKA = delka;
    sila = 0.0;
}


void suspension::vypocetSily(double x, double v){
    double sila_pruziny;// = - x * TVRDOST;
    double sila_tlumeni = 0.0;

    if (fabs(x) > DELKA){
        sila_pruziny = -x * TVRDOST_DELKA;
    } else sila_pruziny = -x * TVRDOST;
/*
    if (x > 0.0){   // naraz (bump)
        double sign = 1.0;
        if (v < 0.0) sign = -1.0;
        sila_tlumeni = - sign * sqrtf(fabs(v)) * TLUMENI_NARAZ;
    } else {        // odskok (rebound)
         sila_tlumeni = - v * TLUMENI_ODSKOK;
    }
*/

    sila_tlumeni = - v * TLUMENI_ODSKOK;

    sila = sila_pruziny + sila_tlumeni;
}

