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

/*
 * File:   wheel.h
 * Author: milan
 *
 * Created on 16. červenec 2009, 11:17
 */

#ifndef _WHEEL_H
#define	_WHEEL_H


#include "3Dmath/Vec3D.h"
#include "3Dmath/Mat3D.h"
#include "teren.h"
#include "suspension.h"
#include "Pacejka.h"

class wheel{
    double POLOMER;         // polomer pneumatiky
    double HMOTNOST;
    double INV_HMOTNOST;
    double SETRVACNOST;
    double INV_SETRVACNOST;
    bool POHON_KOLA;        // jestli je kolo napojene na pohon auta
    bool ZATACENI;          // jestli kolo umi zatacet

    Pacejka* pacejka;

    bool let;
    double tlak;        // jak moc je pneumatika tlacena na povrch terenu

    teren* t;
    suspension* tlumic;

    int povrch;
    double slipRatio;
    double slipRatioRoad;

    Mat3D orientace;
    Mat3D inv_orientace;

    Vec3D pozice;
    Vec3D rychlost;

    double x;
    double v;
    double a;
    double w;
    double lin_sila;
    double rot_sila;
    double rotace;      // natoceni kola kolem osy

    Vec3D kontakt;      // misto, kde se pneumatika dotyka povrchu
    Vec3D sila_pneu;    // sila vznikajici trenim pneu o povrch
    Vec3D sila_kola;    // sila vznikajici kolizi kola s terenem (tlumic pohlcuje jen silu ve smeru nahoru)

    double zatoceni_kola;
    bool brzda;

    // RUNGE KUTTA
    double x0,x1,x2,x3;
    double v0,v1,v2,v3;
    double a1,a2,a3;
    double w0, w1, w2, w3;
    double dw0, dw1, dw2, dw3;
    double rotace0, rotace1, rotace2, rotace3;

    bool tlumeni_narazu;

    void zataceni(const Mat3D& orientace_auta, const Vec3D& pozice_tlumice, const Vec3D& rychlost_tlumice, double krok);            // v pripade, ze kolo je zataceci, vypocita novou matici orientace. Pokud neni, jen prevezme matici auta

    void silaKolize();
    void silaPneu();
    void sila();            // vypocet vsech sil na kolo

    void srovnej(const Vec3D& pozice_tlumice);


public:
    wheel(double polomer, double hmotnost, bool pohon, bool zataceni, teren* t, suspension* tlumic, const Vec3D& pozice, Pacejka* pacejka);
    ~wheel();

    void rk0(double zatoceni, bool brzda);
    void rk1(const Vec3D& pozice_tlumice, const Vec3D& rychlost_tlumice, const Mat3D& orientace_auta, double rot_sila_motor);
    void rk2(const Vec3D& pozice_tlumice, const Vec3D& rychlost_tlumice, const Mat3D& orientace_auta, double rot_sila_motor);
    void rk3(const Vec3D& pozice_tlumice, const Vec3D& rychlost_tlumice, const Mat3D& orientace_auta, double rot_sila_motor);
    void rkfinal(const Vec3D& pozice_tlumice, const Vec3D& rychlost_tlumice, const Mat3D& orientace_auta, double rot_sila_motor);

    const Vec3D& dejPozici() const {return pozice;}
    double dejRotaci() const {return rotace;}
    const Mat3D& dejOrientaci() const {return orientace;}
    const Mat3D& dejInvOrientaci() const {return inv_orientace;}
    const Vec3D& dejSiluPneu() const {return sila_pneu;}
    const Vec3D& dejSiluKola() const {return sila_kola;}
    const Vec3D& dejKontaktPneu() const {return kontakt;}
    double dejW() const {return w;}

    bool pohon() const {return POHON_KOLA;}

    void reset();

    double dejVychylku()const{return x;}
    void pridejSilu(double sila){lin_sila += sila;}

    double SlipRatio() const {return slipRatio;}
    double SlipRatioRoad() const {return slipRatioRoad;}

    Pacejka* DejPacejka() {return pacejka;}
};



#endif	/* _WHEEL_H */

