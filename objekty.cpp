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


#include <stdlib.h>
#include <string.h>
#include <iostream>
using namespace std;

#include "matika.h"
#include "teren.h"
#include "vstup.h"
#include "objekty.h"
#include "loaddata.h"
#include "pruziny.h"

#include "3Dmath/Mat3D.h"
#include "3Dmath/Vec3D.h"

//------------------------------------------------------------------------------
//                           ***   KONSTANTY   ***
//------------------------------------------------------------------------------

const int pocet_objektu = 6;
const double nastupni_vzd = 3.0;
const double VIDITELNOST = 130.0;

//------------------------------------------------------------------------------
//                           ***   KONSTRUKTOR   ***
//------------------------------------------------------------------------------


objekty::objekty(int akt_p, teren *ter, keys *kl){
  prvni_cyklus = true;
  pozice3D poz0 = {-40.0, 90.0, 18.0};
  pozice3D poz1 = {-40.0, 101.5, 18.0};
  pozice3D poz2 = {-7.5, 101.5, 18.0};
  pozice3D poz3 = {-5.0, 101.5, 18.0};
  pozice3D poz4 = {-2.0, 101.5, 18.0};
  pozice3D poz5 = {-90.0, 105.5, 18.0};
  pozice3D poz6 = { 2.0, 98.0, 18.0};

  t = ter;
  klavesy = kl;
  chodec = new tchodec(kl, t, poz0, -45);
  prostredek[0] = new vehikl(TYP_KAROSA, poz6, -90, t);
  prostredek[1] = new vehikl(TYP_SKODAOCT, poz2, -90, t);
  prostredek[2] = new vehikl(TYP_TOYOTA, poz3, -90, t);
  prostredek[3] = new vehikl(TYP_SKODAOCTTURBO, poz4, -90, t);
  prostredek[4] = new vehikl(TYP_SKODAFAV, poz5, 0, t);
  prostredek[5] = new vehikl(TYP_TOYOTA, poz1, 0, t);

  aktivni = nic_aktivni;
  pesky = true;
  MatPoziceSmaz(pozice_kamery);
    prvni_cyklus = true;
 }

void objekty::DeInit()
{
  for (int i = 0; i < pocet_objektu; i++)
  {
    delete prostredek[i];
    prostredek[i] = NULL;
  }
  delete chodec;
  chodec = NULL;
  pesky = true;
  aktivni = nic_aktivni;
}


//------------------------------------------------------------------------------
//                              ***   CYKLUS   ***
//------------------------------------------------------------------------------


void objekty::cyklus(pozice3D kamera){
  MatPoziceKopni(kamera, pozice_kamery);

  for (int i = 0; i < pocet_objektu; i++)
    if (i == aktivni) prostredek[i]->Cyklus(klavesy);
    else
    {
      if (MatPoziceVzdalenost(pozice_kamery, prostredek[i]->Pozice()) < VIDITELNOST || prvni_cyklus)
        prostredek[i]->Cyklus(NULL);
    }


  matice mat;
  Matice(mat);
  chodec->cyklus(pesky, Pozice_vystupu(), mat);
  kolize();

  prvni_cyklus = false;
  }


//------------------------------------------------------------------------------
//                              ***   KRESLI  ***
//------------------------------------------------------------------------------



void objekty::kresli(const Vec3D& cameraPos)
{
  for (int i = 0; i < drawObjects.size(); i++)
  {
    drawObjects[i].Draw(cameraPos, t->poziceSlunce());
  }

  for (int i = 0; i < pocet_objektu; i++)
  {
    if (MatPoziceVzdalenost(pozice_kamery, prostredek[i]->Pozice()) < VIDITELNOST)
    {
      prostredek[i]->DrawDiag();
    }
  }

  if (pesky)
    chodec->DrawDiag();
}


void objekty::PrepareForDrawing()
{
  drawObjects.clear();

  for (int i = 0; i < pocet_objektu; i++)
  {
    if (MatPoziceVzdalenost(pozice_kamery, prostredek[i]->Pozice()) < VIDITELNOST)
    {
      prostredek[i]->GetDrawObjects(drawObjects);
    }
  }
}

//------------------------------------------------------------------------------
//                           ***   KOLIZE   ***
//------------------------------------------------------------------------------


const double TVRDOST = 500000.0;
const double TLUMENI = 50000.0;

void objekty::kolize(){


}


//------------------------------------------------------------------------------
//                           ***   NASTOUPENI   ***
//------------------------------------------------------------------------------




void objekty::nastup(){
  // Vystoupeni
  if (!pesky){
    if (Spd() < 20.0) {
      pesky = true;
      aktivni = nic_aktivni;
      klavesy->delta_uhelxy = 0.0;
    }
    return;
  }

  // Nastoupeni?
  double min = 99999.0;
  int polozka = nic_aktivni;
  double vzd = 99999.0;

  // nalezeni nejblizsiho objektu
  for (int i = 0; i < pocet_objektu; i++){
    vzd = MatPoziceVzdalenost(chodec->Pozice(), prostredek[i]->Pozice_vystupu());
    if (vzd < min) {
      min = vzd;
      polozka = i;
    }
  }

  // pokud je nejblizsi objekt dost blizko, nastoupi do nej
  if (min < nastupni_vzd){
    pesky = false;
    aktivni = polozka;
  }
}




//------------------------------------------------------------------------------
//                             ***   OSTATNI   ***
//------------------------------------------------------------------------------


double objekty::Spd(){
  if (pesky) return 0.0;
  return prostredek[aktivni]->Spd();
}

double objekty::Uz(){
  return 0.0;
}

double *objekty::Pozice(){
  if (pesky) return chodec->Pozice();
  return prostredek[aktivni]->Pozice();
}
double *objekty::Pozice_ridice(){
  if (pesky) return chodec->Pozice();
  return prostredek[aktivni]->Pozice_ridice();
}
double *objekty::Pozice_vystupu(){
  if (pesky) return chodec->Pozice();
  return prostredek[aktivni]->Pozice_vystupu();
}
double objekty::Ot(){
  if (pesky) return 0.0;
  return prostredek[aktivni]->Otacky();
}

int objekty::St(){
  if (pesky) return 0;
  return prostredek[aktivni]->Stupen();
}

float objekty::Zataceni()
{
  if (pesky) return 0.0;
  return prostredek[aktivni]->Zataceni();
}

double objekty::Plyn(){
  return 0;
}

double objekty::Alt(){
  return 0.0;
}

double objekty::Deltaz(){
  return 0.0;
}

double objekty::Un(){
  return 0.0;
}

bool objekty::Cam(){
  if (pesky) return false;
  return true;
}

void objekty::Matice(matice mat){
  if (pesky){
    MatMaticeKopni(chodec->matice_rotace, mat);
    return;
  }
  double m[9];
  prostredek[aktivni]->R.transpose().toArray(m);
  MatMaticeKopni(m, mat);
}


int objekty::PocetObjektu()
{
  return pocet_objektu;
}
