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


#include "suspension.h"

#include "wheel.h"

#include <math.h>

#include <iostream>
using namespace std;

const double TVRDOST_TERENU = 200000.0;
const double TLUMENI_NARAZU = 4000.0;



wheel::wheel(double polomer, double hmotnost, bool pohon, bool zataceni, teren* t, suspension* tlumic, const Vec3D& pozice, Pacejka* pacejka){
    POLOMER = polomer;
    HMOTNOST = hmotnost;
    INV_HMOTNOST = 1.0 / HMOTNOST;
    POHON_KOLA = pohon;
    ZATACENI = zataceni;
    SETRVACNOST = HMOTNOST * powf(POLOMER, 2);
    INV_SETRVACNOST = 1.0 / SETRVACNOST;

    this->pacejka = pacejka;

    this->t = t;
    this->tlumic = tlumic;
    this->pozice = pozice;

    zatoceni_kola = 0.0;
    let = true;
    brzda = false;
    inv_orientace = orientace.invert();

    x = 0.0;
    v = 0.0;
    a = 0.0;
    w = 0.0;

    lin_sila = 0.0;
    rot_sila = 0.0;

    rotace = 0.0;

    tlak = 0.0;

    slipRatio = 0.0;
}

wheel::~wheel()
{

}

void wheel::sila(){
    sila_pneu.clear();
    sila_kola.clear();
    silaKolize();
    silaPneu();
    tlumic->vypocetSily(x, v);
    lin_sila += tlumic->dejSilu();


}

void wheel::srovnej(const Vec3D& pozice_tlumice){
    return;
    if (x > 0.1) x = 0.1;
    pozice = pozice_tlumice + orientace * Vec3D(0.0,0.0,x);

}



void wheel::silaPneu(){
    slipRatio = 0.0f;
    slipRatioRoad = 0.0f;

    if (let) {
        return;
    }

    Vec3D smer_pohybu = inv_orientace * rychlost;      // skutecny smer pohybu kola
    smer_pohybu.setZ(0.0);

    double x = smer_pohybu.getX();
    double y = smer_pohybu.getY();

    double uhel = 0.0;
    if (fabs(x) < 0.0000001){
        if (y > 0.0) uhel = M_PI / 2.0;
        else uhel = -M_PI / 2.0;
    } else {
        uhel = atan(y/x);
        if (x < 0.0) uhel = -uhel;
    }

    if (fabs(x) < 0.001){
        slipRatio = w * POLOMER;
    } else slipRatio = ( w * POLOMER - x) / fabs(x);

    bool road;
    Vec3D sila_pacejka = pacejka->sila(- tlumic->dejSilu(), slipRatio, uhel, x, y, t->Povrch((double*)&pozice, road));

    slipRatio = fabs(w * POLOMER - x + y);
    if (fabs(slipRatio < 3.0f))
      slipRatio = 0.0f;

    if (road)
    {
      slipRatioRoad = slipRatio;
      slipRatio = 0.0f;
    }

    sila_pneu += orientace * sila_pacejka;
    if (!brzda) rot_sila -= POLOMER * sila_pacejka.getX();

}


void wheel::rk0(double zatoceni, bool brzda){
    this->zatoceni_kola = zatoceni;
    this->brzda = brzda;

    x0 = x;
    v0 = v;
    w0 = w;
    lin_sila = 0.0;
    rot_sila = 0.0;
    rotace0 = rotace;
    sila_pneu.clear();
    sila_kola.clear();

}

void wheel::rk1(const Vec3D& pozice_tlumice, const Vec3D& rychlost_tlumice, const Mat3D& orientace_auta, double rot_sila_motor){
    zataceni(orientace_auta, pozice_tlumice, rychlost_tlumice, KROK2);
    rot_sila += rot_sila_motor;

    sila();

    a1 = lin_sila * INV_HMOTNOST * KROK2;
    v1 = v;
    x += v1 * KROK2;
    v += a1;

    dw1 = rot_sila * INV_SETRVACNOST * KROK2;
    w1 = w;
    if (brzda) w1 = 0.0;
    w += dw1;

    rotace += (KROK2 * w1) * POLOMER * 2.0;

    lin_sila = 0.0;
    rot_sila = 0.0;
    srovnej(pozice_tlumice);
}


void wheel::rk2(const Vec3D& pozice_tlumice, const Vec3D& rychlost_tlumice, const Mat3D& orientace_auta, double rot_sila_motor){
    zataceni(orientace_auta, pozice_tlumice, rychlost_tlumice, KROK2);
    rot_sila += rot_sila_motor;

    sila();


    a2 = lin_sila * INV_HMOTNOST * KROK2;
    v2 = v;
    x = x0 + v2 * KROK2;
    v = v1 + a2;

    dw2 = rot_sila * INV_SETRVACNOST * KROK2;
    w2 = w;
    if (brzda) w2 = 0.0;
    w = w1 + dw2;
    rotace = rotace0 + KROK2 * w2 * POLOMER * 2.0;

    lin_sila = 0.0;
    rot_sila = 0.0;
    srovnej(pozice_tlumice);

}


void wheel::rk3(const Vec3D& pozice_tlumice, const Vec3D& rychlost_tlumice, const Mat3D& orientace_auta, double rot_sila_motor){
    zataceni(orientace_auta, pozice_tlumice, rychlost_tlumice, KROK);
    rot_sila += rot_sila_motor;

    sila();
    a3 = lin_sila * INV_HMOTNOST * KROK;
    v3 = v;
    x = x0 + v3 * KROK;
    v = v1 + a3;

    dw3 = rot_sila * INV_SETRVACNOST * KROK;
    w3 = w;
    if (brzda) w3 = 0.0;
    w = w1 + dw3;
    rotace = rotace0 + KROK * w3 * POLOMER * 2.0;

    lin_sila = 0.0;
    rot_sila = 0.0;
    srovnej(pozice_tlumice);

}


void wheel::rkfinal(const Vec3D& pozice_tlumice, const Vec3D& rychlost_tlumice, const Mat3D& orientace_auta, double rot_sila_motor){
    zataceni(orientace_auta, pozice_tlumice, rychlost_tlumice, KROK2);
    rot_sila += rot_sila_motor;
    sila();
    x = x0 + (v + v1 + (v2 + v3)* 2.0) * KROK6;
    v = v1 + (a1 + a2 * 2.0 + a3 + lin_sila * INV_HMOTNOST * KROK2) / 3.0;

    rotace = rotace0 + (w + w1 + 2.0* (w2 + w3)) * KROK6  * POLOMER * 2.0;
    w = w1 + (dw1 + 2.0 * dw2 + dw3 + rot_sila * INV_SETRVACNOST * KROK2) / 3.0;
    if (brzda) w = 0.0;


    while (rotace > 2.0 * PI) rotace -= 2.0 * PI;
    while (rotace < 0.0) rotace += 2.0 * PI;

    lin_sila = 0.0;
    rot_sila = 0.0;
    srovnej(pozice_tlumice);


}




//funkce vypocita jestli kolo narazilo do terenu a jakou silu mu to pripadne dodalo (ulozi se do lin_sila)
void wheel::silaKolize(){
    Vec3D prusecik, normal;
    let = true;
    tlak = 0.0;

    bool naraz = t->Prusecik_n(kontakt, &prusecik, &normal);

    if (naraz){
        normal.normalize();
        double vzd = prusecik.distance(kontakt);
        tlak = vzd;
        double vv = (inv_orientace * rychlost).getZ();  // vypocet rychlosti v so (musi se vzit jen Z slozka, jinak bude tlumeni brzdit i pohyb dopredu)
        Vec3D smer = orientace * Vec3D(0.0, 0.0, 1.0);

        Vec3D sila_naraz = normal * vzd * TVRDOST_TERENU - smer * (vv - lin_sila * KROK / HMOTNOST) * TLUMENI_NARAZU;
        Vec3D sila_naraz_so = inv_orientace * sila_naraz;

        lin_sila += sila_naraz_so.getZ();
        sila_naraz_so.setZ(0.0);
        sila_naraz = orientace * sila_naraz_so;

        sila_kola += sila_naraz;

        let = false;
    }

}


void wheel::zataceni(const Mat3D& orientace_auta, const Vec3D& pozice_tlumice, const Vec3D& rychlost_tlumice, double krok){
    if (ZATACENI) {
        Mat3D z;
        z.rotateZ(zatoceni_kola);
        orientace = z * orientace_auta;
    } else orientace = orientace_auta;
    inv_orientace = orientace.invert();

    pozice = pozice_tlumice + orientace_auta * Vec3D(0.0,0.0,x);
    rychlost = rychlost_tlumice + orientace_auta * Vec3D(0.0,0.0,v * krok);
    kontakt = pozice + orientace_auta * Vec3D(0.0, 0.0, - POLOMER);
}



void wheel::reset(){
    x = 0.0;
    v = 0.0;
    a = 0.0;
    w = 0.0;
    lin_sila = 0.0;
    rot_sila = 0.0;
    sila_kola.clear();
    sila_pneu.clear();
}

