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


// Implementuje fyzikalni model hmotneho kvadru, kod je modifikovan pro konkretni ucely tohoto enginu
// Zatim neni hotova diagonalizace, takze kvadr musi byt soumerny aspon podle dvou(?) os.
// Copyright Milan Timko 2006 (milantimko@seznam.cz)
#ifndef _OBJEKT_
#define _OBJEKT_
#include "glsl.h"
#include "teren.h"
#include "obal.h"


#include "3Dmath/Mat3D.h"
#include "3Dmath/Quat3D.h"
#include "3Dmath/Vec3D.h"


struct DrawObject
{
  Vec3D pos;
  Mat3D orient;
  bool useColor;
  bool useEmissive;
  Vec3D color;
  int dl;
  int texture;
  glsl shader;
  void Draw(const Vec3D& cameraPos, const Vec3D& sunPos);
};

class objekt:public CObal
{
 public:
  bool tlumit_naraz[8];
  pozice3D tlumeni_pozice[8];
  vektor3D tlumeni_smer[8];
  bool init_ok;
  bool kolize;
  bool useTexture;
  unsigned int texture;
  int kresleni_obalu;               // udava jak se ma kreslit obal auta
  glsl shader;

  // zpetna kompatibilita ze starou mat. knihovnou
  pozice3D stred_old;
  pozice3D pozice_old;
  pozice3D rychlost_old;


  // konstanty
  matice SETRVACNOST;           // matice momentu setrvacnosti v souradnicich objektu
  double CELKOVA_HMOTNOST;       // celkova hmotnost objektu
  double M_NA_MINUS_PRVNI;       // obracena hodnota celkove hmotnosti
  double IyyMinusIzzDelenoIxx;   // aby se to nemuselo pocitat v kazdem cyklu
  double IzzMinusIxxDelenoIyy;
  double IxxMinusIyyDelenoIzz;
  double LIN_TLUMENI;            // tlumeni linearni rychlosti (= - kd * m)
  double ROT_TLUMENI[3];         // tlumeni rotacni rychlosti (= - kd * Ixx)
  teren *t;                     // ukazatel na teren (objekt si bude sam hlidat kolize s terenem)

  // stavove promenne
  Vec3D v;                   // linearni rychlost objektu
  Vec3D w;                   // rychlost rotace
  Vec3D rot_sila;            // zasobnik pro rotacni slozku sily
  Vec3D lin_sila;            // zasobnik pro linearni slozku sily
  Vec3D x;                   // stred hmotnosti objektu (kolem nej se bude otacet)
  Mat3D R;                     // matice rotace - musi se prepocitat vzdy po zmene natoceni
  Mat3D inv_R;
  Quat3D Q;                 // informace o rotaci v quaternionu

  // runge-kutta promenne
  Vec3D v0, v1, v2, v3;      // lin. rychlosti pro 4 kroky runge-kutty
  Vec3D w0, w1, w2, w3, w4;  // zrychleni pro 4 kroky a jeden posledni
  Vec3D dv1, dv2, dv3;       // zmena zrychleni, staci 3 kroky
  Vec3D dw1, dw2, dw3;
  Vec3D x0;                  // zaloha pozice
  Quat3D Q0;

  // bounding box
  Vec3D body[8];                // rohy kvadru
  Vec3D bod;                    // geometricky stred objektu (sem se bude vykreslovat)
  double roviny[6][4];              // rovnice rovin jednotlivych sten
  Vec3D body_ts[8];          // relativni pozice jednotlivych bodu od stredu hmoty
  double HMOTNOST[8];               // hmotnosti vsech bodu
  GLuint DL;                       // display list objektu (pokud je 0, tak se zobrazi spojnice mezi body

  // METODY
  objekt(pozice3D _pozice, float azimut);
  void RK0();       // kroky integrace podle Runge-Kutty
  void RK1();
  void RK2();
  void RK3();
  void RKFINAL();

  void KresliObjekt();                                                    // vykresli bounding box
  void Init(double sirka, double delka, double vyska, double *hmotnost_bodu, teren *_t); // nastaveni parametru objektu (nemuze byt v konstruktoru, protoze data se nastavuji az pri nacitani dat ze souboru v potomkovi)
  void VypocitejBB();                                                     // vypocita bounding box
  Vec3D VypocitejDW(double krok);                                            // vypocita zmenu rychlosti rotace v zavislosti na pusobici sile
  void Tlumeni(double krok);                                                         // aplikuje tlumeni
  bool Kolize();                                                          // zpracuje kolize s terenem
  void PridejSilu(const Vec3D& bod_pusobeni, const Vec3D& sila);                  // postrci objekt v danem miste
  Vec3D RychlostBodu(const Vec3D& bod) const;                     // zjisti rychlost bodu leziciho na objektu
  void DrawDiag() const;
  void OnCollision();

  // for audio system
  void ClearCollision() {kolize = false;}
  bool IsCollision() const {return kolize;}

  bool JeTam(pozice3D bod);                                               // zjisti jestli je dany bod uvnitr bounding boxu

  // informace pro ostatni casti enginu
  double Hmotnost(){return CELKOVA_HMOTNOST;}
  double *Stred();
  virtual void Cyklus(keys *klavesy);
  virtual void Kresli(const Vec3D& kamera);
  virtual double *Pozice();
  virtual double *Pozice_ridice();
  virtual double *Pozice_vystupu();
  virtual double Spd();
  virtual double Otacky();
  virtual int Stupen();
  virtual float Zataceni() {return 0.0f;}
  virtual void GetDrawObjects(std::vector<DrawObject>& objectList) const;
  double *V();
};



#endif
