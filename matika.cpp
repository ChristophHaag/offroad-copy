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



#include "matika.h"
#include <iostream>
#include <stdlib.h>
#include <math.h>
using namespace std;


//------------------------------------------------------------------------------
//                           ***   MATICE   ***
//------------------------------------------------------------------------------



// OPENGL
// Prevede matici rotace na transformacni matici opengl
void MatMaticeOpengl(matice mat, float *opengl_mat)
{
  opengl_mat[0] = mat[0];
  opengl_mat[1] = mat[3];
  opengl_mat[2] = mat[6];
  opengl_mat[3] = 0.0;
  opengl_mat[4] = mat[1];
  opengl_mat[5] = mat[4];
  opengl_mat[6] =  mat[7];
  opengl_mat[7] = 0.0;
  opengl_mat[8] = mat[2];
  opengl_mat[9] = mat[5];
  opengl_mat[10] = mat[8];
  opengl_mat[11] = 0.0;
  opengl_mat[12] = 0.0;
  opengl_mat[13] = 0.0;
  opengl_mat[14] = 0.0;
  opengl_mat[15] = 1.0;
}





// SMAZANI
// nastavi vsechny prvky matice na 0
void MatMaticeSmaz(matice mat)
{
  for (int i = 0; i < 9; i++)
    mat[i] = 0.0;
}

// BAZE
// nastavi matici tak, aby jednotlive radky tvorili zakl. bazi lin. prostoru
//     |1,0,0|
// m = |0,1,0|
//     |0,0,1|
void MatMaticeBaze(matice mat)
{
  MatMaticeSmaz(mat);
  mat[0] = 1.0;
  mat[4] = 1.0;
  mat[8] = 1.0;
}



// QUATERNION
void MatMaticeZQuat(matice mat, quaternion q)
{

  double l2 = powf(q[X], 2) + powf(q[Y], 2) + powf(q[Z], 2) + powf(q[W], 2);
  if (l2 < 0.9999 || l2 > 1.0001) {
    double n = (l2 + 1.0) / (2.0 * l2);
    q[X] *= n;
    q[Y] *= n;
    q[Z] *= n;
    q[W] *= n;
    l2 *= powf(n, 2);
  }

  double x2, y2, z2, xy, wz, xz, wy, yz, wx;

  x2 = powf(q[X], 2);
  y2 = powf(q[Y], 2);
  z2 = powf(q[Z], 2);
  xy = q[X] * q[Y];
  wz = q[W] * q[Z];
  xz = q[X] * q[Z];
  wy = q[W] * q[Y];
  yz = q[Y] * q[Z];
  wx = q[W] * q[X];

  double vel = 2.0 / l2;
  mat[0] = 1.0 - vel * (y2 + z2);
  mat[1] = vel * (xy - wz);
  mat[2] = vel * (xz + wy);
  mat[3] = vel * (xy + wz);
  mat[4] = 1.0f - vel * (z2 + x2);
  mat[5] = vel * (yz - wx);
  mat[6] = vel * (xz - wy);
  mat[7] = vel * (yz + wx);
  mat[8] = 1.0f - vel * (x2 + y2);
}



// QUAT PRIDEJ
void MatQuatPridej(quaternion zdroj, quaternion cil){
  for (int i = 0; i < 4; i++)
    cil[i] += zdroj[i];
}



// QUAT VYROB
void MatQuatVyrob(vektor3D osa, double uhel, quaternion q){
  q[W] = cos(uhel/2.0);
  q[X] = osa[X] * cos(uhel/2.0);
  q[Y] = osa[Y] * cos(uhel/2.0);
  q[Z] = osa[Z] * cos(uhel/2.0);
}


// SKALAR
// vynasobi vektor se skalarem
void MatVektorXSkalar(vektor3D v, double s)
{
  for (int i = 0; i<3; i++)
    v[i] *= s;
}


// QUATERNION
// zkopiruje quaternion
void MatQuatKopni(quaternion zdroj, quaternion cil)
{
  for (int i = 0; i < 4; i++)
    cil[i] = zdroj[i];
}

void MatQuatSmaz(quaternion q){
  for (int i = 0; i < 4; i++)
    q[i] = 0.0;
}


// ROTACE
// Predpoklada ze sloupce matice jsou vektory. Vsechny je otoci o dany uhel (v radianech)
void MatMaticeRotaceX(matice mat, double uhel)
{
  MatMaticeTransponuj(mat);
  MatVektorRotace(&mat[0], uhel, X);
  MatVektorRotace(&mat[3], uhel, X);
  MatVektorRotace(&mat[6], uhel, X);
  MatMaticeTransponuj(mat);
}
void MatMaticeRotaceY(matice mat, double uhel)
{
  MatMaticeTransponuj(mat);
  MatVektorRotace(&mat[0], uhel, Y);
  MatVektorRotace(&mat[3], uhel, Y);
  MatVektorRotace(&mat[6], uhel, Y);
  MatMaticeTransponuj(mat);
}
void MatMaticeRotaceZ(matice mat, double uhel)
{
  MatMaticeTransponuj(mat);
  MatVektorRotace(&mat[0], uhel, Z);
  MatVektorRotace(&mat[3], uhel, Z);
  MatVektorRotace(&mat[6], uhel, Z);
  MatMaticeTransponuj(mat);
}


// TRANSPONOVANI
// transponuje matici
// |0,1,2|     |0,3,6|
// |3,4,5|  =  |1,4,7|
// |6,7,8|     |2,5,8|
void MatMaticeTransponuj(matice mat)
{
  matice temp;
  MatMaticeKopni(mat, temp);
  mat[0] = temp[0];
  mat[1] = temp[3];
  mat[2] = temp[6];
  mat[3] = temp[1];
  mat[4] = temp[4];
  mat[5] = temp[7];
  mat[6] = temp[2];
  mat[7] = temp[5];
  mat[8] = temp[8];
}


// KOPIROVANI
// zkopiruje matice
void MatMaticeKopni(matice zdroj, matice cil)
{
  for (int i = 0; i < 9; i++)
    cil[i] = zdroj[i];
}


// NASOBENI
// vynasobi matici s vektorem
// vektor2 = matice * vektor1;
// |a,b,c|   |x|   |ax + by + cz|
// |d,e,f| * |y| = |dx + ey + fz|
// |g,h,i|   |z|   |gx + hy + iz|
void MatMaticeXVektor(matice mat, vektor3D v, vektor3D vysledek)
{
  MatVektorSmaz(vysledek);
  for (int i = 0; i < 3; i++)
  {
    vysledek[X] += mat[i] * v[i];
    vysledek[Y] += mat[i+3] * v[i];
    vysledek[Z] += mat[i+6] * v[i];
  }
}

// vynasobi 2 matice
// vysledek = mat1 * mat2
void MatMaticeXMatice(matice mat1, matice mat2, matice vysledek){
  MatMaticeSmaz(vysledek);
  matice temp;
  MatMaticeKopni(mat2, temp);
  MatMaticeTransponuj(temp);
  for (int i = 0; i < 3; i++){
    vysledek[0] += mat1[i] * temp[i];
    vysledek[1] += mat1[i] * temp[i+3];
    vysledek[2] += mat1[i] * temp[i+6];

    vysledek[3] += mat1[i+3] * temp[i];
    vysledek[4] += mat1[i+3] * temp[i+3];
    vysledek[5] += mat1[i+3] * temp[i+6];

    vysledek[6] += mat1[i+6] * temp[i];
    vysledek[7] += mat1[i+6] * temp[i+3];
    vysledek[8] += mat1[i+6] * temp[i+6];
  }
}


// SKALAR
// proste vynasobi vsechny prvky matice zadanym cislem
void MatMaticeXSkalar(matice mat, double skalar, matice vysledek){
  for (int i = 0; i < 9; i++)
    vysledek[i] = mat[i] * skalar;
}


// DETERMINANT
// vypocita determinant zadane matice
//     |a,b,c|
// det |d,e,f| = a*e*i + b*f*g + c*d*h - c*e*g - a*f*h - b*d*i
//     |g,h,i|
double MatMaticeDeterminant(matice mat)
{
  return mat[0]*mat[4]*mat[8] + mat[1]*mat[5]*mat[6] + mat[2]*mat[3]*mat[7]
       - mat[2]*mat[4]*mat[6] - mat[1]*mat[3]*mat[8] - mat[0]*mat[5]*mat[7];
}


// INVERTOVANI
//                   |A11*A22 - A21*A12, A01*A22 - A21*A02, A01*A12 - A11*A02|
// M^-1 = 1/det(M) * |A10*A22 - A20*A12, A00*A22 - A20*A02, A00*A12 - A10*A02|
//                   |A10*A21 - A20*A11, A00*A21 - A20*A01, A00*A11 - A10*A01|
void MatMaticeInvertuj(matice mat)
{
  double determinant = MatMaticeDeterminant(mat);
  if (determinant == 0.0) exit(1);
  determinant = 1.0 / determinant;
  matice temp;
  MatMaticeKopni(mat, temp);
  mat[0] = temp[4]*temp[8] - temp[5]*temp[7];
  mat[1] = temp[3]*temp[8] - temp[5]*temp[6];
  mat[2] = temp[3]*temp[7] - temp[4]*temp[6];

  mat[3] = temp[1]*temp[8] - temp[2]*temp[7];
  mat[4] = temp[0]*temp[8] - temp[2]*temp[6];
  mat[5] = temp[0]*temp[7] - temp[1]*temp[6];

  mat[6] = temp[1]*temp[5] - temp[2]*temp[4];
  mat[7] = temp[0]*temp[5] - temp[2]*temp[3];
  mat[8] = temp[0]*temp[4] - temp[1]*temp[3];

  MatMaticeXSkalar(mat, determinant, mat);
}




void MatPozicePlusVektor(pozice3D p, vektor3D v, pozice3D vysledek){
  for (int i = 0; i < 3; i++)
    vysledek[i] = p[i] + v[i];
}

void MatPozicePlusVektor(pozice3D p, vektor3D v){
  for (int i = 0; i < 3; i++)
    p[i] = p[i] + v[i];
}




// PRIDANI VEKTORU
// pricte zdrojovy vektor k cilovemu
void MatVektorPridej(vektor3D zdroj, vektor3D cil)
{
  for (int i = 0; i < 3; i++)
    cil[i] += zdroj[i];
}



void MatPoleKopniff(float *zdroj, float *cil, int n){
  for (int i = 0; i < n; i++)
    cil[i] = zdroj[i];

}


void MatPoleKopnidf(double *zdroj, float *cil, int n){
  for (int i = 0; i < n; i++)
    cil[i] = zdroj[i];
}

// VYPIS VEKTORU
void MatVektorVypis(vektor3D v)
{
  cout << "(" << v[0] << ", " << v[1] << ", " << v[2] << ")" << endl;
}


// KOPIROVANI VEKTORU
void MatVektorKopni(vektor3D zdroj, vektor3D cil){
  cil[X] = zdroj[X];
  cil[Y] = zdroj[Y];
  cil[Z] = zdroj[Z];
}
// KOPIROVANI POZIC
void MatPoziceKopni(pozice3D zdroj, pozice3D cil){
  cil[X] = zdroj[X];
  cil[Y] = zdroj[Y];
  cil[Z] = zdroj[Z];
}

void MatPoziceKopni(const pozice3D zdroj, pozice3D cil)
{
  cil[X] = zdroj[X];
  cil[Y] = zdroj[Y];
  cil[Z] = zdroj[Z];
}
// POSUNUTI BODU PODLE USECKY
void MatPozicePresun(pozice3D pocatek, vektor3D smer, double vzd, pozice3D vysledek){
  for (int i = 0; i < 3; i++)
    vysledek[i] = pocatek[i] + smer[i]*vzd;
}
// VZDALENOST DVOU BODU
double MatPoziceVzdalenost(pozice3D p1, pozice3D p2){
  return sqrtf(powf(p1[X] - p2[X], 2) + powf(p1[Y] - p2[Y], 2) + powf(p1[Z] - p2[Z], 2));
}
// OPACNY VEKTOR
void MatVektorOpak(vektor3D zdroj, vektor3D cil){
  for (int i = 0; i < 3; i++)
    cil[i] = - zdroj[i];
}

void MatVektorOpak(vektor3D v){
  for (int i = 0; i < 3; i++)
    v[i] = - v[i];
}



// PREVOD MEZI STUPNEMI A RADIANY
double MatStupenRadian(double stupen){
  return PI * stupen / 180.0;
}
double MatRadianStupen(double radian){
  return 180.0 * radian / PI;
}
// NORMALOVY VEKTOR
void MatVektorNormal(vektor3D v1, vektor3D v2, vektor3D cil){
  cil[X] = v1[Y] * v2[Z] - v2[Y] * v1[Z];
  cil[Y] = v1[Z] * v2[X] - v2[Z] * v1[X];
  cil[Z] = v1[X] * v2[Y] - v2[X] * v1[Y];
}
// VELIKOST VEKTORU
double MatVektorVelikost(vektor3D v){
  double vysledek = 0.0;
  for (int i = 0; i < 3; i++)
    vysledek += powf(v[i],2);
  return sqrtf(vysledek);
}
// JEDNOTKOVY VEKTOR
void MatVektorJednot(vektor3D v, vektor3D cil){
  double vel = MatVektorVelikost(v);
  if (vel == 0.0) {
    MatVektorSmaz(cil);
    return;
  }
  for (int i = 0; i < 3; i++)
    cil[i] = v[i] / vel;
}

void MatVektorJednot(vektor3D v){
  double vel = MatVektorVelikost(v);
  if (vel == 0.0) return;
  for (int i = 0; i < 3; i++)
    v[i] = v[i] / vel;
}


// SMAZANI
void MatVektorSmaz(vektor3D v){
  v[X] = 0.0;
  v[Y] = 0.0;
  v[Z] = 0.0;
}
void MatPoziceSmaz(pozice3D v){
  v[X] = 0.0;
  v[Y] = 0.0;
  v[Z] = 0.0;
}

// MINIMALNI PRVEK
double MatPoleMin(double *pole, int n){
  double minhodnota = 1000.0;
  for (int i = 0; i < n; i++)
    if (pole[i] < minhodnota) minhodnota = pole[i];
  return minhodnota;
}

// PRUMER POZIC
void MatPozicePrumer(pozice3D p1, pozice3D p2, pozice3D cil){
  for (int i = 0; i < 3; i++)
    cil[i] = (p1[i] + p2[i])/2.0;
}

// prusecik primky a roviny
void MatRovinaPrusecik(double *rovina, pozice3D bod, vektor3D vektor, pozice3D vysledek){
  double citatel =  - (rovina[0]*bod[X] + rovina[1]*bod[Y] + rovina[2]*bod[Z] + rovina[3]);
  double jmenovatel = rovina[0]*vektor[X] + rovina[1]*vektor[Y] + rovina[2]*vektor[Z];
  double t;
  if (jmenovatel == 0.0) {
    vysledek[X] = bod[X];
    vysledek[Y] = bod[Y];
    vysledek[Z] = -1000000.0;
    return;
  }
  t = citatel/jmenovatel;
  vysledek[X] = bod[X] + vektor[X] * t;
  vysledek[Y] = bod[Y] + vektor[Y] * t;
  vysledek[Z] = bod[Z] + vektor[Z] * t;
}

// UHEL
double MatVektorUhel(vektor3D v){
  vektor3D n;
  MatVektorKopni(v, n);
  n[2] = 0.0;
  MatVektorJednot(n);
  double uhel = asin(fabs(n[Y]));
  if ((n[X] >= 0.0) && (n[Y] >= 0.0)) return uhel;               // 1. kvadrant
  if ((n[X] <= 0.0) && (n[Y] >= 0.0)) return PI - uhel;          // 2. kvadrant
  if ((n[X] <= 0.0) && (n[Y] <= 0.0)) return PI + uhel;          // 3. kvadrant
  if ((n[X] >= 0.0) && (n[Y] <= 0.0)) return (2.0*PI) - uhel;    // 4. kvadrant
  return 0.0;
}

// UHEL DVOU VEKTORU
double MatVektorUhel(vektor3D v1, vektor3D v2){
  double soucin = 0.0;
  double velikost1 = 0.0;
  double velikost2 = 0.0;
  double vysledek = 0.0;
  for (int i = 0; i < 3; i++)
    soucin += v1[i] * v2[i];
  for (int i = 0; i < 3; i++)
    velikost1 += powf(v1[i], 2);
  for (int i = 0; i < 3; i++)
    velikost2 += powf(v2[i], 2);
  if ((velikost1 == 0.0) || (velikost2 == 0.0)) return 0.0;
  double temp = soucin / (sqrtf(velikost1) * sqrtf(velikost2));
  if (temp >= 1.0) return 0.0;
  if (temp <= -1.0) return PI;
  vysledek = acos(temp);
  if (vysledek > PI) return PI;
  if (vysledek < -PI) return -PI;
  return vysledek;
}


// ROTACE
void MatVektorRotace(vektor3D v, double uhel, int osa){
  vektor3D temp;
  MatVektorKopni(v, temp);
  if (osa == X){
    v[X] = temp[X];
    v[Y] = temp[Y] * cos(uhel) - temp[Z] * sin(uhel);
    v[Z] = temp[Y] * sin(uhel) + temp[Z] * cos(uhel);
  }
  if (osa == Y){
    v[X] = temp[X] * cos(uhel) + temp[Z] * sin(uhel);
    v[Y] = temp[Y];
    v[Z] = -temp[X] * sin(uhel) + temp[Z] * cos(uhel);
  }
  if (osa == Z){
    v[X] = temp[X] * cos(uhel) - temp[Y] * sin(uhel);
    v[Y] = temp[X] * sin(uhel) + temp[Y] * cos(uhel);
    v[Z] = temp[Z];
  }
}

// SCITANI VEKTORU
void MatVektorSecti(vektor3D v1, vektor3D v2, vektor3D cil){
  for (int i = 0; i < 3; i++)
    cil[i] = v1[i] + v2[i];
}

// NASOBENI VEKTORU SKALAREM
void MatVektorNasob(vektor3D v, double cislo){
  for (int i = 0; i < 3; i++)
    v[i] *= cislo;
}

// TVORBA VEKTORU
void MatPoziceVektor(pozice3D p1, pozice3D p2, vektor3D cil){
  for (int i = 0; i < 3; i++)
    cil[i] = p1[i] - p2[i];
}
//
// VYPOCET PARAMETRU D ROVNICE ROVINY
void MatRovina(vektor3D normala, pozice3D bod, double *rovnice){
  double d = - (normala[0] * bod[0] + normala[1] * bod[1] + normala[2] * bod[2]);
  for (int i = 0; i < 3; i++)
    rovnice[i] = normala[i];
  rovnice[3] = d;
}

// ZJISTENI ZNAMENKA
bool MatFloatKladne(double cislo){
  if (cislo > 0.0) return true;
  if (cislo < 0.0) return false;
  return false;
}



// BAZE
void MatBaze::Reset(bool smaz_pozici){
  if (smaz_pozici) MatPoziceSmaz(pozice);
  MatVektorSmaz(nahoru);
  MatVektorSmaz(doleva);
  MatVektorSmaz(dopredu);
  dopredu[X] = 1.0;
  doleva[Y] = 1.0;
  nahoru[Z] = 1.0;
}

void MatBaze::RotaceX(double uhel){
  MatVektorRotace(dopredu, uhel, X);
  MatVektorRotace(doleva, uhel, X);
  MatVektorRotace(nahoru, uhel, X);
}

void MatBaze::RotaceY(double uhel){
  MatVektorRotace(dopredu, uhel, Y);
  MatVektorRotace(doleva, uhel, Y);
  MatVektorRotace(nahoru, uhel, Y);
}

void MatBaze::RotaceZ(double uhel){
  MatVektorRotace(dopredu, uhel, Z);
  MatVektorRotace(doleva, uhel, Z);
  MatVektorRotace(nahoru, uhel, Z);
}

void MatBaze::Nacti(MatBaze *baze2){
  MatVektorKopni(baze2->nahoru, nahoru);
  MatVektorKopni(baze2->doleva, doleva);
  MatVektorKopni(baze2->dopredu, dopredu);
  MatPoziceKopni(baze2->pozice, pozice);
}











// INTERPOLACE
// z n-prvkoveho pole vypocita hodnotu na pozici "index"
// pokud je index zaporny, pocita se jako by byl kladny, ale nakonci se neguje vysledek interpolace
// v pripade, ze index prekracuje pocet polozek pole, pouzije se hodnota posledni polozky pole
//
// vysledek - sem se ulozi vysledek cele operace
// zn - znamenko indexu
// poz - absolutni hodnota indexu
// poz0 - index leveho prvku pole od indexu
// poz1 - index praveho prvku pole od indexu
// vektor - vektor od poz0 k poz1
// rovnice - parametry obecne rovnice primky
//
// vypocet:
// najde se obecna rovnice usecky mezi poz0 a poz1
// 0 = ax + by + c
// do te se potom dosadi misto x hodnota indexu, y je vysledek
//         ax + c
// y = -  --------
//            b

double MatPoleInterpoluj(double *pole, int n, double index){
  // priprava promennych
  double vysledek = 0.0;
  double zn = znamenko(index);
  double poz = fabs(index);
  int poz0 = (int)floor(poz);
  int poz1 = (int)ceil(poz);
  if (poz0 == poz1) poz1++;
  vektor2D vektor;
  double rovnice[3];

  // vypocet
  if (poz1 >= n) {
    vysledek = zn * pole[n-1];
    return vysledek;
  }
  vektor[X] = (double)(poz1 - poz0);
  vektor[Y] = pole[poz1] - pole[poz0];
  rovnice[0] = - vektor[Y];
  rovnice[1] = vektor[X];
  if (rovnice[1] == 0.0) exit(1); // nemuze nastat, usecka by musela byt ve smeru osy Y
  rovnice[2] = - rovnice[0] * (double)poz0 - rovnice[1] * pole[poz0];  // c = -ax -by
  vysledek = - (rovnice[0] * poz + rovnice[2])/rovnice[1];
  return zn * vysledek;
}











// ***   PRUMER   ***
double prumer(double x, double y){
  return ((x + y) / 2.0);
}


// ***   UHEL   ***
double uhelr(double z1, double z2, double prepona){
  double rozdil = z2 - z1;
  if (rozdil > prepona) rozdil = prepona;
  if (rozdil < -prepona) rozdil = -prepona;
  return  asin(rozdil / prepona);
}


// ***   RADIANY   ***
double radiany(double stupne){
  return PI * stupne / 180.0;
}


// ***   STUPNE   ***
double stupne(double radiany){
  return 180.0 * radiany / PI;
}


// ***   VZDALENOST   ***
double vzdalenost(double *bod1, double *bod2){
  return sqrtf(powf(bod1[0] - bod2[0],2) + powf(bod1[1] - bod2[1],2) + powf(bod1[2] - bod2[2],2));
}
double vzdalenost2d(double *bod1, double *bod2){
  return sqrtf(powf(bod1[0] - bod2[0],2) + powf(bod1[1] - bod2[1],2));
}

// ***   POLE   ***
void kopni_pole(double *zdroj, double *cil, int pocet){
  for (int i = 0; i < pocet; i++)
    cil[i] = zdroj[i];
  return;
}
void kopni_pole(int *zdroj, int *cil, int pocet){
  for (int i = 0; i < pocet; i++)
    cil[i] = zdroj[i];
  return;
}


// ***   RANDOM   ***
double random(double max){
  double MAX = RAND_MAX;
  return (max / MAX) * rand();

}

int randomi(int max){
  double cislo = random(((double)max) + 0.9);
  return (int) cislo;
}



void rotace(double *v, double uhel, int osa){
  int X = 0;
  int Y = 1;
  int Z = 2;
  double temp[3];
  kopni_pole(v, temp, 3);
  if (osa == X){
    v[X] = temp[X];
    v[Y] = temp[Y] * cos(uhel) - temp[Z] * sin(uhel);
    v[Z] = temp[Y] * sin(uhel) + temp[Z] * cos(uhel);
  }
  if (osa == Y){
    v[X] = temp[X] * cos(uhel) + temp[Z] * sin(uhel);
    v[Y] = temp[Y];
    v[Z] = -temp[X] * sin(uhel) + temp[Z] * cos(uhel);
  }
  if (osa == Z){
    v[X] = temp[X] * cos(uhel) - temp[Y] * sin(uhel);
    v[Y] = temp[X] * sin(uhel) + temp[Y] * cos(uhel);
    v[Z] = temp[Z];
  }
}

void opacny_vektor(double *v){
  v[0] = v[0] * -1.0;
  v[1] = v[1] * -1.0;
  v[2] = v[2] * -1.0;
}


void normala(double *v1, double *v2, double *cil){
  cil[0] = v1[1] * v2[2] - v2[1] * v1[2];
  cil[1] = v1[2] * v2[0] - v2[2] * v1[0];
  cil[2] = v1[0] * v2[1] - v2[0] * v1[1];
}



double uhelv(double *vektor1, double *vektor2, int n){
  double soucin = 0.0;
  double velikost1 = 0.0;
  double velikost2 = 0.0;
  double vysledek = 0.0;
  for (int i = 0; i < n; i++)
    soucin += vektor1[i] * vektor2[i];
  for (int i = 0; i < n; i++)
    velikost1 += powf(vektor1[i], 2);
  for (int i = 0; i < n; i++)
    velikost2 += powf(vektor2[i], 2);
  if ((velikost1 == 0.0) || (velikost2 == 0.0)) return 0.0;
  sqrtf(velikost1);
  sqrtf(velikost2);
  double temp = soucin / (velikost1 * velikost2);
  if (temp >= 1.0) return 0.0;
  if (temp <= -1.0) return PI;

  vysledek = acos(temp);
  if (vysledek > PI) return PI;
  if (vysledek < -PI) return -PI;
  return vysledek;


}




double kmh(double ms){
  return ms * 3.6;
}
double ms(double kmh){
  return kmh / 3.6;
}

double znamenko(double cislo){
  if (cislo == 0.0) return 0.0;
  return cislo / fabs(cislo);
}


double velikost_vektoru(double *vektor, int n){
  double velikost = 0.0;
  for (int i = 0; i < n; i++)
    velikost += powf(vektor[i], 2);
  return sqrtf(velikost);

}

void normalizuj(double *vektor, int n){
  double v[n];
  double velikost = velikost_vektoru(vektor, n);
  if (velikost == 0.0) return;
  kopni_pole(vektor, v, n);

  for (int i = 0; i < n; i++)
    vektor[i] = v[i] / velikost;
}


double rovina(double *bod1,double *bod2,double *bod3,double *bod4){

  // 0 = ax + by + cz + d
  // d = -ax - by - cz
  // z = (-ax - by - d)/c


  double a;
  double b;
  double c;
  double d;
  double z;
  double v1[3];
  double v2[3];
  double n[3];

  for (int i = 0; i < 3; i++){
    v1[i] = bod2[i] - bod1[i];
    v2[i] = bod3[i] - bod2[i];
  }

  normala(v1, v2, n);

  a = n[0];
  b = n[1];
  c = n[2];
  d = - a*bod1[0] - b*bod1[1] - c*bod1[2];

  if (c == 0.0) return 0.0;

  z = (- a*bod4[0] - b*bod4[1] - d) / c;

  return z;
}


void smaz_pole(int *pole, int n){
  for (int i = 0; i < n; i++) pole[i] = 0;
}


void smaz_pole(double *pole, int n){
  for (int i = 0; i < n; i++) pole[i] = 0.0;
}



int min(int x, int y){
  if (x < y) return x;
  return y;
}


int max(int x, int y){
  if (x > y) return x;
  return y;
}

int iabs(int n){
  if (n < 0) return -n;
  return n;
}




int xy_to_i(int x, int y, int sirka)                     // prevede souradnice 2 rozmerneho pole na souradnici 1 rozmerneho pole
{
  return y * sirka + x;
}



double MatMax(double x, double y){
  if (x > y) return x;
  return y;
}

