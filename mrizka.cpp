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


#include "mrizka.h"
#include "objekt.h"
#include "pruziny.h"


//-===========================================-
//                 CDVOJICE
//-===========================================-


// nastaveni parametru
void CDvojice::Nastav(CObal *obj1, CObal *obj2, bool fobj)
{
  objekt1 = obj1;
  objekt2 = obj2;
  fyz_obj = fobj;
}


void CDvojice::Nastav(CDvojice &dvojice)
{
  objekt1 = dvojice.objekt1;
  objekt2 = dvojice.objekt2;
  fyz_obj = dvojice.fyz_obj;
}

// kolize
const double TVRDOST = 2000000.0;
const double TLUMENI = 100000.0;
void CDvojice::Kolize()
{
  // test kolize
  pozice3D bod1;
  pozice3D bod2;
  bool kolize = objekt1->KolizeObalu(objekt2, bod1, bod2);

  // osetreni kolize
  if (kolize)
  {
    cpruzina pruzina(TVRDOST, TLUMENI, 0.0);
    vektor3D sila1;
    vektor3D sila2;
    objekt *obj1 = (objekt*)objekt1;
    obj1->OnCollision();
    if (fyz_obj)
    {
      objekt *obj2 = (objekt*)objekt2;
      pruzina.PruzinaCyklus(bod1, bod2, obj1->V(), obj2->V());
      MatVektorKopni(pruzina.pruzina_sila, sila1);
      MatVektorOpak(sila1, sila2);
      obj1->PridejSilu(bod1, sila1);
      obj2->PridejSilu(bod1, sila2);
      obj2->OnCollision();
    }
    else
    {
      vektor3D v2 = {0.0, 0.0, 0.0};
      pruzina.PruzinaCyklus(bod1, bod2, obj1->V(), v2);
      obj1->PridejSilu(bod1, pruzina.pruzina_sila);
    }
  }
}


bool CDvojice::Stejne(CDvojice *dvojice2)
{
  if ((objekt1 == dvojice2->objekt1) && (objekt2 == dvojice2->objekt2)) return true;
  if ((objekt1 == dvojice2->objekt2) && (objekt2 == dvojice2->objekt1)) return true;
  return false;
}

//-===========================================-
//                 CPOLICKO
//-===========================================-

// destruktor
CPolicko::~CPolicko()
{
  seznam.clear();
}


//-===========================================-
//                 CMRIZKA
//-===========================================-

// nacteni objektu a jejich ulozeni do mrizky
void CMrizka::NactiObjekty()
{
  // AUTA
  for (int i = 0; i < seznam_objektu->PocetObjektu(); i++)
    Vloz(seznam_objektu->prostredek[i], true);

  // Stromy
  for (int i = 0; i < 255*255; i++)
    if (krajina->stromy->strom[i] != NULL)
      Vloz(krajina->stromy->strom[i], false);

  // chodec
  if (seznam_objektu->pesky)
    Vloz(seznam_objektu->chodec, true);
}

// vlozeni objektu na spravnou pozici v mrizce
void CMrizka::Vloz(CObal *obj, bool fyz_obj)
{

  pozice3D pozice;
  obj->DejPoziciObalu(pozice);
  double polomer = obj->DejPolomer();

  // jeden roh
  int x1 = krajina->GetIndex(pozice[X]-polomer) / 10;
  int y1 = krajina->GetIndex(pozice[Y]-polomer) / 10;

  // druhy roh
  int x2 = krajina->GetIndex(pozice[X]+polomer) / 10;
  int y2 = krajina->GetIndex(pozice[Y]+polomer) / 10;


  // vlozeni objektu do vsech policek na kterych se nachazi
  int i = 0;
  for (int y = y1; y <= y2; y++)
    for (int x = x1; x <= x2; x++)
    {
      i = xy_to_i(x, y, 255);
      if (mrizka[i] == NULL)
        mrizka[i] = new CPolicko();
      CPolozkaPolicka polozka = {obj, fyz_obj};
      mrizka[i]->seznam.push_back(polozka);
    }
}

// naplneni seznamu dvojic
void CMrizka::NaplnSeznam()
{
  int i = 0;
  CObal *obj1;
  CObal *obj2;
  int pevny_obj = 0;
  for (int i = 0; i < 255*255; i++)
  {
    if (mrizka[i] == NULL) continue;
    int pocet = mrizka[i]->seznam.size();
    if (pocet > 1)
    {
      // projde vsechny kombinace objektu v mrizce
      for (int n1 = 0; n1 < pocet - 1; n1++)
        for (int n2 = n1+1; n2 < pocet; n2++)
        {
          CDvojice dvojice;
          obj1 = mrizka[i]->seznam[n1].objekt;
          obj2 = mrizka[i]->seznam[n2].objekt;
          if ((!mrizka[i]->seznam[n1].fyz_obj) && (!mrizka[i]->seznam[n2].fyz_obj)) continue;
          pevny_obj = 0;
          if (!mrizka[i]->seznam[n1].fyz_obj) pevny_obj = 1;
          if (!mrizka[i]->seznam[n2].fyz_obj) pevny_obj = 2;
          switch (pevny_obj)
          {
            case 0:
              dvojice.Nastav(obj1, obj2, true);
            break;
            case 1:
              dvojice.Nastav(obj2, obj1, false);
            break;
            case 2:
              dvojice.Nastav(obj1, obj2, false);
            break;
          } // switch
          VlozDoSeznamu(dvojice);
        } // for (vsechny kombinace)
    } // if (vic nez 1 objekt)
    delete mrizka[i];
    mrizka[i] = NULL;
  } // for (vsechny policka)
}


void CMrizka::VlozDoSeznamu(CDvojice &dvojice)
{
  // test jestli uz tam jednou neni
  vector <CDvojice>::iterator it;
  CDvojice temp;
  for (it=seznam.begin(); it != seznam.end(); it++)
  {
    temp.Nastav(*it);
    if (dvojice.Stejne(&temp)) return;
  }
  // vlozeni
  seznam.push_back(dvojice);
}


// kolize
void CMrizka::Kolize()
{
  NactiObjekty(); // vyplni mrizku objekty
  NaplnSeznam();  // naplni seznam dvojic objekty v mrizce, kterou pak smaze

  // projde seznam dvojic a testuje kolize, pote seznam smaze
  vector <CDvojice>::iterator it;
  for (it=seznam.begin(); it != seznam.end(); it++)
    it->Kolize();
  seznam.clear();
}

// konstruktor
CMrizka::CMrizka(teren *t, objekty *o)
{
  krajina = t;
  seznam_objektu = o;
}
