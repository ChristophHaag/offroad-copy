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


#include "struktury.h"
#include "loaddata.h"

const double STROM_VYSKA = 15.0;
const double STROM_DELTA_VYSKA= 5.0;
const double STROM_SIRKA = 15.0;
const double STROM_DELTA_SIRKA= 5.0;


//------------------------------------------------------------------------------
//                                   STROM
//------------------------------------------------------------------------------


CStrom::CStrom(pozice3D _pozice, GLuint DL_z, GLuint DL_s, GLuint DL_p){
  MatPoziceKopni(_pozice, pozice);
  uhel = 0.0;//random(PI/2.0);
  vyska = STROM_VYSKA - (STROM_DELTA_VYSKA/2.0) + random(STROM_DELTA_VYSKA);
  sirka = STROM_SIRKA - (STROM_DELTA_SIRKA/2.0) + random(STROM_DELTA_SIRKA);
  DL_stred = DL_s;
  DL_zadni = DL_z;
  DL_predni = DL_p;
  opacne = false;

  // obal
  const int pocet = 12;
  obal_koule koule[pocet];
  obal_data data;
  pozice3D p;
  MatPoziceSmaz(p);
  matice R;
  MatMaticeBaze(R);


  // hlavni koule
  koule[0].ID = 0;
  koule[0].rodic = -1;
  koule[0].polomer = vyska/2.0;
  p[Z] = vyska / 2.0;
  MatPoziceKopni(p, koule[0].pozice);

  // horni koule
  koule[1].ID = 1;
  koule[1].rodic = 0;
  koule[1].polomer = vyska/3.0;
  p[Z] = vyska - vyska / 3.0;
  MatPoziceKopni(p, koule[1].pozice);

  // dolni koule
  koule[2].ID = 2;
  koule[2].rodic = 0;
  koule[2].polomer = vyska/3.0;
  p[Z] = vyska / 3.0;
  MatPoziceKopni(p, koule[2].pozice);

  // potomci dolni
  for (int i = 0; i < 4; i++)
  {
    koule[3+i].ID = 3+i;
    koule[3+i].rodic = 2;
    koule[3+i].polomer = vyska/10.0;
    p[Z] = (0.1+0.1*i)*vyska;
    MatPoziceKopni(p, koule[i+3].pozice);
  }

  // potomci horni
  for (int i = 0; i < 5; i++)
  {
    koule[7+i].ID = 7+i;
    koule[7+i].rodic = 1;
    koule[7+i].polomer = vyska/10.0;
    p[Z] = (0.5+0.1*i)*vyska;
    MatPoziceKopni(p, koule[i+7].pozice);
  }
  for (int i = 0; i < pocet; i++)
    data.hierarchie_obalu.push_back(koule[i]);
  InitObal(&data);
  NastavObal(R, _pozice);
}


void CStrom::Cyklus(pozice3D pozice_kamery){
  if (pozice_kamery[Y] > pozice[Y]) opacne = true;
    else opacne = false;
}

void CStrom::Kresli(){
  //KresliObal(true);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glDisable(GL_CULL_FACE);
  glEnable(GL_TEXTURE_2D);
  glPushMatrix();
  glTranslatef(pozice[X], pozice[Y], pozice[Z]);
  glRotatef(MatRadianStupen(uhel), 0.0,0.0,1.0);
  glScalef(sirka, sirka, vyska);
  if (!opacne) glCallList(DL_zadni);
    else glCallList(DL_predni);
  glCallList(DL_stred);
  if (!opacne) glCallList(DL_predni);
    else glCallList(DL_zadni);
  glPopMatrix();
  glEnable(GL_CULL_FACE);
  glDisable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);
}









