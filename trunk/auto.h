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



#ifndef _VEHIKL_
#define _VEHIKL_

#include "soucastky.h"
#include "suspension.h"
#include "wheel.h"
#include "3Dmath/Vec3D.h"
#include "Antirollbar.h"
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include "vstup.h"
#include "objekt.h"

#include <vector>

const int BLIKANI_INTERVAL = 80;

class vehikl: public objekt {
  bool vytvoritlist;            // pokud je true, v cyklu se vytvori DL (musi se vytvorit az po zavolani MAIN)
  int typ;                  // znacka auta
  bool aktivni;             // jestli je v aute hrac
  //tlumic *tlumice[4];
  suspension *tlumice[4];
  wheel *kola[4];

  tvolant *zataceni;
  cmotor *motor;
  cprevodovka *prevodovka;
  double spojka;
  Antirollbar antirollbar_predni;
  Antirollbar antirollbar_zadni;

  Vec3D kola_pozice_os[4];          // vektor na kola v obj. souradnicich (od geom. stredu)
  Vec3D kola_pozice[4];             // pozice kol ve svetovych souradnicich
  Vec3D kola_rychlost[4];           // rychlost bodu, kde je pripojen tlumic kola

  GLuint DL_kola;
  tsvetlo *zadnisvetla;
  tsvetlo *prednisvetla;
  tsvetlo *levesmerovky;
  tsvetlo *pravesmerovky;
  tsvetlo *zpateckasvetlo;
  tsvetlo *brzdovesvetlo;

  Vec3D POZICE_RIDICE;          // pozice ridice v relativnich souradnicich od prostredka auta
  Vec3D pozice_ridice;          // pozice ridice v absolutnich souradnicich
  Vec3D POZICE_VYSTUPU;          // pozice ridice v relativnich souradnicich od prostredka auta
  Vec3D pozice_vystupu;          // pozice ridice v absolutnich souradnicich



  // zpetna kompatibilita
  vektor3D pozice_ridice_stare;          // pozice ridice v absolutnich souradnicich
  vektor3D pozice_vystupu_stare;          // pozice ridice v absolutnich souradnicich



  int pohon_kolo;                   // kolo na ktere pusobi sila od motoru



  double rot_sila_kolo[4];          // rotacni sila pusobici na jednotlive kola
  bool plyn, brzda, doleva, doprava, rucnibrzda, razeni_n, razeni_d, starter;
  bool kresleni_karoserie;          // ma se kreslit karoserie?
  bool svetla;
  void Klavesy(keys *klavesy);      // zpracuje klavesy
  void Reset();
  void VytvorListy();               // vytvori DL
  void SmazListy();
  void VypocitejPozice();           // vypocita pozice tlumicu, ridice, atd. po zmene polohy stredu auta
  double OtackyKol();               // vrati prumer otecek kol s pohonem
  void SilaNaKola();                // vypocita jaka kroutiva sila pusobi na kola

 public:
  vehikl(int _typ, pozice3D _pozice, float azimut, teren *ter);
  virtual ~vehikl();
  void Sily();                      // zpracuje sily z tlumicu
  virtual void Cyklus(keys *klavesy);
  virtual void Kresli(const Vec3D& kamera);
  virtual double *Pozice();
  virtual double *Pozice_ridice();
  virtual double *Pozice_vystupu();
  virtual double Spd();
  virtual double Otacky();
  virtual int Stupen();
  int Typ(){return typ;}
  cmotor* getMotor(){return motor;}
  virtual float Zataceni() {return zataceni->zatoceni;}
  double getSlipRatio() const;
  double getSlipRatioRoad() const;

  virtual void GetDrawObjects(std::vector<DrawObject>& objectList) const;
};

#endif
