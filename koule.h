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



#ifndef _KOULE_
#define _KOULE_
#include "matika.h"




//! Trida obalove koule
/*!
  Sdruzuje vsechny potrebne informace o jedne obalove kouli a implementuje metody pro praci s nimi
*/
class CKoule
{
  //! unikatni identifikator koule
  /*! Kazda koule v hierarchii ma unikatni identifikator. Nutne kvuli nacteni dat ze souboru. */
  int ID;
  //! pozice koule v souradnicich objektu
  /*! (vektor od stredu objektu) */
  vektor3D pozice_obj;
  //! pozice koule v svetovych souradnicich
  /*! (absolutni souradnice v 3D prostoru) */
  pozice3D pozice;
  //! polomer koule
  /*! polomer obalove koule */
  double polomer;
  //! barva koule
  /*! Definuje jakou barvou se vykresli obrys koule.
      Nastaveni se provadi pri detekci kolizi.
  */
  barva3 barva;
  //! ukazatel na vedlejsi kouli
  /*! Ukazuje na dalsi kouli, ktera ma stejneho rodice jako aktualni
      koule ve stejne hloubce a se stejnym rodicem tvori spojovy seznam.
  */
  CKoule *dalsi;
  //! ukazatel na potomky
  /*! ukazuje na prvniho potomka ve spojovem seznamu potomku
  */
  CKoule *obsah;

 public:
  //! vykresleni koule
  /*!
      pouzije opengl na vykresleni obalove koule
    \param listy urcuje jestli se maji vykreslit pouze koule bez dalsich potomku (listy)
  */
  void Kresli(bool listy) const;
  //! Urci jestli koule obsahuje zadany bod
  /*!
      \param bod bod, ktery se testuje (ve svetovych souradnicich)
      \return true pokud je bod uvnitr koule
  */
  bool JeTam(pozice3D bod);
  //! Detekce kolize mezi dvemi koulemi
  /*!
    Prusecik se vypocte jako pozice v polovine usecky mezi stredy kouli.
    Krome detekce kolizi nastavi take barvu obema koulim.
      \param koule2 ukazatel na kouli, se kterou se testuje kolize
      \param bod1 sem se ulozi souradnice bodu pruniku na prvni kruznici
      \param bod2 sem se ulozi souradnice bodu pruniku na druhe kruznici
      \param list sem se ulozi jestli doslo ke kolizi mezi dvemi listy hierarchie
      \return true pokud doslo ke kolizi
  */
  bool Kolize(CKoule *koule2, pozice3D bod1, pozice3D bod2, bool &list);
  //! Nastavi kouli cervenou barvu
  /*!
     Nastavi barvu na cervenou. Tato metoda se vola, pokud doslo ke kolizi s jinou kouli.
     Barva se resetuje pri zavolani funkce NastavPozici();
  */
  void NastavBarvu();
  //! Zkopiruje pozici
  /*!
      \param pozice sem zkopiruje svoji pozici
  */
  void DejPozici(pozice3D pozice);
  //! Vrati polomer
  /*!
      \return vraci svuj polomer
  */
  double DejPolomer();
  //! Vrati ukazatel na vedlejsi kouli
  /*!
      \return ukazatel na vedlejsi kouli
  */
  CKoule *DejDalsi();
  //! Vrati ukazatel na prvniho potomka
  /*!
      \return ukazatel na potomka
  */
  CKoule *DejObsah();
  //! Vrati identifikator
  /*!
      \return vrati svoje ID
  */
  int DejID();

  //! Urceni nove pozice koule
  /*!
    Vypocita pozici koule po zmene orientace objektu ktery obaluje.
    Take nastavi barvu koule na bilou.
      \param R orientacni matice objektu
      \param pozice_objektu pozice objektu ve svetovych souradnicich
  */
  void NastavPozici(matice R, pozice3D pozice_objektu);
  //! Nastaveni vedlejsiho prvku
  /*!
    zmeni ukazatel na dalsi kouli
      \param jina_dalsi timto ukazatelem se nahradi parametr dalsi
  */
  void NastavDalsi(CKoule *jina_dalsi);
  //! Pridani potomka
  /*!
      \param nova_koule ukazatel na kouli, ktera se ma pridat mezi potomky
  */
  void PridejObsah(CKoule *nova_koule);
  //! Konstruktor
  /*!
    Nastavi pocatecni hodnoty.
      \param ID identifikator koule
      \param pozice_obj relativni pozice koule vzhledem ke stredu objektu ktery obaluje.
      \param polomer polomer koule
  */
  CKoule(int ID, vektor3D pozice_obj, double polomer);
};










#endif


