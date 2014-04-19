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


#ifndef _STRUKTURY_
#define _STRUKTURY_
#include "matika.h"
#include "obal.h"

class CStrom:public CObal
{
  pozice3D pozice;
  double uhel;        // uhel natoceni stromu
  double vyska;
  double sirka;
  bool opacne;      // jestli je strom otoceny o 180 stupnu
  GLuint DL_zadni;  // display list zadni casti stromu
  GLuint DL_stred;
  GLuint DL_predni;
 public:
  CStrom(pozice3D _pozice, GLuint DL_z, GLuint DL_s, GLuint DL_p);
  void Cyklus(pozice3D pozice_kamery);
  void Kresli();
};




















#endif

