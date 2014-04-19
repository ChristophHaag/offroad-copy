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


#ifndef _TEREN_
#define _TEREN_

#include <SDL/SDL.h>


#include "3Dmath/Vec3D.h"
#include "struktury.h"
#include "obal.h"
#include "glsl.h"
#include "frustum.h"

class teren;
const int pocet_hvezd = 1000;



struct thvezdy{
  bool zobraz;
  double pozice[pocet_hvezd][3];
  thvezdy(bool enable);
  void cyklus(pozice3D _pozice_kamery);
  pozice3D pozice_kamery;
  void kresli();
};


const int STROM_POCET_TYPU = 6;
const int STROM_DOHLED = 14;
class CStromy{
  bool mapa[255*255];                 // udava pritomnost stromu na policku
  GLuint DL[STROM_POCET_TYPU][3];
  teren *t;
  pozice3D pozice_kamery;
 public:
  CStrom *strom[255*255];              // ukazatel na strom
  CStromy(bool *_mapa, teren *ter);
  ~CStromy();
  void init(GLuint *textury);
  void deinit();
  void Cyklus(pozice3D _pozice_kamery);
  void Kresli(int* quality, int sirka);
};


struct tslunce {
  double R;
  double G;
  double B;
  double Ra;
  double Ga;
  double Ba;
  double delta;                 // o kolik se meni pozice slunce
  double uhel;
  double vektor[3];             // smer paprsku
  double pozice[3];             // pozice textury slunce
  void cyklus(bool plus, bool minus, double *pozice_kamery);
  void kresli();
  tslunce(double _uhel);
};



const int VELIKOST_CTVERCE = 20;
const int KVALITA[] = {1, 5, 10};          // udava kolikaty bod se pouzije pro kresleni, velikost ctverce musi byt delitelna kvalitou



class teren {
    TerrainTextures* terrainTextures;       // trida obsahujici textury terenu (loaddata.h) - musi byt vytvorena az po inicializaci opengl
    glsl shader;                              // shader pro vykreslovani textur terenu

  tslunce *slunce;
  double pozice_kamery[3];
  int pocet_ctvercu;
  int *ctverec_q1;                             // nove informace o kvalite zobrazeni kazdeho ctverce
  int *ctverec_q2;                             // stare informace o kvalite zobrazeni kazdeho ctverce
  GLuint *ctverec_dl;
  char *body;                                  // pole s body terenu (zakodovane)
  unsigned char *silnice;                      // informace o silnicich



  int sirka;                                   // sirka a vyska mapy (pocty bodu)
  int vyska;
  void nastavTextury(int tyc_x, int tyc_y);
  GLuint zjisti_texturu(int tyc_x, int tyc_y);                      // vrati cislo textury. Zadavaji se souradnice policka v tycoonovi
  void kresli_ctverec(int poc_x, int poc_y, int kvalita);           // kvalita udava kolikaty bod se vykresli (1 = nejlepsi)
  void Rovnice_roviny(int x, int y, double *rovnice1, double *rovnice2);
  bool init_ok;
  thvezdy *hvezdy;
 public:
  CStromy *stromy;
  short *typ_povrchu;                          // typ policka
  void NactiBod(int x, int y, pozice3D souradnice);                            // Nacte bod ze souradnic pole
  int GetIndex(double s);                                                       // Prevede globalni souradnice na souradnice pole
  void Normala_bodu(int x, int y, vektor3D normala);                           // vrati normalovy vektor bodu
  void Normala_roviny(pozice3D bod, vektor3D vysledek);
  bool JeTam(const Vec3D& bod);
  teren(char *cesta);
  ~teren();
  void cyklus(bool plus, bool minus, double *pozice_kam, double uhel_kam, const Frustum& frustum);
  void InitStromy(GLuint *textury);
  void kresli();
  double Z(double xx, double yy);
  bool Prusecik_n(const Vec3D& bod, Vec3D* vysledek, Vec3D* normal);
  double Tvrdost(pozice3D pozice);
  double Povrch(pozice3D p, bool& isOnRoad);                                                           // vraci smykovou vlastnost povrchu
  bool Prusecik(pozice3D bod, vektor3D smer, double vzd, pozice3D prusecik); // najde prusecik terenu s primkou (pokud nenajde bod v zadane vzdalenosti, vraci false)
  double* poziceSlunce(){return slunce->pozice;}

  void init();
  void deinit();
};


#endif
