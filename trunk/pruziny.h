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


#ifndef _PRUZINY_
#define _PRUZINY_

#include "matika.h"

struct cpruzina{
  double TVRDOST;
  double TLUMENI;
  double DELKA;                  // delka pri ktere je pruzina v ustalenem stavu (nepusobi zadnou silou na koncove body)
  vektor3D pruzina_sila;        // celkova sila, ktera pusobi na bod1 (na bod2 pusobi ta sama, ale opacnym smerem)
  bool exponencialni;           // jestli je vysledna sila zavisla na stlaceni pruziny linearne nebo exponencialne
  cpruzina(double _tvrdost, double _tlumeni, double _delka);
  cpruzina(double _tvrdost, double _tlumeni, double _delka, bool exponencialni);
  void PruzinaCyklus(pozice3D x1, pozice3D x2, vektor3D v1, vektor3D v2);
};

#endif
