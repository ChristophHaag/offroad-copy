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


#ifndef _OBAL_
#define _OBAL_

#include "koule.h"
#include "loaddata.h"
#include <vector>


//! Trida hierarchie obalovych kouli
/*!
  Obsahuje ukazatel na strom obalovych kouli a deklaruje metody pro manipulaci s nimi
*/
class CObal
{
  //! ukazatel na prvni kouli
  /*! ukazatel na prvni kouli v hierarchii obalovych kouli
  */
  CKoule *top;
  //! seznam vsech kouli
  /*! seznam vsech kouli slouzi k urychleni prochazeni celeho stromu (update pozice, hledani rodice, atd...).
  */
  vector<CKoule*>obsah;

  //! Nalezeni koule podle ID
  /*!
    Vrati ukazatel na kouli s hledanym ID. Pokud neexistuje, vrati null.
    \param ID hledane ID
  */
  CKoule *NajdiKouli(int ID);

 public:
  //! Nastaveni pozic
  /*!
    Vypocita pozici kouli po zmene orientace objektu ktery obaluje.
    Take nastavi barvu koule na bilou.
      \param R orientacni matice objektu
      \param pozice_objektu pozice objektu ve svetovych souradnicich
  */
  void NastavObal(matice R, pozice3D pozice_objektu);

  //! Vykresleni obalovych kouli
  /*!
    Vykresli celou hierarchii obalovych kouli
    \param listy urcuje jestli se maji vykreslit pouze koule bez dalsich potomku (listy)
  */
  void KresliObal(bool listy) const;

  //! Detekce kolize mezi dvema obaly
  /*!
      \param obal2 ukazatel na obal, se kterym se testuje kolize
      \param bod1 sem se ulozi souradnice bodu pruniku na prvni kruznici
      \param bod2 sem se ulozi souradnice bodu pruniku na druhe kruznici
      \return true pokud doslo ke kolizi
  */
  bool KolizeObalu(CObal *obal2, pozice3D bod1, pozice3D bod2);

//!  Rekurzivni detekce kolize
/*!  Pokud ke kolizi doslo, zavola sama sebe na detekci mezi vsemi potomky 2 a kouli 1
    \param koule1 ukazatel na prvni kouli
    \param koule2 ukazatel na druhou kouli
    \param bod1 sem se ulozi souradnice bodu pruniku na prvni kruznici
    \param bod2 sem se ulozi souradnice bodu pruniku na druhe kruznici
    \param pocet sem se ulozi pocet kolidujicich kouli
    \return true pokud doslo ke kolizi
*/
  bool RekurzivniDetekce(CKoule *koule1, CKoule *koule2, pozice3D bod1, pozice3D bod2, int &pocet);
  //! Nacteni struktury obalovych kouli
  /*!
    Nacte data ze struktury obal_data, ktera obsahuje informace o obalu nactene z konfiguracniho souboru.
  */
  void InitObal(obal_data *data);
  //! Konstruktor
  /*!
    Nastavi top na null, koule se pridaji pozdeji
  */
  CObal();

  //! Vrati pozici obalu
  /*!
    Vrati souradnice stredu nejvetsi obalove koule (prvni v hierarchii)
    \param pozice Sem se ulozi vysledna pozice
  */
  void DejPoziciObalu(pozice3D pozice);
  //! Vrati polomer obalu
  /*!
    Vrati polomer nejvetsi obalove koule (prvni v hierarchii)
  */
  double DejPolomer();

  ~CObal();
};

#endif

