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


#ifndef _OBJEKTY_
#define _OBJEKTY_

#include <vector>
#include "chodec.h"
#include "obal.h"
#include "objekt.h"
#include "auto.h"

const int nic_aktivni = 99;

struct objekty{
  bool prvni_cyklus;
  tchodec *chodec;
  objekt *prostredek[99];
  keys *klavesy;            // ukazatel na objekt pro obsluhu vstupu
  teren *t;                 // ukazatel na teren
  int aktivni;              // pozice v poli
  bool pesky;               // zadny aktivni objekt
  void nastup();            // nastoupi/vystoupi
  void kolize();
  void tlumeni();
  pozice3D pozice_kamery;
  std::vector<DrawObject> drawObjects;

  int St();                // vrati zarazeny stupen aktualniho objektu
  double Spd();             // vrati rychlost aktualniho objektu
  double Uz();
  double Un();
  double Ot();              // vrati otacky aktualniho objektu
  double Alt();             // vrati vysku aktualniho objektu
  double Deltaz();
  double Plyn();
  double *Pozice();         // vrati pozici aktualniho objektu
  float Zataceni();
  double *Pozice_ridice();  // vrati pozici ridice aktualniho objektu
  double *Pozice_vystupu(); // kam ridic vystoupi
  bool Cam();              // moznost pohledu zezadu
  objekty(int akt_p, teren *ter, keys *kl);
  void cyklus(pozice3D kamera);
  void kresli(const Vec3D& camPos);
  void Matice(matice mat);
  int PocetObjektu();
  void PrepareForDrawing();  // set drawing variables, m-safe ensuded from outside
  void DeInit();
};

#endif
