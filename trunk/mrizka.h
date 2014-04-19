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



#ifndef _MRIZKA_
#define _MRIZKA_


#include "obal.h"
#include "teren.h"
#include "objekty.h"
#include <vector>


//! Dvojice objektu
/*!
  Trida obsahuje ukazatele na dve dvojice obalu, mezi kterymi se ma provest detekce kolizi.
  Reakce na kolizi zavisi na tom, jestli je druhy objekt strom nebo auto.
*/
class CDvojice
{
  //! ukazatel na prvni objekt
  /*!
    Musi byt vzdy fyzikalni objekt. (zatim implementovano jen auto)
  */
  CObal *objekt1;
  //! ukazatel na druhy objekt
  /*!
    Bud fyzikalni nebo pevny objekt (zatim jen strom)
  */
  CObal *objekt2;
  //! Informace o druhem objektu
  /*!
    Udava jestli je druhy objekt fyzikalni nebo pevny
  */
  bool fyz_obj;

 public:
  //! Test kolize
  /*!
    Testuje kolizi mezi objekt1 a objekt2 (a pocita reakci podle toho, jestli je druhy pevny nebo fyzikalni).
  */
  void Kolize();
  //! Nastaveni parametru
  /*!
    \param obj1 ukazatel na prvni objekt
    \param obj2 ukazatel na druhy objekt
    \param fobj Udava jestli je druhy objekt fyzikalni nebo pevny
  */
  void Nastav(CObal *obj1, CObal *obj2, bool fobj);

  //! Nastaveni parametru z jineho objektu
  /*!
    \param dvojice objekt ze ktereho se zkopiruji parametry
  */
  void Nastav(CDvojice &dvojice);

  //! Porovnani
  /*!
    Porovna tuto dvojici s jinou a vrati true pokud ukazuji na stejne objekty.
  */
  bool Stejne(CDvojice *dvojice2);
};

//! Struktura polozky policka
/*!
  Informace o jedne polozce.
*/
struct CPolozkaPolicka
{
  //! Ukazatel na objekt.
  CObal *objekt;
  //! Udava jestli je druhy objekt fyzikalni nebo pevny.
  bool fyz_obj;
};

//! Struktura policka
/*!
  Informace o jednom policku mrizky.
*/
struct CPolicko
{
  //! Seznam dvojic objektu.
  /*!
    Seznam vsech objektu na danem policku.
  */
  vector<CPolozkaPolicka>seznam;
  //! Destruktor
  /*!
    Smaze seznam polozek.
  */
  ~CPolicko();
};



//! Trida mrizky
/*!
  Informace o mrizce.
*/
class CMrizka
{
  //! Ukazatele na vsechny policka mrizky.
  /*!
    Pole ukazatelu na kazde policko mrizky.
  */
  CPolicko *mrizka[255*255];

  //! Ukazatel na teren.
  /*!
    Odtud se vycucnou pozice stromu.
  */
  teren *krajina;

  //! Ukazatel na fyzikalni objekty.
  /*!
    Odtud se vycucnou pozice aut.
  */
  objekty *seznam_objektu;

  //! Seznam vsech dvojic objektu.
  /*!
    Seznam dvojic objektu, mezi kterymi se provede detekce kolizi.
    Ulozi se sem jen unikatni dvojice.
  */
  vector<CDvojice>seznam;

  //! Nacteni objektu do mrizky.
  /*!
    Projde vsechny objekty a ulozi jejich ukazatele na spravnou pozici v mrizce.
  */
  void NactiObjekty();

  //! Nacteni seznamu dvojic objektu.
  /*!
    Projde vsechna policka mrizky a naplni seznam dvojic.
    Z mrizky se dvojice smazou.
  */
  void NaplnSeznam();

  //! Vlozeni jedne dvojice do seznamu dvojic
  /*!
    Vlozi dvojici do seznamu dvojic (ale zkontroluje jestli tam uz neni)
    \param dvojice Dvojice ktera se ma ulozit.
  */
  void VlozDoSeznamu(CDvojice &dvojice);


  //! Vlozeni jednoho objektu do mrizky.
  /*!
    Vlozi zadany objekt na spravna policka v mrizce.
    \param obj Ukazatel na objekt, ktery se ma vlozit.
    \param fyz_obj Udava jestli je druhy objekt fyzikalni nebo pevny
  */
  void Vloz(CObal *obj, bool fyz_obj);
 public:
  //! Test kolizi.
  /*!
    Naplni mrizku objekty, pak je "shrabne" do seznamu a nakonec mezi nimi provede kolizi.
  */
  void Kolize();
  //! Konstruktor
  /*!
    \param t ukazatel na teren
    \param o ukazatel na seznam objektu
  */
  CMrizka(teren *t, objekty *o);
};





#endif







