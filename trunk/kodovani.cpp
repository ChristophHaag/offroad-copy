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



#include "kodovani.h"
#include <math.h>
using namespace std;

int abs(int cislo){
  if (cislo >= 0) return cislo;
  return - cislo;

}

void zakodujf(float cislo, unsigned char *byty){
  byty[1] = (int)fabs(cislo);                                       // celociselna cast
  byty[0] = (int)((fabs(cislo) - (float)floor(fabs(cislo))) * 100.0);     // desetinna cast
  if (cislo < 0.0) byty[0] += 128;                                  // znamenko
}

void zakoduji(int cislo, unsigned char *byty){
  byty[0] = (abs(cislo))&0x000000FF;
  byty[1] = ((abs(cislo))&0x0000FF00)>>8;
}

unsigned char zakoduj2i(int cislo1, int cislo2){
  return (((abs(cislo1))&0x0000000F) + (((abs(cislo2))&0x0000000F) << 4));
}



float dekodujf(unsigned char horni, unsigned char dolni){
  return (((float)horni + (float(dolni&0x7F))/100.0) * ((float)(1 - ((dolni&0x80)>>6))));
}

int dekoduji(unsigned char horni, unsigned char dolni){
  return (((int)horni)<<8) + (int)dolni;
}

void dekoduj2i(unsigned char byt, int *cisla){
  cisla[0] = (byt&0x0F);
  cisla[1] = ((byt&0xF0)>>4);
}
