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



#ifndef _KAMERA_
#define _KAMERA_


#include "teren.h"
#include "3Dmath/Vec3D.h"
#include "3Dmath/Mat3D.h"

class kamera{
  vektor3D SMER;
  vektor3D smer;

  matice matice_rotace;
  double pozice1[3];      // stred
  double pozice2[3];      // kamera

  // variables used just for drawing
  Vec3D pos1;
  Vec3D pos2;
  Vec3D up;

  double uhelx;
  double uhely;
  double vzd;
  bool fp;               // first-person pohled
  keys *klavesy;
  int wait_time;
  vektor3D nahoru;
  Mat3D orient;
  teren *t;
  float zatoceni;
 public:
  void kresli();
  void PrepareForDrawing();  // set drawing variable, m-safe ensuded from outside
  void cyklus(pozice3D _pozice, matice mat, float zataceni, bool cam);
  kamera(keys *klav, teren *ter);
  double *Pozice(){return pozice2;}
  const Mat3D& Orientation() const {return orient;}
};

#endif
