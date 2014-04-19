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


#include "obal.h"


// konstruktor
CObal::CObal()
{
  top = NULL;
}

CObal::~CObal()
{
  top = NULL;
  vector <CKoule*>::const_iterator it;
  for (it=obsah.begin(); it != obsah.end(); it++)
  {
    CKoule* koule = *it;
    delete koule;
  }
  obsah.clear();
}

// NajdiKouli
CKoule *CObal::NajdiKouli(int ID)
{
  vector <CKoule*>::iterator it;
  for (it=obsah.begin(); it != obsah.end(); it++)
    if ((*it)->DejID() == ID) return *it;
  return NULL;
}

// NastavObal
void CObal::NastavObal(matice R, pozice3D pozice_objektu)
{
  vector <CKoule*>::iterator it;
  for (it=obsah.begin(); it != obsah.end(); it++)
    (*it)->NastavPozici(R, pozice_objektu);
}

// KresliObal
void CObal::KresliObal(bool listy) const
{
  vector <CKoule*>::const_iterator it;
  for (it=obsah.begin(); it != obsah.end(); it++)
    (*it)->Kresli(listy);
}

//!  Rekurzivni detekce kolize
/*!  Pokud ke kolizi doslo, zavola sama sebe na detekci mezi vsemi potomky 2 a kouli 1
    \param koule1 ukazatel na prvni kouli
    \param koule2 ukazatel na druhou kouli
    \param prusecik sem se ulozi soucet pozice vsech pruseciku mezi listy hierarchii
    \param smer sem se ulozi soucet smeru vsech kolidujicich kouli
    \param hloubka sem se ulozi soucet hloubky pruniku vsech kolidujicich kouli
    \param pocet sem se ulozi pocet kolidujicich kouli
    \return true pokud doslo ke kolizi
*/
bool CObal::RekurzivniDetekce(CKoule *koule1, CKoule *koule2, pozice3D bod1, pozice3D bod2, int &pocet)
{
  pozice3D _bod1;
  pozice3D _bod2;
  bool ret = false;
  bool list;
  if (koule1->Kolize(koule2, _bod1, _bod2, list))
  {
    if (list)
    {
      MatVektorPridej(_bod1, bod1);
      MatVektorPridej(_bod2, bod2);
      pocet++;
      ret = true;
    }
    CKoule *temp = koule2->DejObsah();
    while (temp != NULL)
    {
      ret = ret + RekurzivniDetekce(temp, koule1, bod1, bod2, pocet);
      temp = temp->DejDalsi();
    }
  }
  return ret;
}

bool CObal::KolizeObalu(CObal *obal2, pozice3D bod1, pozice3D bod2)
{
  pozice3D _bod1;
  pozice3D _bod2;
  int pocet = 0;
  MatPoziceSmaz(_bod1);
  MatPoziceSmaz(_bod2);
  bool ret = RekurzivniDetekce(top, obal2->top, _bod1, _bod2, pocet);

  for (int i = 0; i < 3; i++)
  {
    bod1[i] = _bod1[i] / ((double)(pocet));
    bod2[i] = _bod2[i] / ((double)(pocet));
  }
  return ret;
}


// InitObal
void CObal::InitObal(obal_data *data)
{
  vector <obal_koule>::iterator it;
  int ID;
  double polomer;
  pozice3D pozice;
  int rodicID;
  CKoule *rodic;
  for (it= data->hierarchie_obalu.begin(); it != data->hierarchie_obalu.end(); it++)
  {
    ID = it->ID;
    MatVektorKopni(it->pozice, pozice);
    rodicID = it->rodic;
    polomer = it->polomer;
    CKoule *koule = new CKoule(ID, pozice, polomer);
    rodic = NajdiKouli(rodicID);
    if (rodic != NULL) rodic->PridejObsah(koule);
      else top = koule;
    obsah.push_back(koule);
  }
}




// pozice
void CObal::DejPoziciObalu(pozice3D pozice)
{
  if (top == NULL) return;
  top->DejPozici(pozice);
}


// polomer
double CObal::DejPolomer()
{
  if (top == NULL) return 0.0;
  return top->DejPolomer();
}







