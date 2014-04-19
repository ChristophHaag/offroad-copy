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


#include "koule.h"
#include <iostream>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>


barva3 CERVENA = {1.0, 0.2, 0.1};
barva3 BILA = {0.8, 0.8, 0.8};


// KONSTRUKTOR
CKoule::CKoule(int ID, vektor3D pozice_obj, double polomer)
{
  this->ID = ID;
  this->polomer = polomer;
  MatVektorKopni(pozice_obj, this->pozice_obj);
  dalsi = NULL;
  obsah = NULL;
}

// KRESLI
void CKoule::Kresli(bool listy) const
{
  if ((listy) && (obsah != NULL)) return;
  glPushMatrix();
  glEnable(GL_COLOR_MATERIAL);
  glDisable(GL_TEXTURE_2D);
  glColor3dv(barva);
  glTranslatef(pozice[X], pozice[Y], pozice[Z]);
//  glutWireSphere(polomer, 20, 20);

  GLUquadricObj* quadric;
  quadric = gluNewQuadric();
  gluQuadricDrawStyle(quadric, GLU_LINE);
  gluSphere(quadric, polomer, 16, 16);
  gluDeleteQuadric(quadric);

  glPopMatrix();
}

// JeTam
bool CKoule::JeTam(pozice3D bod)
{
  double leva_strana = pow(pozice[X]-bod[X], 2) + pow(pozice[Y]-bod[Y],2) + pow(pozice[Z]-bod[Z],2);
  double prava_strana = polomer*polomer;
  if (leva_strana < prava_strana) return true;
  return false;
}

// Kolize
bool CKoule::Kolize(CKoule *koule2, pozice3D bod1, pozice3D bod2, bool &list)
{
  bool kolize = false;
  list = false;
  pozice3D bod;
  koule2->DejPozici(bod);
  double leva_strana = pow(pozice[X]-bod[X],2) + pow(pozice[Y]-bod[Y],2) + pow(pozice[Z]-bod[Z],2);
  double prava_strana = pow(polomer + koule2->DejPolomer(), 2);
  if (leva_strana < prava_strana) kolize = true;
  if ((kolize) && (koule2->DejObsah() == NULL) && (obsah == NULL))
  {
    list = true;
    pozice3D smer_12;
    pozice3D smer_21;
    NastavBarvu();
    koule2->NastavBarvu();
    MatPoziceVektor(bod, pozice, smer_12);
    MatVektorJednot(smer_12);
    MatVektorOpak(smer_12, smer_21);
    MatVektorXSkalar(smer_12, polomer);
    MatVektorXSkalar(smer_21, koule2->DejPolomer());
    MatPozicePlusVektor(pozice, smer_12, bod1);
    MatPozicePlusVektor(koule2->pozice, smer_21, bod2);
  }
  return kolize;
}

// NASTAVBARVU
void CKoule::NastavBarvu()
{
  MatVektorKopni(CERVENA, barva);
}

// DEJPOZICI
void CKoule::DejPozici(pozice3D pozice)
{
  MatPoziceKopni(this->pozice, pozice);
}

// DEJPOLOMER
double CKoule::DejPolomer()
{
  return polomer;
}

// DEJDALSI
CKoule *CKoule::DejDalsi()
{
  return dalsi;
}

// DEJOBSAH
CKoule *CKoule::DejObsah()
{
  return obsah;
}

// NastavPozici
void CKoule::NastavPozici(matice R, pozice3D pozice_objektu)
{
  MatVektorKopni(BILA, barva);
  MatMaticeXVektor(R, pozice_obj, pozice);
  MatVektorPridej(pozice_objektu, pozice);
}

// NastavDalsi
void CKoule::NastavDalsi(CKoule *jina_dalsi)
{
  dalsi = jina_dalsi;
}

// PridejObsah
void CKoule::PridejObsah(CKoule *nova_koule)
{
  nova_koule->NastavDalsi(obsah);
  obsah = nova_koule;
}

// DejID
int CKoule::DejID()
{
  return ID;
}
