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


#include "tgaload.h"
#include "hud.h"
#include "matika.h"

double pozice_rych[2] = {20.0,80.0};
double pozice_vyska[2] = {20.0,580.0};
double pozice_koule[2] = {300.0, 20.0};
double pozice_stoupa[2] = {500.0, 20.0};
const double pozice_otac[2] = {650.0,150.0};
const double pozice_plyn[2] = {30.0, 500.0};


double rych_pryc[2] = {-80.0, -20.0};
double vyska_pryc[2] = {-80.0, 680.0};
double koule_pryc[2] = {300.0, -80.0};
double stoupa_pryc[2] = {500.0, -80.0};
const double otac_pryc[2] = {750.0, 50.0};
const double plyn_pryc[2] = {-80.0, 500.0};

const double rych_mezera = -5.0;
const double rych_sirka = 48.0;
const double rych_vyska = 64.0;
const double otac_rozmer = 128.0;
const double plyn_vyska = 400.0;
const double plyn_sirka = 50.0;
const double koule_velikost = 100.0;
const double stoupa_velikost = 100.0;
const double krok = 4.0;





//   ************************
//   ***   KONSTRUKTORY   ***
//   ************************



trychlost::trychlost(bool enable, double *pozice_zap,  double *pozice_vyp, int _typ){
  rych = 0.0;
  typ = _typ;
  cislo[0] = 0;
  cislo[1] = 0;
  cislo[2] = 0;
  kopni_pole(pozice_vyp, pozice_v, 2);
  kopni_pole(pozice_zap, pozice_z, 2);
  if (enable) {
    zobraz = true;
    kopni_pole(pozice_z, pozice,2);
  } else {
    zobraz = false;
    kopni_pole(pozice_v, pozice,2);
  }
}


totacky::totacky(bool enable){
  st = 0;
  ot = 0.0;
  if (enable) {
    zobraz = true;
    pozice[0] = pozice_otac[0];
    pozice[1] = pozice_otac[1];
  } else {
    zobraz = false;
    pozice[0] = otac_pryc[0];
    pozice[1] = otac_pryc[1];
  }
}

tplyn::tplyn(bool enable){
  plyn = 0;
  if (enable)
  {
    zobraz = true;
    pozice[0] = pozice_plyn[0];
    pozice[1] = pozice_plyn[1];
  }
  else
  {
    zobraz = false;
    pozice[0] = plyn_pryc[0];
    pozice[1] = plyn_pryc[1];
  }
}

tkoule::tkoule(bool enable){
  uheln = 0.0;
  uhelz = 0.0;
  kopni_pole(pozice_koule, pozice_z, 2);
  kopni_pole(koule_pryc, pozice_v, 2);
  if (enable)
  {
    zobraz = true;
    kopni_pole(pozice_z, pozice, 2);
  }
  else
  {
    zobraz = false;
    kopni_pole(pozice_v, pozice, 2);
  }
}


tstoupani::tstoupani(bool enable){
  uhel = 0.0;
  kopni_pole(pozice_stoupa, pozice_z, 2);
  kopni_pole(stoupa_pryc, pozice_v, 2);
  if (enable)
  {
    zobraz = true;
    kopni_pole(pozice_z, pozice, 2);
  }
  else
  {
    zobraz = false;
    kopni_pole(pozice_v, pozice, 2);
  }
}



hud::hud(bool enable){
  rychlost = new trychlost(enable, pozice_rych, rych_pryc, 0);
  otacky = new totacky(enable);
//  plyn = new tplyn(enable);
//  vyska = new trychlost(enable, pozice_vyska, vyska_pryc, 1);
//  koule = new tkoule(enable);
//  stoupani = new tstoupani(enable);
  zobraz = enable;
  text_fps[0] = 'F';
  text_fps[1] = 'P';
  text_fps[2] = 'S';
  text_fps[3] = ':';
  text_fps[4] = ' ';
}

hud::~hud()
{
  delete rychlost;
  delete otacky;
//  delete plyn;
//  delete vyska;
//  delete koule;
//  delete stoupani;
}







//   ************************
//   ***     KRESLENI     ***
//   ************************



void trychlost::kresli(GLuint *textury){
  if (!zobraz) return;
  glColor4f(0.5,0.5,0.5,0.5);
  glBindTexture ( GL_TEXTURE_2D, textury[0]);
  glBegin(GL_QUADS);

  double x = pozice[0];
  double y = pozice[1];
  double txy[2];




  // rychlost
  for (int i = 0; i < 3; i++){
    znak(cislo[i], txy);

    // levy dolni
    glTexCoord2f(txy[0], txy[1]);
    glVertex2f(x, y - rych_vyska);

    // pravy dolni
    glTexCoord2f(txy[0] + 0.25, txy[1]);
    glVertex2f(x + rych_sirka, y - rych_vyska);

    // pravy horni
    glTexCoord2f(txy[0] + 0.25, txy[1] + 0.25);
    glVertex2f(x + rych_sirka, y);

    // pravy horni
    glTexCoord2f(txy[0], txy[1] + 0.25);
    glVertex2f(x, y);

    x = x + rych_sirka + rych_mezera;
  }
  // KMH
  if (typ == 0){
  y = y - 32.0;
  for (int i = 0; i < 3; i++){
      znak(13+i, txy);
       // levy dolni
      glTexCoord2f(txy[0], txy[1]);
      glVertex2f(x, y - rych_vyska/2.0);

      // pravy dolni
      glTexCoord2f(txy[0] + 0.25, txy[1]);
      glVertex2f(x + rych_sirka, y - rych_vyska/2.0);

      // pravy horni
      glTexCoord2f(txy[0] + 0.25, txy[1] + 0.25);
      glVertex2f(x + rych_sirka, y);

      // pravy horni
      glTexCoord2f(txy[0], txy[1] + 0.25);
      glVertex2f(x, y);

      x = x + rych_sirka + -20.0;
    }
  }
  if (typ == 1){
    znak(14, txy);
    // levy dolni
    glTexCoord2f(txy[0], txy[1]);
    glVertex2f(x, y - rych_vyska/2.0);

    // pravy dolni
    glTexCoord2f(txy[0] + 0.25, txy[1]);
    glVertex2f(x + rych_sirka, y - rych_vyska/2.0);

    // pravy horni
    glTexCoord2f(txy[0] + 0.25, txy[1] + 0.25);
    glVertex2f(x + rych_sirka, y);

    // pravy horni
    glTexCoord2f(txy[0], txy[1] + 0.25);
    glVertex2f(x, y);
  }
  glEnd();
}


void totacky::kresli(GLuint *textury){
  if (!zobraz) return;
  glColor4f(1.0,1.0,1.0,0.7);
  glBindTexture ( GL_TEXTURE_2D, textury[1]);
  glBegin(GL_QUADS);
    glTexCoord2f(0.0,0.0);
    glVertex2f(pozice[0],pozice[1]-otac_rozmer);
    glTexCoord2f(1.0,0.0);
    glVertex2f(pozice[0]+otac_rozmer,pozice[1]-otac_rozmer);
    glTexCoord2f(1.0,1.0);
    glVertex2f(pozice[0]+otac_rozmer,pozice[1]);
    glTexCoord2f(0.0,1.0);
    glVertex2f(pozice[0],pozice[1]);
  glEnd();

 // stupen
  glColor4f(0.5,0.5,0.5,0.5);
  glBindTexture ( GL_TEXTURE_2D, textury[0]);
  glBegin(GL_QUADS);
    double txy[2];

    if (st > 0) znak(st, txy);
    if (st == 0) znak(11, txy);
    if (st == -1) znak(12, txy);

    // levy dolni
    glTexCoord2f(txy[0], txy[1]);
    glVertex2f(pozice[0] + 30.0, pozice[1] - 110.0);

    // pravy dolni
    glTexCoord2f(txy[0] + 0.25, txy[1]);
    glVertex2f(pozice[0] + 70.0, pozice[1] - 110.0);

    // pravy horni
    glTexCoord2f(txy[0] + 0.25, txy[1] + 0.25);
    glVertex2f(pozice[0] + 70.0, pozice[1] - 60.0);

    // pravy horni
    glTexCoord2f(txy[0], txy[1] + 0.25);
    glVertex2f(pozice[0] + 30.0, pozice[1] - 60.0);

  glEnd();



  // rucicka
  glColor4f(1.0,0.3,0.3,0.5);
  double p[2] = {pozice[0] + 100.0, pozice[1] - 100.0};
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);
  glPushMatrix();

  glTranslatef(p[0], p[1], 0.0);
  glRotatef(-uhel,0.0,0.0,1.0);
  glBegin(GL_QUADS);
    glVertex2f(10.0, 0.0);
    glVertex2f(0.0, 10.0);
    glVertex2f(-100.0, 0.0);
    glVertex2f(0.0, - 10.0);
  glEnd();
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glPopMatrix();
}


void tplyn::kresli(GLuint *textury){
  if (zobraz)
  {
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glColor3f(0.0,0.0,0.0);
    glBegin(GL_LINE_LOOP);
      glVertex2f(pozice[0] - 5, pozice[1] + 5);
      glVertex2f(pozice[0] + 5 + plyn_sirka, pozice[1] + 5);
      glVertex2f(pozice[0] + 5 + plyn_sirka, pozice[1] - 5 - plyn_vyska);
      glVertex2f(pozice[0] - 5, pozice[1] - 5 - plyn_vyska);
    glEnd();

    glEnable(GL_BLEND);
    glColor4f(1.0,1.0 - plyn,0.0,0.4);
    glBegin(GL_QUADS);
      glVertex2f(pozice[0], pozice[1] - plyn_vyska);
      glVertex2f(pozice[0] + plyn_sirka, pozice[1] - plyn_vyska);
      glVertex2f(pozice[0] + plyn_sirka, pozice[1] - (1-plyn)*plyn_vyska);
      glVertex2f(pozice[0], pozice[1] - (1-plyn)*plyn_vyska);
    glEnd();
    glEnable(GL_TEXTURE_2D);


  }
}

void tkoule::kresli(GLuint *textury){
  if (!zobraz) return;
  glColor4f(1.0,1.0,1.0, 1.0);
  glPushMatrix();

  // textury
  glMatrixMode(GL_TEXTURE);
  glPushMatrix();
  glTranslatef(0.0, uhelz/5.0, 0.0);
  glRotatef(-stupne(uheln), 0.0,0.0,1.0);
  glMatrixMode(GL_MODELVIEW);

  glBindTexture(GL_TEXTURE_2D, textury[3]);
  glBegin(GL_QUADS);
    //hl
    glTexCoord2f(-0.1,0.1);
    glVertex2f(pozice[0], pozice[1]);
    //hp
    glTexCoord2f(0.1,0.1);
    glVertex2f(pozice[0] + koule_velikost, pozice[1]);
    //dp
    glTexCoord2f(0.1,-0.1);
    glVertex2f(pozice[0] + koule_velikost, pozice[1]+ koule_velikost);
    //dl
    glTexCoord2f(-0.1,-0.1);
    glVertex2f(pozice[0], pozice[1]+ koule_velikost);
  glEnd();

  glColor3f(0.0,0.0,0.0);
  glDisable(GL_BLEND);
  glBegin(GL_LINES);
    glVertex2f(pozice[0], pozice[1] + koule_velikost / 2.0);
    glVertex2f(pozice[0] + koule_velikost, pozice[1] + koule_velikost / 2.0);
  glEnd();
  glBegin(GL_LINE_LOOP);
    glVertex2f(pozice[0], pozice[1]);
    glVertex2f(pozice[0] + koule_velikost, pozice[1]);
    glVertex2f(pozice[0] + koule_velikost, pozice[1] + koule_velikost);
    glVertex2f(pozice[0], pozice[1] + koule_velikost);
  glEnd();
  // textury
  glMatrixMode(GL_TEXTURE);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);

  glPopMatrix();
  glEnable(GL_BLEND);
}


void tstoupani::kresli(GLuint *textury){
  if (!zobraz) return;
  glColor4f(1.0,1.0,1.0, 1.0);
  glPushMatrix();
  glBindTexture(GL_TEXTURE_2D, textury[2]);

  glBegin(GL_QUADS);
    //hl
    glTexCoord2f(0.0,1.0);
    glVertex2f(pozice[0], pozice[1]);
    //hp
    glTexCoord2f(1.0,1.0);
    glVertex2f(pozice[0] + stoupa_velikost, pozice[1]);
    //dp
    glTexCoord2f(1.0,0.0);
    glVertex2f(pozice[0] + stoupa_velikost, pozice[1]+ stoupa_velikost);
    //dl
    glTexCoord2f(0.0,0.0);
    glVertex2f(pozice[0], pozice[1]+ stoupa_velikost);
  glEnd();


  // rucicka
  glColor4f(1.0,0.3,0.3,0.5);
  double p[2] = {pozice[0] + 10.0, pozice[1] + stoupa_velikost/2.0};
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);

  glTranslatef(p[0], p[1], 0.0);
  glRotatef(180.0 + uhel,0.0,0.0,1.0);
  glBegin(GL_QUADS);
    glVertex2f(10.0, 0.0);
    glVertex2f(0.0, 10.0);
    glVertex2f(-90.0, 0.0);
    glVertex2f(0.0, - 10.0);
  glEnd();

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glPopMatrix();


}






void hud::kresli(GLuint *textury){
  if (zobraz){
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0,800,0,600,-1,1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glEnable(GL_COLOR_MATERIAL);
    glDisable(GL_DEPTH_TEST);

    glDisable(GL_TEXTURE_2D);
//    vypis(text_fps, GLUT_BITMAP_HELVETICA_18, 395, 580, 1.0, 0.5,0.0);
    glEnable(GL_TEXTURE_2D);






    otacky->kresli(textury);
    rychlost->kresli(textury);
/*
    plyn->kresli(textury);
    vyska->kresli(textury);
    koule->kresli(textury);
    stoupani->kresli(textury);
*/


    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_COLOR_MATERIAL);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

  }
}





//   ************************
//   ***      CYKLUS      ***
//   ************************




void trychlost::cyklus(double spd, bool zapnuto){
  int temp = (int)fabs(spd);
  cislo[0] = 0;
  cislo[1] = 0;
  cislo[2] = 0;

  while (temp > 100) {
    temp -= 100;
    cislo[0]++;
  }
  while (temp > 10) {
    temp -= 10;
    cislo[1]++;
  }
  while (temp > 1) {
    temp -= 1;
    cislo[2]++;
  }
  double zn = 1.0;
  if (typ == 1) zn = -1.0;
  if (zapnuto){
    if (pozice[0] < pozice_z[0]) pozice[0] += krok;
    if (pozice[1]*zn < pozice_z[1]*zn) pozice[1] += zn * krok;
  }
  if (!zapnuto){
    if (pozice[0] > pozice_v[0]) pozice[0] -= krok;
    if (pozice[1]*zn > pozice_v[1]*zn) pozice[1] -= zn*krok;
  }
  if ((pozice[0] > pozice_v[0]) && (zn*pozice[1] > zn*pozice_v[1])) zobraz = true;
    else zobraz = false;
}


void totacky::cyklus(double otac, int stup, bool zapnuto){
  if (zapnuto){
    if (pozice[0] > pozice_otac[0]) pozice[0] -= krok;
    if (pozice[1] < pozice_otac[1]) pozice[1] += krok;
  }
  if (!zapnuto){
    if (pozice[0] < otac_pryc[0]) pozice[0] += krok;
    if (pozice[1] > otac_pryc[1]) pozice[1] -= krok;
  }
  if ((pozice[0] < otac_pryc[0]) && (pozice[1] > otac_pryc[1])) zobraz = true;
    else zobraz = false;

  double ot = otac;
  uhel = 90.0 * (1.2*ot - 0.25);
  st = stup;

}

void tplyn::cyklus(double _plyn, bool zapnuto){
  if (zapnuto)
    if (pozice[0] < pozice_plyn[0]) pozice[0] += krok;
  if (!zapnuto)
    if (pozice[0] > plyn_pryc[0]) pozice[0] -= krok;
  if (pozice[0] > plyn_pryc[0]) zobraz = true;
    else zobraz = false;
  plyn = _plyn;

}

void tkoule::cyklus(double un, double uz, bool zapnuto){
  uheln = un;
  uhelz = uz;
  if (zapnuto)
    if (pozice[1] < pozice_z[1]) pozice[1] += krok;
  if (!zapnuto)
    if (pozice[1] > pozice_v[1]) pozice[1] -= krok;
  if (pozice[1] > pozice_v[1]) zobraz = true;
    else zobraz = false;
}

void tstoupani::cyklus(double deltaz, bool zapnuto){
  uhel = deltaz * 100.0;

  if (uhel > 45.0) uhel = 45.0;
  if (uhel < -45.0) uhel = -45.0;


  if (zapnuto)
    if (pozice[1] < pozice_z[1]) pozice[1] += krok;
  if (!zapnuto)
    if (pozice[1] > pozice_v[1]) pozice[1] -= krok;
  if (pozice[1] > pozice_v[1]) zobraz = true;
    else zobraz = false;
}




void hud::cyklus(double spd, double otac, int stup,double _plyn, double alt, double un, double uz, double deltaz, int fps, bool enable){
  bool polozky[6];
  for (int i = 0; i < 6; i++)
    polozky[i] = false;

  polozky[0] = enable;
  polozky[1] = enable;

  rychlost->cyklus(spd, polozky[0]);
  otacky->cyklus(otac, stup, polozky[1]);
/*
  vyska->cyklus(alt, polozky[3]);
  koule->cyklus(un, uz, polozky[4]);
  stoupani->cyklus(deltaz, polozky[5]);
  plyn->cyklus(_plyn, polozky[2]);
*/
  zobraz = true;

  int n0 = 0;
  int n1 = 0;
  int n2 = 0;
  int cislo = fps;

  while (cislo > 100) {
    cislo -= 100;
    n2++;
  }
  while (cislo > 10) {
    cislo -= 10;
    n1++;
  }
  while (cislo > 0) {
    cislo -= 1;
    n0++;
  }
  text_fps[5] = 48 + n2;
  text_fps[6] = 48 + n1;
  text_fps[7] = 48 + n0;

}





void znak(int cislo, double *pole){
  int radek = (int)floor(cislo / 4.0);
  int sloupec = (int)cislo % 4;
  pole[0] = sloupec * 0.25;
  pole[1] = 1.0 - 0.25 * (radek + 1);
}


void hud::vypis(char *string, void *font, int x, int y, double r, double g, double b){
    glColor4f(r, g, b, 0.8);
    glRasterPos2i(x, y);
    //for (int i = 0; i < 10; i++) glutBitmapCharacter(font, *(string+i));
}
