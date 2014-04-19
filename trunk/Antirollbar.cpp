/* 
 * File:   Antirollbar.cpp
 * Author: milan
 * 
 * Created on 10. říjen 2009, 13:32
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


#include "Antirollbar.h"

Antirollbar::Antirollbar(){
    leve_kolo = NULL;
    prave_kolo = NULL;
    TVRDOST = 0.0;
}


void Antirollbar::init(double tvrdost, wheel* leve_kolo, wheel* prave_kolo) {
    TVRDOST = tvrdost;
    this->leve_kolo = leve_kolo;
    this->prave_kolo = prave_kolo;
}

void Antirollbar::cyklus(){
    if ((leve_kolo == NULL) || (prave_kolo == NULL)) return;
    double delta = leve_kolo->dejVychylku() - prave_kolo->dejVychylku();
    double sila = delta * TVRDOST;
    leve_kolo->pridejSilu(-sila);
    prave_kolo->pridejSilu(sila);
}
