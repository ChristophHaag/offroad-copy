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



#ifndef _CHODEC_
#define _CHODEC_
#include "vstup.h"
#include "teren.h"
#include "objekt.h"
class tchodec : public objekt
{
  keys *klavesy;
  teren *t;
  double uhelxy;
  double uhelz;
  pozice3D pozice;
  vektor3D smer;
 public:
  matice matice_rotace;
  tchodec(keys *kl, teren *ter, double *p_pozice, float azimut);
  void cyklus(bool aktivni, pozice3D _pozice, matice mat);
  double Spd();
  double Uxy();
  double *Pozice();
};


#endif

