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
#include "teren.h"
#include "soucastky.h"
#include <iostream>
using namespace std;

const bool BBOX = false;  // jestli se zobrazi bounding box kolem objektu


const double TLUMENI_NARAZU =300.0;
const double SILA_NARAZU = 100000.0;
const double LIN_TLUMENI = 0.1;
const double ROT_TLUMENI = 0.1;

//------------------------------------------------------------------------------
//                       ***   TLUMIC   ***
//------------------------------------------------------------------------------

// KONSTRUKTOR

tlumic::tlumic(double _prumer_kola, double _delka_tlumice, double _tvrdost, double _tlumeni, double _hmotnost_kola, teren *ter, bool _pohon_kola, bool _zataceni, double *pacejkaf, double *pacejkas)
:cpruzina(_tvrdost, _tlumeni, _delka_tlumice){
  rotace = 0.0;
  exponencialni = true;
  w = 0.0;
  otacky = 0.0;
  ZATACENI = _zataceni;
  POHON_KOLA = _pohon_kola;
  PRUMER_KOLA = _prumer_kola;
  HMOTNOST_KOLA = _hmotnost_kola;
  M_NA_MINUS_PRVNI = 1.0 / HMOTNOST_KOLA;
  natoceni_kola = 0.0;
  t = ter;
  lin_x = - _delka_tlumice;
  init_ok = false;
  lin_v = 0.0;
  let = true;
  MatMaticeBaze(R);
  MatVektorSmaz(vk3D);
  MatVektorSmaz(vt3D);
  MatVektorSmaz(sila_up);
  MatVektorSmaz(sila_down);
  lin_sila = 0.0;
  rot_sila = 0.0;
  SETRVACNOST = HMOTNOST_KOLA * powf(PRUMER_KOLA, 2);
  I_NA_MINUS_PRVNI = 1.0 / SETRVACNOST;
  for (int i = 0; i < 20; i++){
    PACEJKAF[i] = pacejkaf[i];
    PACEJKAS[i] = pacejkas[i];
  }
}

// SROVNEJ
void tlumic::Srovnej(){
  vektor3D smer_ke_kolu = {0.0,0.0,lin_x};
  MatMaticeXVektor(R, smer_ke_kolu, pozice_kola);
  MatPozicePlusVektor(pozice_tlumice, pozice_kola, pozice_kola);
  vektor3D smer = {0.0,0.0, -PRUMER_KOLA};
  MatMaticeXVektor(R, smer, kontakt);
  MatPozicePlusVektor(pozice_kola, kontakt, kontakt);
}

// KOLIZE
void tlumic::Kolize(vektor3D rychlost_kola){
  pozice3D prusecik;
  pozice3D normal;
  let = true;
/*
  if (t->Prusecik_n(kontakt, prusecik, normal)){
    cpruzina *pruzina = new cpruzina(SILA_NARAZU, TLUMENI_NARAZU, 0.0);
    pruzina->PruzinaCyklus(kontakt, prusecik, rychlost_kola, vk3D);
    vektor3D sila;
    vektor3D sila_na_objekt;
    MatMaticeXVektor(Rtr, pruzina->pruzina_sila, sila);
    lin_sila += sila[Z];
    sila[Z] = 0.0;
    sila[X] = 0.0;
    MatMaticeXVektor(R, sila, sila_na_objekt);
    MatVektorPridej(sila_na_objekt, sila_down);
    delete pruzina;
    let = false;
  }
 */
}

// MATICE
void tlumic::Matice(double uhel){
  if (ZATACENI){
    matice m;
    matice tempm;
    MatMaticeBaze(m);
    MatMaticeRotaceZ(m, uhel);
    MatMaticeXMatice(R, m, tempm);
    MatMaticeKopni(tempm, R);
  }
  MatMaticeKopni(R, Rtr);
  MatMaticeTransponuj(Rtr);
}

// RYCHLOST
void tlumic::Rychlost(vektor3D rychlost_pozice_kola){

  MatVektorKopni(rychlost_pozice_kola, vk3D);
  vektor3D smer = {0.0, 0.0, lin_v};
  vektor3D smer2;
  MatMaticeXVektor(R, smer, smer2);
  MatVektorPridej(smer2, vk3D);
}

// TLUMENI
void tlumic::Tlumeni(){
  vektor3D dolu = {-1.0,0.0,0.0};
  vektor3D doluTr;
  MatMaticeXVektor(R, dolu, doluTr);

  lin_sila += - lin_v * LIN_TLUMENI;
  rot_sila += - w * ROT_TLUMENI;
  lin_sila += 1.0 * GRAVITACE * HMOTNOST_KOLA * doluTr[Z];


/*
  if (lin_x > 0.0) {
    lin_x = -0.2;
    lin_sila = 0.0;
  }
*/
}


// CYKLUS
void tlumic::RK0(double _zatoceni, bool _brzda){
  brzda = _brzda;
  zatoceni = _zatoceni;
  lin_v0 = lin_v;
  lin_x0 = lin_x;
  if (brzda) w = 0.0;
  w0 = w;
  rotace0 = rotace;
}

void tlumic::RK1(matice _R, vektor3D _pozice_tlumice, vektor3D rychlost_pozice_kola, vektor3D rychlost_pozice_tlumice, double rot_sila0){
  MatPoziceKopni(_pozice_tlumice, pozice_tlumice);
  MatVektorKopni(rychlost_pozice_tlumice, vt3D);
  MatMaticeKopni(_R, R);
  Matice(zatoceni);
  Srovnej();
  Rychlost(rychlost_pozice_kola);

  Sily(rychlost_pozice_kola);
  rot_sila += rot_sila0;
  Tlumeni();

  lin_a1 = lin_sila * M_NA_MINUS_PRVNI * KROK2;
  lin_v1 = lin_v;
  lin_x += KROK2 * lin_v1;
  lin_v += lin_a1;

  dw1 = rot_sila * I_NA_MINUS_PRVNI * KROK2;
  w1 = w;
  if (brzda) w1 = 0.0;
  rotace += (KROK2 * w1)*PRUMER_KOLA;
  w += dw1;

  rot_sila = 0.0;
  lin_sila = 0.0;

}

void tlumic::RK2(matice _R, vektor3D _pozice_tlumice, vektor3D rychlost_pozice_kola, vektor3D rychlost_pozice_tlumice, double rot_sila0){
  MatPoziceKopni(_pozice_tlumice, pozice_tlumice);
  MatVektorKopni(rychlost_pozice_tlumice, vt3D);
  MatMaticeKopni(_R, R);
  Matice(zatoceni);
  Srovnej();
  Rychlost(rychlost_pozice_kola);

  Sily(rychlost_pozice_kola);
  rot_sila += rot_sila0;
  Tlumeni();

  lin_a2 = lin_sila * M_NA_MINUS_PRVNI * KROK2;
  lin_v2 = lin_v;
  lin_x = lin_x0 + KROK2 * lin_v2;
  lin_v = lin_v1 + lin_a2;

  dw2 = rot_sila * I_NA_MINUS_PRVNI * KROK2;
  if (brzda) w2 = 0.0;
  w2 = w;
  rotace = rotace0 + KROK2 * w2 * PRUMER_KOLA;
  w = w1 + dw2;

  rot_sila = 0.0;
  lin_sila = 0.0;
}

void tlumic::RK3(matice _R, vektor3D _pozice_tlumice, vektor3D rychlost_pozice_kola, vektor3D rychlost_pozice_tlumice, double rot_sila0){
  MatPoziceKopni(_pozice_tlumice, pozice_tlumice);
  MatVektorKopni(rychlost_pozice_tlumice, vt3D);
  MatMaticeKopni(_R, R);
  Matice(zatoceni);
  Srovnej();
  Rychlost(rychlost_pozice_kola);

  Sily(rychlost_pozice_kola);
  rot_sila += rot_sila0;
  Tlumeni();


  lin_a3 = lin_sila * M_NA_MINUS_PRVNI * KROK;
  lin_v3 = lin_v;
  lin_x = lin_x0 + KROK * lin_v3;
  lin_v = lin_v1 + lin_a3;

  dw3 = rot_sila * I_NA_MINUS_PRVNI * KROK;
  w3 = w;
  if (brzda) w3 = 0.0;
  rotace = rotace0 + KROK * w3 * PRUMER_KOLA;
  w = w1 + dw3;

  rot_sila = 0.0;
  lin_sila = 0.0;
}

void tlumic::RKFINAL(matice _R, vektor3D _pozice_tlumice, vektor3D rychlost_pozice_kola, vektor3D rychlost_pozice_tlumice, double rot_sila0){
  MatPoziceKopni(_pozice_tlumice, pozice_tlumice);
  MatVektorKopni(rychlost_pozice_tlumice, vt3D);
  MatMaticeKopni(_R, R);
  Matice(zatoceni);
  Srovnej();
  Rychlost(rychlost_pozice_kola);

  Sily(rychlost_pozice_kola);
  rot_sila += rot_sila0;
  Tlumeni();

  lin_x = lin_x0 + (lin_v + lin_v1 + 2.0* (lin_v2 + lin_v3)) * KROK6;
  lin_v = lin_v1 + (lin_a1 + 2.0 * lin_a2 + lin_a3 + lin_sila * M_NA_MINUS_PRVNI * KROK2) / 3.0;

  rotace = rotace0 + (w + w1 + 2.0* (w2 + w3)) * KROK6  * PRUMER_KOLA;
  w = w1 + (dw1 + 2.0 * dw2 + dw3 + rot_sila * I_NA_MINUS_PRVNI * KROK2) / 3.0;
  if (brzda) w = 0.0;

  while (rotace > 2.0 * PI) rotace -= 2.0 * PI;
  while (rotace < 0.0) rotace += 2.0 * PI;

  rot_sila = 0.0;
  lin_sila = 0.0;
}



// SILY
void tlumic::Sily(vektor3D rychlost_kola){

  MatVektorSmaz(sila_up);
  MatVektorSmaz(sila_down);

  Kolize(rychlost_kola);

  // pokud se vektor sily od pruziny vynasobi transponovanou matici rotace, mel by ukazovat primo nahoru (dolu)
  PruzinaCyklus(pozice_tlumice, pozice_kola, vt3D, vk3D);
  vektor3D temp;
  MatMaticeXVektor(Rtr, pruzina_sila, temp);
  lin_sila -= temp[Z];

  // sila na auto
  MatVektorKopni(pruzina_sila, sila_up);

  // sila od kol
  smyk = false;

  if (let) return;
  SilaPneu();
}



void tlumic::SilaPneu(){
  vektor3D skutecny_pohyb_so;   // kam se kolo skutecne pohybuje (souradnice objektu)
  vektor3D spravny_pohyb_so = {1.0, 0.0, 0.0};    // kam by se kolo melo pohybovat vzhledem ke svemu natoceni
  vektor3D sila_zataceni_so;    // kam pusobi sila zataceni (souradnice objektu)
  vektor3D sila_zataceni_ss;    // kam pusobi sila zataceni (svetove souradnice)
  bool road;
  double POVRCH =  0.5 * MatVektorVelikost(pruzina_sila) * t->Povrch(kontakt, road);


  double rych_x;      // rychlost kola na ose X
  double rych_y;      // rychlost kola na ose Y
  vektor3D rych;      // rychlost kola
  vektor3D sila;      // vysledna sila v SO


  MatMaticeXVektor(Rtr, vk3D, rych);
  rych_x = rych[X];
  rych_y = rych[Y];
  MatVektorSmaz(sila);


  // urceni sily pusobici do strany vlivem natoceni kola mimo smer jizdy
/*
  MatMaticeXVektor(Rtr, vk3D, skutecny_pohyb_so);
  MatVektorSmaz(sila_zataceni_so);
  double silaS = skutecny_pohyb_so[Y] * 1.0;
  sila_zataceni_so[Y] = - MatPoleInterpoluj( PACEJKAS, 20, silaS);
  if (fabs(skutecny_pohyb_so[Y]*2.0) > 10.0) smyk = true;
  MatMaticeXVektor(R, sila_zataceni_so, sila_zataceni_ss);
  MatVektorXSkalar(sila_zataceni_ss, POVRCH*5.0);
  MatVektorPridej(sila_zataceni_ss, sila_down);
*/

  vektor3D skutecny_pohyb_soj;
  MatMaticeXVektor(Rtr, vk3D, skutecny_pohyb_so);
  MatVektorKopni(skutecny_pohyb_so, skutecny_pohyb_soj);
  double zn = znamenko(skutecny_pohyb_soj[Y]);
  skutecny_pohyb_soj[Z] = 0.0;
  MatVektorJednot(skutecny_pohyb_soj);
  MatVektorSmaz(sila_zataceni_so);
  vektor3D dopredu = {1.0, 0.0, 0.0};
  double uhel = MatRadianStupen(MatVektorUhel(skutecny_pohyb_soj, dopredu));
  sila[Y] = MatPoleInterpoluj( PACEJKAS, 20, uhel * 0.3);
  sila[Y] *= - zn * POVRCH *3.0;
  double sila_max = fabs(0.5 * POVRCH * MatVektorVelikost(vk3D));
  //double sila_max = fabs(POVRCH * skutecny_pohyb_so[Y]);
  if (sila[Y] > sila_max) sila[Y] = sila_max;
  if (sila[Y] < -sila_max) sila[Y] = -sila_max;
//  MatVektorXSkalar(sila_zataceni_so, sila[Y]);
//  MatMaticeXVektor(R, sila_zataceni_so, sila_zataceni_ss);
  //MatVektorPridej(sila_zataceni_ss, sila_down);
  //cout << sila << endl;


  // rotacni sila od povrchu
  double rychlost_kola = w * PRUMER_KOLA;
  double rozdil = rych_x - rychlost_kola;
  sila[X] -= POVRCH * 1.5 * MatPoleInterpoluj(PACEJKAF, 20, rozdil * 0.3);
  rot_sila += -PRUMER_KOLA * sila[X];

/*
  // lin sila od povrchu
  vektor3D smer_lin_sily_SO = {-sila[X], 0.0, 0.0};
  vektor3D smer_lin_sily_SS;
  MatMaticeXVektor(R, smer_lin_sily_SO, smer_lin_sily_SS);
  MatVektorPridej(smer_lin_sily_SS, sila_down);
*/

  // transformace
  MatMaticeXVektor(R, sila, sila_down);


  // smyky - kombinace sil do stran a do predu nesmi prekrocit urcitou hranici
  double velikost = MatVektorVelikost(sila_down);
  MatVektorJednot(sila_down);
  if (velikost > POVRCH) {
    velikost = POVRCH;
    smyk = true;
  }
  MatVektorXSkalar(sila_down, velikost);

}





// KRESLI
void tlumic::Kresli(){
  if (!BBOX) return;

  glDisable(GL_TEXTURE_2D);
  glEnable(GL_COLOR_MATERIAL);

  // tlumic
  glBegin(GL_LINES);
    glVertex3dv(pozice_tlumice);
    glVertex3dv(pozice_kola);
  glEnd();


  // kolo
  vektor3D vektor_kola[100];
  pozice3D body_kola[100];
  double uhel = 0.0;
  for (int n = 0; n < 100; n++){
    vektor_kola[n][X] = PRUMER_KOLA * cos(uhel);
    vektor_kola[n][Y] = 0.0;
    vektor_kola[n][Z] = PRUMER_KOLA * sin(uhel);
    uhel += 2.0 * PI / 100.0;
  }

  for (int n = 0; n < 100; n++){
    MatMaticeXVektor(R, vektor_kola[n], body_kola[n]);
    for (int i = 0; i < 3; i++)
      body_kola[n][i] += pozice_kola[i];
  }
  glBegin(GL_LINE_LOOP);
  for (int n = 0; n < 100; n++)
    glVertex3dv(body_kola[n]);
  glEnd();

  pozice3D p2;

  for (int i = 0; i < 3; i++)
    p2[i] = pozice_kola[i] + sila_down[i] * 0.0001;
  glBegin(GL_LINES);
    glVertex3dv(pozice_kola);
    glVertex3dv(p2);
  glEnd();

/*
  pozice3D b1;
  pozice3D b2;

  MatPoziceKopni(pozice_kola, b1);
  MatPoziceKopni(pozice_kola, b2);
  for(int i = 0; i < 3; i++)
    b2[i] += vk3D[i] * 10.0;
  glBegin(GL_LINES);
    glVertex3fv(b1);
    glVertex3fv(b2);
  glEnd();
*/
  glEnable(GL_TEXTURE_2D);
  glDisable(GL_COLOR_MATERIAL);

}












//------------------------------------------------------------------------------
//                       ***   VOLANT   ***
//------------------------------------------------------------------------------

tvolant::tvolant(double _max_zataceni, double _rychlost_zataceni){
  MAX_ZATOCENI = _max_zataceni;
  RYCHLOST_ZATACENI = _rychlost_zataceni;
  zatoceni = 0.0;
}


void tvolant::Cyklus(bool left, bool right){
//  <-----o----->
//  +           -

  // osetreni klaves
  bool l = false;
  bool r = false;
  if ((left) && (!right)) l = true;
  if ((!left) && (right)) r = true;

  // reakce na klavesy
  if (l) zatoceni += RYCHLOST_ZATACENI;
  if (r) zatoceni -= RYCHLOST_ZATACENI;

  // meze
  if (zatoceni >= MAX_ZATOCENI) zatoceni = MAX_ZATOCENI;
  if (zatoceni <= - MAX_ZATOCENI) zatoceni = - MAX_ZATOCENI;

  // vraceni volantu na stred
  if ((!l) && (!r)) {
    if (zatoceni >= 0.0 + RYCHLOST_ZATACENI) zatoceni -= RYCHLOST_ZATACENI;
    if (zatoceni <= 0.0 - RYCHLOST_ZATACENI) zatoceni += RYCHLOST_ZATACENI;
    if (fabs(zatoceni) < RYCHLOST_ZATACENI) zatoceni = 0.0;
  }
}


//-------------------------------------------------------------------------------------------
//                                        SVETLO
//-------------------------------------------------------------------------------------------

tsvetlo::tsvetlo(int interval, barva3 bv, barva3 bz){
  MatPoziceKopni(bv, barva_vyp);
  MatPoziceKopni(bz, barva_zap);
  interval_blikani = interval;
  if (interval != 0) blik = true;
  interval_blikani = interval;
  interval_stav = 0;
  DL = 0;
  sviti = false;
}


void tsvetlo::Cyklus(bool zap, const Mat3D& _matice_rotace, const Vec3D& pos){
  sviti = false;
  if (zap){
    if (blik){
      interval_stav += 1;
      if (interval_stav <= interval_blikani/2) sviti = true;
        else sviti = false;
      if (interval_stav >= interval_blikani) interval_stav = 0;
    }
    else
      sviti = zap;
  }

  matice_rotace = _matice_rotace;
  pozice = pos;
}


void tsvetlo::Kresli(){
  if (DL == 0) return;

  glDisable(GL_TEXTURE_2D);
  glDisable(GL_COLOR_MATERIAL);

  float barva[4] = {0.0,0.0,0.0, 1.0};
  MatPoleKopnidf(barva_vyp, barva, 3);
  float nic[4] = {0.0, 0.0, 0.0, 1.0};


  if (sviti) MatPoleKopnidf(barva_zap, barva, 3);


  if (sviti) glMaterialfv (GL_FRONT_AND_BACK, GL_EMISSION, barva);

  glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT, barva);
  glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, barva);

  //glColor3fv(barva);

  glPushMatrix();
  glTranslatef(pozice.getX(), pozice.getY(), pozice.getZ());

  double mat[16];
  matice_rotace.convertToOpenGL(mat);

  glMultMatrixd(mat);

  glCallList(DL);

  glMaterialfv (GL_FRONT_AND_BACK, GL_EMISSION, nic);
  glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT, nic);
  glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, nic);

  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_TEXTURE_2D);

  glPopMatrix();
}



//-------------------------------------------------------------------------------------------
//                                        MOTOR
//-------------------------------------------------------------------------------------------

const int DOBA_STARTOVANI = 350;
const int DOBA_STARTOVANI_PROTACENI = 300;

const double TLUMENI = 0.001;

cmotor::cmotor(double _max_otacky, double _vykon){
  MAX_W = 2.0 * M_PI * _max_otacky / 60.0; // prevede otacky za minutu na uhlovou rychlost
  CHCIP_W = 2.0 * M_PI * 500.0 / 60.0;
  NEUTRAL_W = 2.0 * M_PI * 500.0 / 60.0;
  NORMAL_W = 2.0 * M_PI * 1000.0 / 60.0;
  VYKON = _vykon * 1.0;
  w = 0.0;
  dw = 0.0;
  sila = 0.0;
  I_NA_MINUS_PRVNI = 1.0 / 0.13;
  stav = MOTOR_VYP;
  stav_startovani = 0;
  starter_sound = false;
}

void cmotor::Sila()
{
    sila = 0.0;
  switch (stav){
		case MOTOR_VYP:
		{
			if ((starter) && (stav_startovani == 0))
			{
				stav = MOTOR_STARTUJE;
				stav_startovani = 0;
                                starter_sound = true;
			}
			sila = 0.0;
 			if (stav_startovani > 0) stav_startovani--;
		}
		break;
		case MOTOR_BEZI:
		{
			if (starter)
			{
				stav = MOTOR_VYP;
				stav_startovani = 100;
				sila = 0.0;
				return;
			}
      double x = w / MAX_W;
      if ((x > 0.0) && (x < 1.0) && (plyn)){
        sila = VYKON;
      }
    }
		break;
		case MOTOR_STARTUJE:
		{
                    if (stav_startovani < DOBA_STARTOVANI_PROTACENI){
                        sila =  VYKON * 0.1;
                    } else sila = VYKON;


			if (stav_startovani >= DOBA_STARTOVANI)
			{
				stav = MOTOR_BEZI;
				sila = 0.0;
			}
			stav_startovani++;
		}
	}
  Tlumeni();
}

void cmotor::RK0(bool plyn, bool starter)
{
  this->plyn = plyn;
  this->starter = starter;
  w0 = w;
}

void cmotor::RK1(double spojka)
{
  Sila();
  dw1 = (sila - spojka) * I_NA_MINUS_PRVNI * KROK2;
  w1 = w;
  w += dw1;
}


void cmotor::RK2(double spojka)
{
  Sila();
  dw2 = (sila - spojka) * I_NA_MINUS_PRVNI * KROK2;
  w2 = w;
  w = w1 + dw2;
}

void cmotor::RK3(double spojka)
{
  Sila();
  dw3 = (sila - spojka) * I_NA_MINUS_PRVNI * KROK;
  w3 = w;
  w = w1 + dw3;
}

void cmotor::RKFINAL(double spojka)
{

  Sila();
  w = w1 + (dw1 + 2.0 * dw2 + dw3 + (sila - spojka) * I_NA_MINUS_PRVNI * KROK2) / 3.0;
}


void cmotor::Tlumeni()
{
    double x = w / MAX_W;
    double sila_tlumeni = - w * w * TLUMENI;
    if (x < 0.0) sila_tlumeni = - sila_tlumeni;
    sila += sila_tlumeni;

    if ((w < CHCIP_W) && (stav == MOTOR_BEZI)) {
        stav = MOTOR_VYP;
    }

    if ((stav == MOTOR_BEZI) && (w < NORMAL_W)) sila += VYKON * 0.1;   // udrzovaci sila (drzi minimalni otacky)

}


//-------------------------------------------------------------------------------------------
//                                     PREVODOVKA
//-------------------------------------------------------------------------------------------


cprevodovka::cprevodovka(int _pocet, double *_pomery){
  POCET_STUPNU = _pocet;
  for (int i = 0; i < _pocet; i++)
    POMERY[i] = _pomery[i];
  stupen = 1;
  radim = false;
  stav_razeni = 0;
  stupen_cil = 1;
	spojka = false;
}

void cprevodovka::Cyklus(bool up, bool down){
  if (radim){
    stav_razeni += 1;
    stupen = 1;
    if (stav_razeni >= 30){
      stupen = stupen_cil;
      stav_razeni = 0;
      radim = false;
    }
  }
	if (spojka) stupen = 1;

  if ((up) && (!down) && (!radim)){
    radim = true;
    stupen_cil = stupen + 1;
  }
  if ((!up) && (down) && (!radim)){
    radim = true;
    stupen_cil = stupen - 1;
  }
  if (stupen < 0) stupen = 0;
  if (stupen >= POCET_STUPNU) stupen = POCET_STUPNU - 1;
}

