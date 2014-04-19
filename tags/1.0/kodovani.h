//
//      *******   Copyright *********
//      ***   Milan Timko  2005   ***
//      *****************************
//
//      Slouzi k zakodovani/dekodovani INTu a FLOATu na 16bitovou velikost.
//
//      FLOAT:   8 bitu na nezaporne cislo pred desetinnou carkou
//               1 bit na znamenko
//               7 bitu na cislo za desetinnou carkou o rozsahu 0-99.
//
//      INT:     8 bitu na 15. - 8. bit integeru
//               8 bitu na 7. - 0. bit integeru
//               Vyssi bity se oriznou.
//
//      2INT     Prvni int na spodni pulku bytu a druhy na horni
//

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


void zakodujf(float cislo, unsigned char *byty);
void zakoduji(int cislo, unsigned char *byty);
unsigned char zakoduj2i(int cislo1, int cislo2);



float dekodujf(unsigned char horni, unsigned char dolni);
int dekoduji(unsigned char horni, unsigned char dolni);
void dekoduj2i(unsigned char byt, int *cisla);
