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


#ifndef _LOADDATA_
#define _LOADDATA_

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

#include "matika.h"
#include <vector>
#include <map>
#include "Pacejka.h"
using namespace std;

const int TYP_SKODA120 = 0;
const int TYP_SKODAFAV = 1;
const int TYP_KAROSA = 2;
const int TYP_SUPER120 = 3;
const int TYP_SKODAOCT = 4;
const int TYP_SKODAOCTTURBO = 5;
const int TYP_TOYOTA = 6;

const int MODEL_KAROSERIE = 0;
const int MODEL_KOLO = 1;
const int MODEL_ZSVETLA = 2;
const int MODEL_PSVETLA = 3;
const int MODEL_LSMER = 4;
const int MODEL_PSMER = 5;
const int MODEL_ZPSVETLA = 6;
const int MODEL_BSVETLA = 7;


const int ZVUK_STARTER = 0;
const int ZVUK_MOTOR_IDLE = 1;
const int ZVUK_MOTOR = 2;
const int ZVUK_SMYK = 3;
const int ZVUK_SMYK_ASFALT = 4;
const int ZVUK_RAZENI = 5;
const int ZVUK_KOLIZE = 6;


GLuint VytvorDisplayList(int typ, int soucastka, bool mat); // typ udava typ auta (skoda) a soucastka dil auta (svetlo)
GLuint VytvorTexturu(int typ);
GLuint CreateSphereDisplayList();

void EnvTextures(GLuint* textures);


typedef enum {TEXTURE_GRASS, TEXTURE_SNOW, TEXTURE_SNOW25, TEXTURE_SNOW50, TEXTURE_SNOW75, TEXTURE_MUD} surface_texture;
typedef enum {TEXTURE_STRAIGHT, TEXTURE_CURVE, TEXTURE_CROSS, TEXTURE_HALFCROSS} road_texture;


class TerrainTextures{
      map<surface_texture, GLuint> surface_normal;
      map<surface_texture, GLuint> surface;
      map<road_texture, GLuint> road;
public:
    TerrainTextures();
    ~TerrainTextures();
    void bindSurfaceTexture(surface_texture st);
    GLuint getSurfaceTexture(surface_texture st);
    void bindRoadTexture(road_texture rt);
    GLuint getRoadTexture(road_texture rt);

    void bindSurfaceNormalTexture(surface_texture st);

};



char* getSoundPath(int car, int sound);




//! Struktura s informacemi pro jednu obalovou kouli.
/*!
  Sdruzuje vsechny potrebne informace o jedne obalove kouli nactene ze souboru,
*/
struct obal_koule
{
  //! Unikatni identifikator koule.
  /*! Kazda koule v hierarchii ma unikatni identifikator. */
  int ID;
  //! pozice koule v souradnicich objektu
  /*! (vektor od stredu objektu) */
  pozice3D pozice;
  //! polomer koule
  /*! polomer obalove koule */
  double polomer;
  //! rodic koule
  /*! Identifikator nadrazene koule v hierarchhii obalovych kouli. */
  int rodic;

  obal_koule()
  {
    ID = 0;
    MatPoziceSmaz(pozice);
    polomer = 0;
    rodic = -1;
  }
  obal_koule(const obal_koule& koule)
  {
    (*this) = koule;
  }
  obal_koule& operator=(const obal_koule& koule)
  {
    ID = koule.ID;
    MatPoziceKopni(koule.pozice, pozice);
    polomer = koule.polomer;
    rodic = koule.rodic;
    return *this;
  }
};

//! Struktura s informacemi o hierarchii obalovych kouli.
/*!
  Sdruzuje vsechny potrebne informace o vsech obalovych koulich v hierarchii. Tato struktura se predava metode InitObal() tridy
  CObal, ktera podle ni vytvori hierarchii.
*/
struct obal_data
{
  //! seznam kouli
  /*! seznam vsech nactenych kouli ze souboru */
  vector<obal_koule> hierarchie_obalu;
};

// tato trida slouzi k nacteni dat o autu z konfiguracniho souboru
class vehicle_data{
  double hmotnost[8];
  double rozmery[3];               // delka, sirka, vyska
  vektor3D pozice_vystupu;
  vektor3D pozice_ridice;
  pozice3D pozice_tlumicu[4];
  double tlumeni_predni_naraz;            // tlumeni tlumicu predni napravy (naraz)
  double tlumeni_predni_odraz;            // tlumeni tlumicu predni napravy (odraz)
  double tlumeni_zadni_naraz;            // tlumeni tlumicu zadni napravy
  double tlumeni_zadni_odraz;            // tlumeni tlumicu zadni napravy
  double tvrdost_predni;            // tvrdost tlumicu predni napravy
  double tvrdost_zadni;             // tvrdost tlumicu zadni napravy
  double antirollbar_predni;
  double antirollbar_zadni;
  double maxzat;                   // maximalni zataceni v radianech (ze stupnu se prevede pri nacitani ze souboru)
  double rychzat;                  // rychlost zataceni
  double polomer_predni;             // polomer kol na predni naprave
  double polomer_zadni;             // polomer kol na zadni naprave
  double max_otacky;
  double vykon;
  int pocet_stupnu;
  double pomery[10];
  bool pohon_predni;                // pohon na predni napravu
  bool pohon_zadni;                // pohon na zadni napravu
  double hmotnost_kola_zadni;       // hmotnost kol na zadni naprave
  double hmotnost_kola_predni;       // hmotnost kol na predni naprave
  bool svetla;                    // jestli auto ma svetla
  bool smerovky;
  bool brzda;
  bool zpatecka;
  double delka_predni;
  double delka_zadni;
  bool textura;


  Pacejka* pacejka;
 public:


  obal_data ObaloveKoule;
  double Hmotnost(int bod){return hmotnost[bod];}
  double Delka(){return rozmery[X];}
  double Sirka(){return rozmery[Y];}
  double Vyska(){return rozmery[Z];}
  double *TlumicPozice(int num){return pozice_tlumicu[num];}
  double TlumicTlumeniPredniNaraz(){return tlumeni_predni_naraz;}
  double TlumicTlumeniPredniOdraz(){return tlumeni_predni_odraz;}
  double TlumicTlumeniZadniNaraz(){return tlumeni_zadni_naraz;}
  double TlumicTlumeniZadniOdraz(){return tlumeni_zadni_odraz;}
  double TlumicTvrdostPredni(){return tvrdost_predni;}
  double TlumicTvrdostZadni(){return tvrdost_zadni;}
  double TlumicDelkaZadni(){return delka_zadni;}
  double TlumicDelkaPredni(){return delka_predni;}

  double PolomerPredni(){return polomer_predni;}
  double PolomerZadni(){return polomer_zadni;}
  bool TlumicPohonPredni(){return pohon_predni;}
  bool TlumicPohonZadni(){return pohon_zadni;}
  double HmotnostKolaPredni(){return hmotnost_kola_predni;}
  double HmotnostKolaZadni(){return hmotnost_kola_zadni;}
  double AntirollbarPredni(){return antirollbar_predni;}
  double AntirollbarZadni(){return antirollbar_zadni;}
  bool Svetla(){return svetla;}
  bool Smerovky(){return smerovky;}
  bool Brzda(){return brzda;}
  bool Zpatecka(){return zpatecka;}
  double MaxZat(){return maxzat;}
  double RychZat(){return rychzat;}
  vehicle_data(int typ);
  double *PoziceRidice(){return pozice_ridice;}
  double *PoziceVystupu(){return pozice_vystupu;}
  double MaxOtacky(){return max_otacky;}
  double Vykon(){return vykon;}
  double *Pomery(){return pomery;}
  int PocetStupnu(){return pocet_stupnu;}
  Pacejka* dejPacejku(){return pacejka;}
  bool Textura() const {return textura;}

};





#endif
