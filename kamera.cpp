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
#include "kamera.h"
#include "teren.h"

const int wait = 10;
const int NUM5 = 53;
const int NUMP = 43;
const int NUMM = 45;


const double MAX_VZD = 30.0;
const double MIN_VZD = 3.0;
const double KROK_VZD = 0.5;
const double DEF_VZD = 10.0;
const double MAX_SKLON = MatStupenRadian(50.0);


kamera::kamera(keys *klav, teren *ter){
  SMER[X] = -10.0;
  SMER[Y] = 0.0;
  SMER[Z] = 3.0;
  MatVektorJednot(SMER);
  vzd = DEF_VZD;
  uhelx = 0.0;
  uhely = 0.0;
  MatMaticeSmaz(matice_rotace);
  MatPoziceSmaz(pozice1);
  MatPoziceSmaz(pozice2);
  klavesy = klav;
  wait_time = 0;
  fp = false;
  nahoru[X] = 0.0;
  nahoru[Y] = 0.0;
  nahoru[Z] = 1.0;
  t = ter;
  zatoceni = 0.0f;
}



void kamera::cyklus(double *npozice, matice mat, float zataceni, bool cam){

  if ((klavesy->cameraReset) || (klavesy->middle_button)){
    uhelx = 0.0;
    uhely = 0.0;
    vzd = DEF_VZD;
  }

  zatoceni += (zataceni - zatoceni) * 0.05f;

  if (klavesy->left_button || klavesy->zoomIn) vzd -= KROK_VZD;
  if (klavesy->right_button || klavesy->zoomOut) vzd += KROK_VZD;
  if ((klavesy->key['c']) && (wait_time == 0)){
    if (fp) fp = false;
      else fp = true;
    wait_time = 50;
  }
  if (wait_time > 0) wait_time--;

  nahoru[X] = 0.0;
  nahoru[Y] = 0.0;
  nahoru[Z] = 1.0;

  MatVektorKopni(npozice, pozice2);
  MatPoziceKopni(npozice, pozice1);
  MatMaticeKopni(mat, matice_rotace);

  if ((!fp) && (cam)){
    if (vzd > MAX_VZD) vzd = MAX_VZD;
    if (vzd < MIN_VZD) vzd = MIN_VZD;
    MatVektorKopni(SMER, smer);
    MatVektorXSkalar(smer, vzd);
    uhelx += klavesy->delta_uhelxy;
    uhely += klavesy->delta_uhelz;
    while (uhelx > 2.0 * PI) uhelx -= 2.0 * PI;
    while (uhelx < 0.0) uhelx += 2.0 * PI;
    if (uhely > MAX_SKLON) uhely = MAX_SKLON;
    if (uhely < -MAX_SKLON) uhely = -MAX_SKLON;

    MatVektorRotace(smer, uhely, Y);
    MatVektorRotace(smer, uhelx + zatoceni, Z);
    vektor3D s;
    MatMaticeXVektor(matice_rotace, smer, s);
    s[Z] = smer[Z];
    MatVektorPridej(s, pozice2);
    double z = t->Z(pozice2[X], pozice2[Y]);
    if (z > pozice2[Z]) pozice2[Z] = z;


  } else {
    vektor3D smerfp = {1.0, 0.0, 0.0};
    vektor3D s;
    MatMaticeXVektor(matice_rotace, smerfp, s);
    MatVektorPridej(s, pozice1);

    vektor3D n;
    MatVektorKopni(nahoru, n);
    MatMaticeXVektor(matice_rotace, n, nahoru);
    zatoceni = 0.0f;
  }

  Vec3D dirF = Vec3D(pozice1) - Vec3D(pozice2);
  dirF.normalize();
  Vec3D dirUp(up);
  Vec3D dirSide = dirUp.cross(dirF);
  orient = Mat3D(dirF, dirSide, dirUp);
}

void kamera::PrepareForDrawing()
{
  pos1 = Vec3D(pozice1);
  pos2 = Vec3D(pozice2);
  up = Vec3D(nahoru);
}

void kamera::kresli(){

  gluLookAt(pos2.getX(),pos2.getY(),pos2.getZ(),
            pos1.getX(),pos1.getY(),pos1.getZ(),
            up.getX(),up.getY(),up.getZ());

}
