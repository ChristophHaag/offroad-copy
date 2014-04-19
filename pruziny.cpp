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



#include "pruziny.h"
#include "matika.h"

#include <iostream>
using namespace std;


cpruzina::cpruzina(double _tvrdost, double _tlumeni, double _delka){
  TVRDOST = _tvrdost;
  TLUMENI = _tlumeni;
  DELKA = _delka;
  MatVektorSmaz(pruzina_sila);
  exponencialni = false;
}

cpruzina::cpruzina(double _tvrdost, double _tlumeni, double _delka, bool exponencialni){
  TVRDOST = _tvrdost;
  TLUMENI = _tlumeni;
  DELKA = _delka;
  MatVektorSmaz(pruzina_sila);
  this->exponencialni = exponencialni;
}


void cpruzina::PruzinaCyklus(pozice3D x1, pozice3D x2, vektor3D v1, vektor3D v2){
  // pouzite promenne
  vektor3D vektor_BA;
  vektor3D smer_BA;
  double vzd;
  double sila_pruziny;
  vektor3D sila_tlumeni;
  vektor3D rozdil_rychlosti;

  // vypocet promennych
  for (int i = 0; i < 3; i++){
    vektor_BA[i] = x1[i] - x2[i];
    rozdil_rychlosti[i] = v1[i] - v2[i];
  }
  MatVektorJednot(vektor_BA, smer_BA);
  vzd = MatVektorVelikost(vektor_BA);

  // vypocet sily pruziny
  //sila_pruziny = TVRDOST * (vzd - DELKA);
  double k = vzd / DELKA;   // koeficient roztazeni (0-1+)    1 - rovnovazny stav
  if (k <0.0) k = 0.0;      // osetreni zapornych hodnot
  if (k >2.0) k = 2.0;      // osetreni velkych hodnot
  k = 1.0 - k;
  if (exponencialni) {
    sila_pruziny = - k*k * TVRDOST;
    cout << sila_pruziny << endl;
  } else {
    sila_pruziny = TVRDOST * (vzd - DELKA);
  }




  // vypocet sily tlumeni
  for (int i = 0; i < 3; i++)
    sila_tlumeni[i] = TLUMENI * rozdil_rychlosti[i] * smer_BA[i];

  // vypocet celkove sily
  for (int i = 0; i < 3; i++)
    pruzina_sila[i] = - (sila_pruziny * smer_BA[i] + sila_tlumeni[i] * smer_BA[i]);
}
