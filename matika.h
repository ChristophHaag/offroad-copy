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


#ifndef _XYZW_
#define _XYZW_
const int X = 0;
const int Y = 1;
const int Z = 2;
const int W = 3;
#endif


#ifndef _MATIKA_
#define _MATIKA_

const double KROK = 0.005;
const double KROK2 = KROK / 2.0;
const double KROK4 = KROK / 4.0;
const double KROK6 = KROK / 6.0;

const double GRAVITACE = 9.81;


#include <math.h>
const double PI = M_PI;
typedef double pozice3D[3];
typedef double vektor3D[3];
typedef double pozice2D[2];
typedef double vektor2D[2];
typedef double barva3[3];
typedef double matice[9];
typedef double quaternion[4];


struct MatBaze{
  pozice3D pozice;
  vektor3D nahoru;
  vektor3D doleva;
  vektor3D dopredu;
  void Reset(bool smaz_pozici);
  void RotaceX(double uhel);
  void RotaceY(double uhel);
  void RotaceZ(double uhel);
  void Nacti(MatBaze *baze2);
};
int xy_to_i(int x, int y, int sirka);

// MATICE
void MatMaticeSmaz(matice mat);
void MatMaticeBaze(matice mat);
void MatMaticeRotaceX(matice mat, double uhel);
void MatMaticeRotaceY(matice mat, double uhel);
void MatMaticeRotaceZ(matice mat, double uhel);
void MatMaticeTransponuj(matice mat);
void MatMaticeKopni(matice zdroj, matice cil);
void MatMaticeXVektor(matice mat, vektor3D v, vektor3D vysledek);
void MatMaticeXMatice(matice mat1, matice mat2, matice vysledek);
void MatMaticeXSkalar(matice mat, double skalar, matice vysledek);
double MatMaticeDeterminant(matice mat);
void MatMaticeInvertuj(matice mat);
void MatMaticeZQuat(matice mat, quaternion q);
void MatMaticeOpengl(matice mat, float *opengl_mat);


// POZICE
void MatPozicePlusVektor(pozice3D p, vektor3D v, pozice3D vysledek);
void MatPozicePlusVektor(pozice3D p, vektor3D v);


// VEKTOR
void MatVektorPridej(vektor3D zdroj, vektor3D cil);
void MatVektorXSkalar(vektor3D v, double s);

// QUATERNION
void MatQuatKopni(quaternion zdroj, quaternion cil);
void MatQuatSmaz(quaternion q);
void MatQuatPridej(quaternion zdroj, quaternion cil);
void MatQuatVyrob(vektor3D osa, double uhel, quaternion q);

void MatVektorVypis(vektor3D v);
void MatPoziceVektor(pozice3D p1, pozice3D p2, vektor3D cil);
void MatVektorNasob(vektor3D v, double cislo);
void MatVektorKopni(vektor3D zdroj, vektor3D cil);
void MatPoziceKopni(pozice3D zdroj, pozice3D cil);
void MatPoziceKopni(const pozice3D zdroj, pozice3D cil);
void MatPozicePresun(pozice3D pocatek, vektor3D smer, double vzd, pozice3D vysledek);
double MatPoziceVzdalenost(pozice3D p1, pozice3D p2);
void MatVektorOpak(vektor3D zdroj, vektor3D cil);
void MatVektorOpak(vektor3D v);
double MatStupenRadian(double stupen);
double MatRadianStupen(double radian);
void MatVektorNormal(vektor3D v1, vektor3D v2, vektor3D cil);
double MatVektorVelikost(vektor3D v);
void MatVektorJednot(vektor3D v, vektor3D cil);
void MatVektorJednot(vektor3D v);
void MatVektorSmaz(vektor3D v);
void MatPoziceSmaz(pozice3D v);
double MatPoleMin(double *pole, int n);
void MatPozicePrumer(pozice3D p1, pozice3D p2, pozice3D cil);
void MatRovinaPrusecik(double *rovina, pozice3D bod, vektor3D vektor, pozice3D vysledek);
double MatVektorUhel(vektor3D v);        // slozka Z musi byt 0
void MatVektorRotace(vektor3D v, double uhel, int osa);
void MatVektorSecti(vektor3D v1, vektor3D v2, vektor3D cil);
double MatVektorUhel(vektor3D v);
double MatVektorUhel(vektor3D v1, vektor3D v2);
void MatRovina(vektor3D normala, pozice3D bod, double *rovnice);
bool MatFloatKladne(double cislo);


// pole
double MatPoleInterpoluj(double *pole, int n, double index);  // Linearni interpolace meyi dvemi polozkami pole
void MatPoleKopniff(float *zdroj, float *cil, int n);
void MatPoleKopnidf(double *zdroj, float *cil, int n);
// stare funkce

double prumer(double x, double y);
double uhelr(double z1, double z2, double prepona);
double radiany(double stupne);
double stupne(double radiany);
double vzdalenost(double *bod1, double *bod2);
double vzdalenost2d(double *bod1, double *bod2);
void kopni_pole(double *zdroj, double *cil, int pocet);
void kopni_pole(int *zdroj, int *cil, int pocet);
void rotace(double *v, double uhel, int osa);
void opacny_vektor(double *v);
void normala(double *v1, double *v2, double *cil);
double uhelv(double *vektor1, double *vektor2, int n);
double velikost_vektoru(double *vektor, int n);
void normalizuj(double *vektor, int n);
double rovina(double *bod1,double *bod2,double *bod3,double *bod4);  // funkce vrati pozici Z ctvrteho bodu tak, aby byl v rovine s predchozimi tremi
double random(double max);
int randomi(int max);

double kmh(double ms);
double ms(double kmh);
double znamenko(double cislo);

void smaz_pole(int *pole, int n);
void smaz_pole(double *pole, int n);

int MatIntMin(int xx, int yy);
int MatIntMax(int xx, int yy);
double MatMax(double x, double y);

int iabs(int n);


#endif
