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



#include "3Dmath/Vec3D.h"

#include "matika.h"
#include "teren.h"
#include "pruziny.h"
#include "vstup.h"
#include "objekt.h"
#include <iostream>
#include <stdlib.h>
using namespace std;


const bool BBOX = false; // jestli se zobrazi bounding box kolem objektu

const double TLUMENI_RYCHLOSTI = 0.001;
const double TLUMENI_ROTACE = 0.1;

const double TLUMENI_NARAZU = 5000.0;
const double SILA_NARAZU = 200000.0;




const int Ixx = 0;
const int Iyy = 4;
const int Izz = 8;

//------------------------------------------------------------------------------
//                       ***   KONSTRUKTOR   ***
//------------------------------------------------------------------------------

objekt::objekt(pozice3D _pozice, float azimut)
{
  DL = 0;
  init_ok = false;
  bod = Vec3D(_pozice);
  CELKOVA_HMOTNOST = 0.0;
  M_NA_MINUS_PRVNI = 0.0;

  Q.Set(Vec3D(0,0,1), azimut * M_PI / 180.0f);
  R = Q.computeMatrix();
  inv_R = R.invert();
  useTexture = false;
  texture = 0;
  kresleni_obalu = 0;
}

void objekt::DrawDiag() const
{
  if (kresleni_obalu == 1) KresliObal(false);
  if (kresleni_obalu == 2) KresliObal(true);
}

//------------------------------------------------------------------------------
//                           ***   INIT   ***
//------------------------------------------------------------------------------


// Data se budou nacitat ze souboru az PO vytvoreni objektu, proto tato funkce neni v konstruktoru.
void objekt::Init(double sirka, double delka, double vyska, double *hmotnost_bodu, teren *_t){
  t = _t;
  // vypocet celkove hmotnosti
  for (int n = 0; n < 8; n++){
    HMOTNOST[n] = hmotnost_bodu[n];
    CELKOVA_HMOTNOST += hmotnost_bodu[n];
  }
  if (CELKOVA_HMOTNOST == 0.0) exit(1);
  M_NA_MINUS_PRVNI = 1.0 / CELKOVA_HMOTNOST;

  // vypocet pozic bodu kvadru
  Vec3D offset[8] = {Vec3D(-0.5, 0.5, -0.5), Vec3D(-0.5, -0.5, -0.5), Vec3D(0.5, -0.5, -0.5), Vec3D(0.5, 0.5, -0.5),
                              Vec3D(-0.5, 0.5, 0.5),  Vec3D(-0.5, -0.5, 0.5),  Vec3D(0.5, -0.5, 0.5),  Vec3D(0.5, 0.5, 0.5)};

  Vec3D rozmery = Vec3D(delka, sirka, vyska);
  for (int n = 0; n < 8; n++){
      body[n] = bod + offset[n] * rozmery;
  }


  // vypocet teziste
  x.clear();
  for (int n = 0; n < 8; n++)
      x += body[n] * HMOTNOST[n] * M_NA_MINUS_PRVNI;

  // vypocet relativnich pozic bodu vzhledem k tezisti (kvuli snadnemu prepocitani pozic bodu pri zmene rotace)
  for (int n = 0; n < 8; n++)
     body_ts[n] = body[n] - x;

  // vypocet matice setrvacnosti (moment of inertia matrix)
  MatMaticeSmaz(SETRVACNOST);
   // verze s tezistem
  for (int n = 0; n < 8; n++){
    SETRVACNOST[Ixx] += 1.0*(powf(body_ts[n].getY(), 2) + powf(body_ts[n].getZ(), 2)) * HMOTNOST[n];
    SETRVACNOST[Iyy] += 1.0*(powf(body_ts[n].getX(), 2) + powf(body_ts[n].getZ(), 2)) * HMOTNOST[n];
    SETRVACNOST[Izz] += 1.0*(powf(body_ts[n].getX(), 2) + powf(body_ts[n].getY(), 2)) * HMOTNOST[n];
  }

  IyyMinusIzzDelenoIxx = (SETRVACNOST[Iyy] - SETRVACNOST[Izz]) / SETRVACNOST[Ixx];
  IzzMinusIxxDelenoIyy = (SETRVACNOST[Izz] - SETRVACNOST[Ixx]) / SETRVACNOST[Iyy];
  IxxMinusIyyDelenoIzz = (SETRVACNOST[Ixx] - SETRVACNOST[Iyy]) / SETRVACNOST[Izz];

  // konstanty tlumeni
  LIN_TLUMENI = - TLUMENI_RYCHLOSTI * CELKOVA_HMOTNOST;
  ROT_TLUMENI[X] = - TLUMENI_ROTACE * SETRVACNOST[Ixx];
  ROT_TLUMENI[Y] = - TLUMENI_ROTACE * SETRVACNOST[Iyy];
  ROT_TLUMENI[Z] = - TLUMENI_ROTACE * SETRVACNOST[Izz];

  init_ok = true;


}






//------------------------------------------------------------------------------
//                       ***   VypocitejBB   ***
//------------------------------------------------------------------------------

// prepocita body bounding boxu z matice rotace a pozice stredu
// vypocita take rovnice rovin sten BB

void objekt::VypocitejBB(){

  // pozice bodu
  for (int n = 0; n < 8; n++){
      body[n] = x + R * body_ts[n];
  }

  // pozice geom. stredu
  bod.clear();
  for (int n = 0; n < 8; n++){
      bod += body[n];
  }
  bod *= 1.0 / 8.0;

/*
  // rovnice rovin
  double* n1, n2, n3;
  n1 = R.getE1().toArray();
  n2 = R.getE2().toArray();
  n3 = R.getE3().toArray();

  double* b1, b2, b3, b4, b5, b6;


  MatRovina(n1, body[0], roviny[0]);
  MatRovina(n1, body[2], roviny[1]);
  MatRovina(n2, body[1], roviny[2]);
  MatRovina(n2, body[0], roviny[3]);
  MatRovina(n3, body[4], roviny[4]);
  MatRovina(n3, body[0], roviny[5]);

  delete n1, n2, n3;
 */

}


/*

//------------------------------------------------------------------------------
//                       ***   VypocitejQ   ***
//------------------------------------------------------------------------------


// Updatuje QUATERNION z rychlosti rotace a puvodniho Q
// puvodni Q musi byt ulozen na jinem miste v pameti!
void objekt::VypocitejQ(vektor3D _w, quaternion _q, double krok)
{

  Q[W] -= krok * (_q[X] * _w[X] + _q[Y] * _w[Y] + _q[Z] * _w[Z]);
  Q[X] += krok * (_q[W] * _w[X] - _q[Z] * _w[Y] + _q[Y] * _w[Z]);
  Q[Y] += krok * (_q[Z] * _w[X] + _q[W] * _w[Y] - _q[X] * _w[Z]);
  Q[Z] += krok * (_q[X] * _w[Y] - _q[Y] * _w[X] + _q[W] * _w[Z]);

}

*/


//------------------------------------------------------------------------------
//                       ***   VypocitejDW   ***
//------------------------------------------------------------------------------

// vypocita zmenu rychlosti rotace z puvodni rychlosti rotace a pusobici sily
Vec3D objekt::VypocitejDW(double krok){
    Vec3D vysledek;
    vysledek.setX(krok * (w.getY() * w.getZ() * IyyMinusIzzDelenoIxx + rot_sila.getX() / SETRVACNOST[Ixx]));
    vysledek.setY(krok * (w.getZ() * w.getX() * IzzMinusIxxDelenoIyy + rot_sila.getY() / SETRVACNOST[Iyy]));
    vysledek.setZ(krok * (w.getX() * w.getY() * IxxMinusIyyDelenoIzz + rot_sila.getZ() / SETRVACNOST[Izz]));
    return vysledek;
}

// KRESLI
void objekt::Kresli(const Vec3D& kamera){
  KresliObjekt();
}

// CYKLUS
void objekt::Cyklus(keys *klavesy){
}

// KRESLI_OBJEKT
void objekt::KresliObjekt(){
}


void objekt::GetDrawObjects(std::vector<DrawObject>& objectList) const
{
  DrawObject obj;
  obj.dl = DL;
  obj.orient = R;
  obj.pos = bod;
  obj.shader = shader;
  obj.useEmissive = false;
  obj.useColor = false;
  obj.texture = useTexture ? texture : -1;
  objectList.push_back(obj);
}

//------------------------------------------------------------------------------
//                       ***   CYKLUS   ***
//------------------------------------------------------------------------------

// RK0
// provede zalohu stavovych promennych
void objekt::RK0(){
    x0 = x;
    v0 = v;
    w0 = w;
    Q.normalize();
    Q0 = Q;

  //Tlumeni();
}

// RK1
// prvni krok integrace
void objekt::RK1(){
    // zrychleni ze sily

    Tlumeni(KROK2);
    dv1 = lin_sila * M_NA_MINUS_PRVNI * KROK2;
    dw1 = VypocitejDW(KROK2);
    lin_sila.clear();
    rot_sila.clear();
    v1 = v;
    w1 = w;

    // zmena linearni pozice a orientace
    x += v1 * KROK2;
    Q = Q0;
    Q.rotate(w1, KROK4);
    Q.normalize();
    R = Q.computeMatrix();
    inv_R = R.invert();


    VypocitejBB();

    // zmena rychlosti
    v += dv1;
    w += dw1;

    double mat[9];
    double b[3];
    R.transpose().toArray(mat);
    bod.toArray(b);
    NastavObal(mat, b);

    Kolize();

}



// RK2
// druhy krok integrace
void objekt::RK2(){
    Tlumeni(KROK2);

    dv2 = lin_sila * M_NA_MINUS_PRVNI * KROK2;
    dw2 = VypocitejDW(KROK2);
    lin_sila.clear();
    rot_sila.clear();
    v2 = v;
    w2 = w;

    // zmena linearni pozice a orientace
    x = x0 + v2 * KROK2;
    Q = Q0;
    Q.rotate(w2, KROK4);
    Q.normalize();
    R = Q.computeMatrix();
    inv_R = R.invert();

    VypocitejBB();

    // zmena rychlosti
    v = v1 + dv2;
    w = w1 + dw2;

    double mat[9];
    double b[3];
    R.transpose().toArray(mat);
    bod.toArray(b);
    NastavObal(mat, b);

    Kolize();
}

// RK3
// treti krok integrace
void objekt::RK3(){
    Tlumeni(KROK);

    dv3 = lin_sila * M_NA_MINUS_PRVNI * KROK;
    dw3 = VypocitejDW(KROK);
    lin_sila.clear();
    rot_sila.clear();
    v3 = v;
    w3 = w;

    // zmena linearni pozice a orientace
    x = x0 + v3 * KROK;
    Q = Q0;
    Q.rotate(w3, KROK2);
    Q.normalize();
    R = Q.computeMatrix();
    inv_R = R.invert();


    VypocitejBB();

    // zmena rychlosti
    v = v1 + dv3;
    w = w1 + dw3;

    double mat[9];
    double b[3];
    R.transpose().toArray(mat);
    bod.toArray(b);
    NastavObal(mat, b);

    Kolize();
}

// RKFINAL
// vypocita finalni vysledek z predchoyich kroku
void objekt::RKFINAL(){
    Tlumeni(KROK2);

    x = x0 + (v + v1 + (v2 + v3) * 2.0) * KROK6;
    w4 = w3 + w2 + (w + w1) * 0.5;

    Q = Q0;
    Q.rotate(w4, KROK6);
    R = Q.computeMatrix();
    inv_R = R.invert();


    v = v1 + (dv1 + dv2 * 2.0 + dv3 + lin_sila * M_NA_MINUS_PRVNI * KROK2) * (1.0 / 3.0);

    w4.setX(w1.getX() + (dw1.getX() + 2.0 * dw2.getX() + dw3.getX() + ((w.getY() * w.getZ() * IyyMinusIzzDelenoIxx + rot_sila.getX() / SETRVACNOST[Ixx]) * KROK2)) / 3.0);
    w4.setY(w1.getY() + (dw1.getY() + 2.0 * dw2.getY() + dw3.getY() + ((w.getZ() * w.getX() * IzzMinusIxxDelenoIyy + rot_sila.getY() / SETRVACNOST[Iyy]) * KROK2)) / 3.0);
    w4.setZ(w1.getZ() + (dw1.getZ() + 2.0 * dw2.getZ() + dw3.getZ() + ((w.getX() * w.getY() * IxxMinusIyyDelenoIzz + rot_sila.getZ() / SETRVACNOST[Izz]) * KROK2)) / 3.0);

    w = w4;

    double mat[9];
    double b[3];
    R.transpose().toArray(mat);
    bod.toArray(b);

    NastavObal(mat, b);

    lin_sila.clear();
    rot_sila.clear();
}




// TLUMENI
// prida silu pusobici proti rychlosti
// prida taky gravitacni silu
void objekt::Tlumeni(double krok){
    lin_sila += v * LIN_TLUMENI;
    rot_sila += w * ROT_TLUMENI;
    lin_sila += - Vec3D(0.0, 0.0, GRAVITACE) * (CELKOVA_HMOTNOST);
}



//------------------------------------------------------------------------------
//                       ***   KOLIZE   ***
//------------------------------------------------------------------------------

bool objekt::Kolize(){
    bool kolize = false;
    for (int n = 0; n < 8; n++){
        if (t->JeTam(body[n])){
            kolize = true;
            Vec3D prusecik;
            Vec3D normal;
            t->Prusecik_n(body[n], &prusecik, &normal);
            normal.normalize();
            Vec3D rychlost = RychlostBodu(body[n]);
            double vzd = body[n].distance(prusecik);

            Vec3D sila = normal * vzd * SILA_NARAZU;
            Vec3D tlumeni =  - rychlost *  TLUMENI_NARAZU;

            PridejSilu(body[n], sila + tlumeni);

            OnCollision();
        }
    }
    return kolize;
}

void objekt::OnCollision()
{
  if (v.length() > 3.0f)
    kolize = true;
}


// PRIDEJSILU
// Prida silu do zasobniku sily objeku
// podle mista pusobeni spocita rotacni slozku
// bod pusobeni i sila jsou ve svetovych souradnicich
void objekt::PridejSilu(const Vec3D& bod_pusobeni, const Vec3D& sila){
  // rotacni sila ve svetovych souradnicich:
  Vec3D smer = bod_pusobeni - x;
  Vec3D torgue = smer.cross(sila);

  // rotacni sila v souradnicich objektu:
  Vec3D torgue_so = inv_R * torgue;

  // pridani sil do zasobniku
  lin_sila += sila;
  rot_sila += torgue_so;
}


// vypocita rychlost libovolneho bodu na objektu
// zadany bod i vysledek jsou ve svetovych souradnicich
Vec3D objekt::RychlostBodu(const Vec3D& bod) const{
  Vec3D Rap = bod - x;
  Vec3D Rap_os = inv_R * Rap;

  Vec3D rot_rych_os = w.cross(Rap_os);
  Vec3D rot_rych = R * rot_rych_os;

  return rot_rych + v;
}



//------------------------------------------------------------------------------
//                       ***   INFO PRO KAMERU atd...   ***
//------------------------------------------------------------------------------


double *objekt::Stred(){
    stred_old[0] = x.getX();
    stred_old[1] = x.getY();
    stred_old[2] = x.getZ();
    return stred_old;
}
double *objekt::Pozice(){
    pozice_old[0] = bod.getX();
    pozice_old[1] = bod.getY();
    pozice_old[2] = bod.getZ();
    return pozice_old;
}
double *objekt::Pozice_ridice(){
    pozice_old[0] = bod.getX();
    pozice_old[1] = bod.getY();
    pozice_old[2] = bod.getZ();
    return pozice_old;

}
double *objekt::Pozice_vystupu(){
    pozice_old[0] = bod.getX();
    pozice_old[1] = bod.getY();
    pozice_old[2] = bod.getZ();
    return pozice_old;

}
double objekt::Spd(){
  return 0.0;
}
double objekt::Otacky(){
  return 0.0;
}

int objekt::Stupen(){
  return 0;
}

double *objekt::V()
{
    rychlost_old[0] = v.getX();
    rychlost_old[1] = v.getY();
    rychlost_old[2] = v.getZ();
    return rychlost_old;
}

