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
 * File:   Antirollbar.h
 * Author: milan
 *
 * Created on 10. říjen 2009, 13:32
 */

#ifndef _ANTIROLLBAR_H
#define	_ANTIROLLBAR_H

#include "wheel.h"

// antirollbar zabranuje prevraceni auta tim, ze kdyz jde levy tlumic nahoru, tak pusobi smerem nahoru i na pravy tlumic.


class Antirollbar {
    double TVRDOST;
    wheel *leve_kolo;
    wheel *prave_kolo;
  public:
    void init(double tvrdost, wheel* leve_kolo, wheel* prave_kolo);
    Antirollbar();
    void cyklus();
private:

};

#endif	/* _ANTIROLLBAR_H */


