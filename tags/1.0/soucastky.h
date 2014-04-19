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


#ifndef _SOUCASTKY_
#define _SOUCASTKY_

#include "pruziny.h"
#include "3Dmath/Vec3D.h"
#include "3Dmath/Mat3D.h"
#include "teren.h"

struct tlumic:public cpruzina{
  bool init_ok;
  bool POHON_KOLA;
  bool ZATACENI;
  double PRUMER_KOLA;
  double HMOTNOST_KOLA;
  double M_NA_MINUS_PRVNI;     // 1/hmotnost_kola
  double SETRVACNOST;
  double I_NA_MINUS_PRVNI;     // 1/setrvacnost
  double PACEJKAF[20];
  double PACEJKAS[20];
  bool let;
  bool smyk;
  bool brzda;
  matice R;
  matice Rtr;
  pozice3D pozice_tlumice;
  pozice3D pozice_kola;
  pozice3D kontakt;           // misto, kde se pneumatika dotkne terenu
  double lin_x;                // 1D vzdalenost kola od karoserie
  double lin_v;                // 1D rychlost kola od tlumice
  double lin_a;                // 1D zrychleni
  vektor3D vk3D;              // 3D rychlost kola
  vektor3D vt3D;              // 3D rychlost tlumice
  vektor3D vp3D;              // rychlost povrchu
  double natoceni_kola;        // kvuli vykreslovani
  double zatoceni;             // uhel zatoceni kola
  double w;                    // uhlova rychlost kola
  double dw;                   // zmena otoceni kola
  double rotace;                // otoceni kola kolem sve osy
  double otacky;               // otacky za minutu (RPM)
  vektor3D sila_up;           // vysledna sila tlumice
  vektor3D sila_down;         // vysledna sila kola
  double lin_sila;             // sila pusobici na kolo (od tlumice a terenu)
  double rot_sila;
  teren *t;                   // ukazatel na teren (kvuli kolizim)

  // RUNGE-KUTTA
  double lin_x0, lin_x1, lin_x2, lin_x3;
  double lin_v0, lin_v1, lin_v2, lin_v3;
  double lin_a1, lin_a2, lin_a3;
  double dw1, dw2, dw3;
  double w0, w1, w2, w3;
  double rotace0, rotace1, rotace2, rotace3;

  tlumic(double _prumer_kola, double _delka_tlumice, double _tvrdost, double _tlumeni, double _hmotnost_kola, teren *ter, bool _pohon_kola, bool _zataceni, double *pacejkaf, double *pacejkas);
  void Srovnej();                               // zmeni pozici kola tak, aby bylo v linii s tlumicem
  void Kolize(vektor3D rychlost_kola);          // osetri kolizi kola s terenem
  void Matice(double uhel);      // natoci zadanou matici rotace podle zatoceni kol
  void SilaPneu();              // urci jaka sila pusobi na auto vlivem treni pneumatik o povrch
  void Rychlost(vektor3D rychlost_pozice_kola);              // spocita 3D rychlost kola
  void RK0(double _zatoceni, bool _brzda);
  void RK1(matice _R, vektor3D _pozice_tlumice, vektor3D rychlost_pozice_kola, vektor3D rychlost_pozice_tlumice, double rot_sila0);
  void RK2(matice _R, vektor3D _pozice_tlumice, vektor3D rychlost_pozice_kola, vektor3D rychlost_pozice_tlumice, double rot_sila0);
  void RK3(matice _R, vektor3D _pozice_tlumice, vektor3D rychlost_pozice_kola, vektor3D rychlost_pozice_tlumice, double rot_sila0);
  void RKFINAL(matice _R, vektor3D _pozice_tlumice, vektor3D rychlost_pozice_kola, vektor3D rychlost_pozice_tlumice, double rot_sila0);
  void Sily(vektor3D rychlost_kola);
  void Tlumeni();               // tlumeni rychlosti kola
  void Kresli();
};




struct tsvetlo{
  int interval_blikani;
  int interval_stav;
  bool blik;
  barva3 barva_vyp;
  barva3 barva_zap;
  bool sviti;
  GLuint DL;
  Vec3D pozice;
  Mat3D matice_rotace;
  void Cyklus(bool zap, const Mat3D& _matice_rotace, const Vec3D& pos);
  void Kresli();
  tsvetlo(int interval, barva3 bv, barva3 bz);
};



struct tvolant{
  double MAX_ZATOCENI;
  double RYCHLOST_ZATACENI;
  double zatoceni;
  void Cyklus(bool left, bool right);
  tvolant(double _max_zataceni, double _rychlost_zataceni);
};



typedef enum
{
   MOTOR_VYP, MOTOR_STARTUJE, MOTOR_BEZI
} EStavMotoru;

struct cmotor{
  double MAX_W;
  double NEUTRAL_W;							// kdy se tam hodi neutral
  double CHCIP_W;								// kdy chcipne
	double NORMAL_W;							// otacky na neutral
	EStavMotoru stav;							// aktualni stav motoru
  double VYKON;
  double I_NA_MINUS_PRVNI;
  double w, dw;
  double w0, w1, w2, w3;
  double dw1, dw2, dw3;
  bool plyn, starter;
 	int stav_startovani;
  double sila;
  cmotor(double _max_otacky, double _vykon);
  void Sila();
  void RK0(bool plyn, bool starter);
  void RK1(double spojka);
  void RK2(double spojka);
  void RK3(double spojka);
  void RKFINAL(double spojka);
  void Tlumeni();

  bool starter_sound;
};

struct cprevodovka{
  double POMERY[10];
  int POCET_STUPNU;
  int stupen;       // aktualni zarazeny stupen
  int stupen_cil;   // kam se snazi zaradit
  bool radim;       // probiha razeni
  bool spojka;
  int stav_razeni;  // razeni trva urcity cas
  cprevodovka(int _pocet, double *_pomery);
  void Cyklus(bool up, bool down);
  double Pomer(){return POMERY[stupen];}   // vrati soucasny prevodovy pomer
};


#endif
