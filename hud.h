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



struct trychlost {
  int cislo[3];
  int typ;
  double rych;
  double pozice[2];
  double pozice_z[2];
  double pozice_v[2];
  bool zobraz;
  void cyklus(double spd, bool zapnuto);
  void kresli(GLuint *textury);
  trychlost(bool enable, double *pozice_zap,  double *pozice_vyp, int _typ);
};

struct totacky {
  double ot;       // otacky
  int st;       // zarazeny stupen
  double pozice[2];
  double uhel;
  bool zobraz;
  void cyklus(double otac, int stup, bool zapnuto);
  void kresli(GLuint *textury);
  totacky(bool enable);
};

struct tplyn {
  bool zobraz;
  double plyn;
  double pozice[2];
  void cyklus(double _plyn, bool zapnuto);
  void kresli(GLuint *textury);
  tplyn(bool enable);
};

struct tkoule{
  bool zobraz;
  double uheln;
  double uhelz;
  double pozice[2];       // aktualni
  double pozice_v[2];     // vypnuto
  double pozice_z[2];     // zapnuto
  void cyklus(double un, double uz, bool zapnuto);
  void kresli(GLuint *textury);
  tkoule(bool enable);
};

struct tstoupani{
  bool zobraz;
  double uhel;        // uhel rucicky
  double pozice[2];       // aktualni
  double pozice_v[2];     // vypnuto
  double pozice_z[2];     // zapnuto
  void cyklus(double deltaz, bool zapnuto);
  void kresli(GLuint *textury);
  tstoupani(bool enable);
};



class hud {
  totacky *otacky;
  trychlost *rychlost;
  tplyn *plyn;
  trychlost *vyska;
  tkoule *koule;
  tstoupani *stoupani;
  char text_fps[10];
  bool zobraz;
  void vypis(char *string, void *font, int x, int y, double r, double g, double b);
 public:
  void cyklus(double spd, double otac, int stup,double _plyn, double alt, double un, double uz, double deltaz, int fps,  bool enable);
  void kresli(GLuint *textury);
  hud(bool enable);
  ~hud();
};


void znak(int cislo, double *pole);
