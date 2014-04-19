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


#include "vstup.h"
#include "matika.h"
#include "teren.h"
#include "chodec.h"

const double VYSKA = 1.5;           // vyska v m
const double RYCHLOST = 30.0;       // max rychlost v km/h

tchodec::tchodec(keys *kl, teren *ter, double *p_pozice, float azimut) : objekt(p_pozice, azimut){
  uhelxy = 0.0;
  uhelz = 0.0;
  CELKOVA_HMOTNOST = 75.0;
  M_NA_MINUS_PRVNI = 1.0f / CELKOVA_HMOTNOST;
  klavesy = kl;
  t = ter;
  kopni_pole(p_pozice, pozice, 3);
  MatMaticeBaze(matice_rotace);

  // create bounding sphere hierarchy
  pozice3D center;
  center[0] = 0.0f;
  center[1] = 0.0f;
  center[2] = -0.5f; // 2m high hierarchy but only 1.5m high person

  obal_data obalData;

  obal_koule root;
  root.ID = 0;
  root.polomer = 1.0f;
  root.rodic = -1;
  MatPoziceKopni(center, root.pozice);
  obalData.hierarchie_obalu.push_back(root);

  obal_koule sub1;
  sub1.ID = 1;
  sub1.polomer = 0.5f;
  sub1.rodic = 0;
  MatPoziceKopni(center, sub1.pozice);
  sub1.pozice[2] += 0.5f;
  obalData.hierarchie_obalu.push_back(sub1);

  obal_koule sub2;
  sub2.ID = 2;
  sub2.polomer = 0.5f;
  sub2.rodic = 0;
  MatPoziceKopni(center, sub2.pozice);
  sub2.pozice[2] -= 0.5f;
  obalData.hierarchie_obalu.push_back(sub2);

  float height = center[2] + 0.75f;
  for (int i = 0; i < 4; i++)
  {
    obal_koule leaf;
    leaf.polomer = 0.25f;
    leaf.ID = 3 + i;
    leaf.rodic = i < 2 ? 1 : 2;
    float h = height - (float)i * 0.5f;
    leaf.pozice[0] = 0.0f;
    leaf.pozice[1] = 0.0f;
    leaf.pozice[2] = h;
    obalData.hierarchie_obalu.push_back(leaf);
  }

  InitObal(&obalData);
}


void tchodec::cyklus(bool aktivni, pozice3D _pozice, matice mat){
  if (!aktivni){
    MatPoziceKopni(_pozice, pozice);
    MatMaticeKopni(mat, matice_rotace);
    vektor3D d = {1.0, 0.0, 0.0};
    vektor3D dopredu;
    MatMaticeXVektor(matice_rotace, d, dopredu);
    uhelz = 0.0;
    dopredu[Z] = 0.0;
    uhelxy = MatVektorUhel(d, dopredu);
    if (dopredu[Y] < 0.0) uhelxy *= -1.0;
    uhelxy -= PI/2.0;
    return;
  }

  // klavesy
  bool keyup = false;
  bool keydown = false;
  bool keyleft = false;
  bool keyright = false;
  if ((klavesy->up) && (!klavesy->down)) keyup = true;
  if ((!klavesy->up) && (klavesy->down)) keydown = true;
  if ((klavesy->left) && (!klavesy->right)) keyleft = true;
  if ((!klavesy->left) && (klavesy->right)) keyright = true;

  if ((klavesy->key['w']) && (!klavesy->key['s'])) keyup = true;
  if ((!klavesy->key['w']) && (klavesy->key['s'])) keydown = true;
  if ((klavesy->key['a']) && (!klavesy->key['d'])) keyleft = true;
  if ((!klavesy->key['a']) && (klavesy->key['d'])) keyright = true;

  // pohyb
  vektor3D temp;
  MatVektorSmaz(temp);
  if (keyup) temp[X] = 1.0;
  if (keydown) temp[X] = -1.0;
  if (keyleft) temp[Y] = 1.0;
  if (keyright) temp[Y] = -1.0;
  MatVektorJednot(temp);

  // uhly
  uhelxy -= klavesy->delta_uhelxy;
  uhelz += klavesy->delta_uhelz;
  while (uhelxy > 2.0 * PI) uhelxy -= 2.0 * PI;
  while (uhelz > 2.0 * PI) uhelz -= 2.0 * PI;
  while (uhelxy < 0.0) uhelxy += 2.0 * PI;
  while (uhelz < 0.0) uhelz += 2.0 * PI;
  MatMaticeBaze(matice_rotace);
  MatMaticeRotaceY(matice_rotace, uhelz);
  MatMaticeRotaceZ(matice_rotace, uhelxy);

  float run = 1.0f;
  if (klavesy->shift)
    run = 2.0f;

  static int waittime = 0;
  if (waittime == 0 && klavesy->key['o'])
  {
    kresleni_obalu++;
    waittime = 20;
  }
  if (kresleni_obalu > 2) kresleni_obalu = 0;
  if (waittime > 0) waittime--;

  // rotace
  MatMaticeXVektor(matice_rotace, temp, smer);
  smer[Z] = 0.0;
  MatVektorJednot(smer);


  // vypocet pozice
  pozice[0] = pozice[0] + lin_sila.getX() * M_NA_MINUS_PRVNI * KROK * 0.01f + run * smer[X] * KROK * RYCHLOST / 3.6;
  pozice[1] = pozice[1] + lin_sila.getY() * M_NA_MINUS_PRVNI * KROK * 0.01f + run * smer[Y] * KROK * RYCHLOST / 3.6;
  pozice[2] = t->Z(pozice[0], pozice[1]) + VYSKA;

  matice matRot;
  matRot[0] = smer[0];
  matRot[1] = smer[1];
  matRot[2] = 0.0f;
  matRot[3] = -smer[1];
  matRot[4] = smer[0];
  matRot[5] = 0.0f;
  matRot[6] = 0.0f;
  matRot[7] = 0.0f;
  matRot[8] = 1.0f;

  lin_sila = Vec3D(0,0,0);

  NastavObal(matRot, pozice);
}





double *tchodec::Pozice(){return pozice;}
double tchodec::Spd(){return 0.0;}
double tchodec::Uxy(){return 0.0;}



