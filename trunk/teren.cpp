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



#include "tgaload.h"
#include "glsl.h"
#include "loaddata.h"
#include "matika.h"
#include "teren.h"
#include "kodovani.h"

#include <fstream>
#include <sys/stat.h>
#include <iostream>
#include <stdlib.h>
#include <sys/timeb.h>


using namespace std;

const double velikost = 170.0;

// konstanty pole silnic
const unsigned char SILNICE_SEVER = 0x80;
const unsigned char SILNICE_ZAPAD = 0x40;
const unsigned char SILNICE_JIH = 0x20;
const unsigned char SILNICE_VYCHOD = 0x10;

const unsigned char OBJEKT_STROM = 1;


const bool PLACKA = false;       // pokud je true, pak se nenacte teren ze souboru, ale udela se misto nej placka (pro testovani pneumatik)

//------------------------------------------------------------------------------
//                              POMOCNE FUNKCE
//------------------------------------------------------------------------------


int velikost_souboru(char *jmeno){
  struct stat results;
  stat(jmeno, &results);
  return results.st_size;
}

int max_rozdil(double *cisla, int n){                        // vrati index cisla, ktere je nejvic vzdalene od prumeru vsech cisel
  if (n == 0) return -1;
  double prumer = 0.0;
  for (int i = 0; i < n; i++)
    prumer += cisla[i];
  prumer = prumer / n;
  double max_rozdil = 0.0;
  int max_polozka = 0;
  for (int i = 0; i < n; i++){
    if (fabs(prumer - cisla[i]) >= max_rozdil){
      max_rozdil = fabs(prumer-cisla[i]);
      max_polozka = i;
    }
  }
  return max_polozka;
}



//------------------------------------------------------------------------------
//                                  TEREN
//------------------------------------------------------------------------------


teren::teren(char *cesta){
  // otevre se soubor mapy a obsah se zkopiruje do bufferu
  int delka_bufferu = velikost_souboru(cesta);
  if (delka_bufferu < 255*255){
    cout << "Chybny soubor mapy" << endl;
    cout << "Delka " << delka_bufferu << endl;
    system("pause");
    exit(0);
  }
  cout << "Nacitam mapu    : " << cesta << endl;
  char *buffer = new char[delka_bufferu];

  ifstream soubor(cesta, ios::in | ios::binary);
  soubor.read(buffer, delka_bufferu);
  soubor.close();


  // ted se dekoduje velikost mapy
  unsigned char horni;
  unsigned char dolni;
  horni = buffer[0];
  dolni = buffer[1];
  sirka = dekoduji(horni, dolni);
  horni = buffer[2];
  dolni = buffer[3];

  vyska = dekoduji(horni, dolni);
  if ((vyska != 2550) || (sirka != 2550)){
    cout << "Chybny soubor mapy." << endl;
    cout << "sirka " << sirka << endl;
    cout << "vyska " << vyska << endl;
    system("pause");
    exit(0);
  }
  // tady se nacte vyskova mapa z bufferu do pole "body"
  body = new char[vyska*sirka*2];
  for (int i = 0; i < sirka*vyska*2; i++)
     if (PLACKA) body[i] = 17;
     else body[i] = buffer[i+4];
  cout << "Mapa nactena" << endl;

  // ted zas typ policek do "typ_povrchu"
  int pozice = 0;
  int zacatek = 2*vyska*sirka+4;
  int konec = zacatek + ((255*255)/2);
  int temp[2];
  typ_povrchu = new short[255*255];
  for (int i = zacatek; i < konec; i++){
    dekoduj2i(buffer[i], temp);
    typ_povrchu[pozice] = temp[0];
    typ_povrchu[pozice+1] = temp[1];
    pozice += 2;
  }
  cout << "Povrch nacten" << endl;


  // taky se nactou informace o silnicich do "silnice"
  silnice = new unsigned char[255*255];
  zacatek = konec;
  konec = zacatek + 255*255;
  pozice = 0;
  for (int i = zacatek; i < konec; i++)
    silnice[pozice++] = buffer[i];
  cout << "Mapa silnic nactena" << endl;

  // nacteni objektu
  zacatek = konec;
  konec = zacatek + 255*255;
  pozice = 0;
  bool mapa_stromu[255*255];

  for (int i = zacatek; i < konec; i++){
    mapa_stromu[pozice] = false;
    if (PLACKA) continue;
    switch (buffer[i]){
      case OBJEKT_STROM:
        mapa_stromu[pozice] = true;
      break;
    }
    pozice++;
  }

  stromy = new CStromy(mapa_stromu, this);
  cout << "Mapa objektu nactena" << endl;




  // ted se vytvori prazdne display listy (kazdy dl obsahuje 2*2 policek z tycoona)
  pocet_ctvercu = (sirka/VELIKOST_CTVERCE)*(vyska/VELIKOST_CTVERCE);

  ctverec_q1 = new int [pocet_ctvercu];
  ctverec_q2 = new int [pocet_ctvercu];
  ctverec_dl = new GLuint [pocet_ctvercu];
  for (int i = 0; i < pocet_ctvercu; i++)
    ctverec_dl[i] = 0;
  smaz_pole(ctverec_q1, pocet_ctvercu);
  smaz_pole(ctverec_q2, pocet_ctvercu);
  slunce = new tslunce(310.0f * M_PI/180.0f);
  hvezdy = new thvezdy(true);
  delete buffer;
  init_ok = false;
}

teren::~teren()
{
  delete [] ctverec_dl;
  delete [] ctverec_q1;
  delete [] ctverec_q2;

  delete slunce;
  delete hvezdy;
  delete stromy;
  delete [] body;
  delete [] silnice;
  delete [] typ_povrchu;
}

void teren::init(){
    terrainTextures = new TerrainTextures();
    shader.create("data/shaders/terrain");
    init_ok = true;
}


void teren::deinit()
{
  if (terrainTextures)
  {
    delete terrainTextures;
    terrainTextures = NULL;
  }

  for (int i = 0; i < pocet_ctvercu; i++)
  {
    if (ctverec_dl[i] != 0) glDeleteLists(ctverec_dl[i], 1);
    ctverec_dl[i] = 0;
  }

  stromy->deinit();
  shader.release();
}


void teren::cyklus(bool plus, bool minus, double *pozice_kam, double uhel_kam, const Frustum& frustum){
    if (!init_ok) return;
  kopni_pole(pozice_kam, pozice_kamery, 3);
  slunce->cyklus(plus,minus,pozice_kamery);
  hvezdy->cyklus(pozice_kamery);
  stromy->Cyklus(pozice_kam);
  int zaok_x = ((int)floor(fabs(pozice_kamery[0]))) / 2;
  int zaok_y = ((int)floor(fabs(pozice_kamery[1]))) / 2;
  if (pozice_kamery[0] < 0.0) zaok_x = - zaok_x - 1;
  if (pozice_kamery[1] < 0.0) zaok_y = - zaok_y - 1;

  int px = zaok_x + sirka/2;
  int py = zaok_y + vyska/2;
  px = px / VELIKOST_CTVERCE;           // prevede souradnice bodu na souradnice display listu
  py = py / VELIKOST_CTVERCE;
  const int POLOMER4 = 9;
  const int POLOMER3 = 3;
  const int POLOMER2 = 2;
  const int POLOMER1 = 1;

  const int SIRKA_CTVERCE = sirka/VELIKOST_CTVERCE;

  float tileRadius = sqrt(VELIKOST_CTVERCE * 2.0f * VELIKOST_CTVERCE * 2.0f + VELIKOST_CTVERCE * 2.0f * VELIKOST_CTVERCE * 2.0f);
  smaz_pole(ctverec_q1, pocet_ctvercu);
  // urceni kvality zobrazeni
  for (int y = py - POLOMER4; y < py + POLOMER4+1; y++)
  {
    if ((y < 1) || (y*VELIKOST_CTVERCE >= sirka - 2*VELIKOST_CTVERCE)) continue;
    for (int x = px - POLOMER4; x < px + POLOMER4+1; x++)
    {
      if ((x < 1) || (x*VELIKOST_CTVERCE >= sirka-2*VELIKOST_CTVERCE)) continue;

      pozice3D pos;
      NactiBod(x * VELIKOST_CTVERCE + VELIKOST_CTVERCE/2, y * VELIKOST_CTVERCE + VELIKOST_CTVERCE/2, pos);
      Vec3D center(pos);
      if (!frustum.IsIn(center, tileRadius))
      {
        continue;
      }

      double radius = (x - px)*(x - px) + (y - py)*(y - py);
      if (radius <= POLOMER4*POLOMER4)
        ctverec_q1[xy_to_i(x,y,SIRKA_CTVERCE)] = KVALITA[2];
      if (PLACKA) continue;
      if (radius <= POLOMER2*POLOMER2)
        ctverec_q1[xy_to_i(x,y,SIRKA_CTVERCE)] = KVALITA[1];
      if (((x - px <= POLOMER1) && (x - px >= -POLOMER1)) && ((y - py <= POLOMER1) && (y - py >= -POLOMER1)))
              ctverec_q1[xy_to_i(x,y,SIRKA_CTVERCE)] = KVALITA[0];
    }

  }
}







void teren::kresli_ctverec(int poc_x, int poc_y, int kvalita){          // Vykresli ctverec. Ctverec se sklada z 2x2 policek v tycoonovi.
  const int POCET_BODU = 1 + 2 + (VELIKOST_CTVERCE / kvalita);          // pocet bodu na ctverec + 2 okrajove + jeden navic
  const int xm = (2 * poc_x) - sirka;                                   // prevede pocatecni bod v tabulce bodu na svetove souradnice v metrech
  const int ym = (2 * poc_y) - vyska;
  double ctverec[POCET_BODU*POCET_BODU][3];                              // sem se ulozi x,y,z vsech bodu zobrazovane casti + 2 okrajove radky a sloupce
  double normal[(POCET_BODU-2)*(POCET_BODU-2)][3];                       // normalove vektory vsech bodu ctverce
  double texcoord[(POCET_BODU-2)*(POCET_BODU-2)][2];                     // souradnice pro mapovani textury
  double nx[3];                                                          // pomocny vektor pro vypocet normal
  double ny[3];                                                          // pomocny vektor pro vypocet normal



  // urceni texturovacich souradnic
  double textura_krok = 2.0/((double)(POCET_BODU-3));
  for (int y = 0; y < POCET_BODU-2; y++)
    for (int x = 0; x < POCET_BODU-2; x++){
      texcoord[xy_to_i(x,y,POCET_BODU-2)][0] = ((double)x) * textura_krok;
      texcoord[xy_to_i(x,y,POCET_BODU-2)][1] = ((double)y) * textura_krok;
    }



  // nacte body ctverce a okoli
  for (int y = 0; y < POCET_BODU; y++)
    for (int x = 0; x < POCET_BODU; x++){
      ctverec[xy_to_i(x,y,POCET_BODU)][0] = xm + 2*(x-1)*kvalita;
      ctverec[xy_to_i(x,y,POCET_BODU)][1] = ym + 2*(y-1)*kvalita;
      ctverec[xy_to_i(x,y,POCET_BODU)][2] = dekodujf(body[2*xy_to_i(poc_x + (x-1)*kvalita, poc_y + (y-1)*kvalita, sirka)], body[2*xy_to_i(poc_x + (x-1)*kvalita, poc_y + (y-1)*kvalita, sirka)+1]);
    }
  // vypocita normaly
  for (int y = 1; y < POCET_BODU - 1; y++)
    for (int x = 1; x < POCET_BODU - 1; x++){
       for (int i = 0; i < 3; i++){
         nx[i] = ctverec[xy_to_i(x+1,y,POCET_BODU)][i] - ctverec[xy_to_i(x-1,y,POCET_BODU)][i];
         ny[i] = ctverec[xy_to_i(x,y+1,POCET_BODU)][i] - ctverec[xy_to_i(x,y-1,POCET_BODU)][i];
       }
       normalizuj(nx, 3);
       normalizuj(ny, 3);
       normala(nx, ny, normal[xy_to_i(x-1, y-1, POCET_BODU-2)]);
    }

  // pokud je nejhorsi kvalita zobrazeni, tak pouzije jen texturu pozadi. (nezobrazi silnice atd.)
  double pole_bodu[4][3];
  double pole_texc[4][2];
  double pole_normal[4][3];
  int tyc_x;
  int tyc_y;

  glEnable(GL_TEXTURE_2D);
  // vykresli ctverce


  for (int y = 1; y < POCET_BODU - 2; y++)
    for (int x = 1; x < POCET_BODU - 2; x++){
       tyc_x = ((poc_x + (x - 1) * kvalita)/10);
       tyc_y = ((poc_y + (y - 1) * kvalita)/10);
       //glBindTexture (GL_TEXTURE_2D, zjisti_texturu(tyc_x, tyc_y));

      glMatrixMode(GL_TEXTURE);
      glPushMatrix();
      glMatrixMode(GL_MODELVIEW);

      nastavTextury(tyc_x, tyc_y);



       // zkopirovani bodu
       kopni_pole(ctverec[xy_to_i(x,y,POCET_BODU)], pole_bodu[0], 3);
       kopni_pole(ctverec[xy_to_i(x+1,y,POCET_BODU)], pole_bodu[1], 3);
       kopni_pole(ctverec[xy_to_i(x+1,y+1,POCET_BODU)], pole_bodu[2], 3);
       kopni_pole(ctverec[xy_to_i(x,y+1,POCET_BODU)], pole_bodu[3], 3);

       // zkopirovani normal
       kopni_pole(normal[xy_to_i(x-1,y-1, POCET_BODU-2)], pole_normal[0], 3);
       kopni_pole(normal[xy_to_i(x,y-1, POCET_BODU-2)], pole_normal[1], 3);
       kopni_pole(normal[xy_to_i(x,y, POCET_BODU-2)], pole_normal[2], 3);
       kopni_pole(normal[xy_to_i(x-1,y, POCET_BODU-2)], pole_normal[3], 3);

       // vytvoreni souradnic textur
       kopni_pole(texcoord[xy_to_i(x-1,y-1, POCET_BODU-2)], pole_texc[0], 2);
       kopni_pole(texcoord[xy_to_i(x,y-1, POCET_BODU-2)], pole_texc[1], 2);
       kopni_pole(texcoord[xy_to_i(x,y, POCET_BODU-2)], pole_texc[2], 2);
       kopni_pole(texcoord[xy_to_i(x-1,y, POCET_BODU-2)], pole_texc[3], 2);

       glBegin(GL_TRIANGLES);
       for (int i = 0; i < 3; i++){
         glTexCoord2d(pole_texc[i][0],pole_texc[i][1]);
         glNormal3dv(pole_normal[i]);
         glVertex3dv(pole_bodu[i]);
       }
       for (int i = 0; i < 4; i++){
         if (i == 1) continue;
         glTexCoord2d(pole_texc[i][0],pole_texc[i][1]);
         glNormal3dv(pole_normal[i]);
         glVertex3dv(pole_bodu[i]);
       }
       glEnd();
       glMatrixMode(GL_TEXTURE);
       glPopMatrix();
       glMatrixMode(GL_MODELVIEW);

    }

  typedef void (APIENTRY * glActiveTextureFunc)(GLenum  	texture);
  glActiveTextureFunc glActiveTexture = NULL;
  glActiveTexture = (glActiveTextureFunc) SDL_GL_GetProcAddress("glActiveTexture");
  if (!glActiveTexture)
  {
     cout << "glActiveTexture not available! " << endl;
     exit(1);
  }

  glActiveTexture(GL_TEXTURE0);

}


const int HLINA = 0;
const int TRAVA = 1;
const int SNIH25 = 2;
const int SNIH50 = 3;
const int SNIH75 = 4;
const int SNIH = 5;
const int KAMENY = 6;
const int POLE = 7;

GLuint teren::zjisti_texturu(int tyc_x, int tyc_y){                             // funkce na zaklade souradnic v tycoonovi vrati spravnou texturu povrchu
  int temp_povrch = typ_povrchu[xy_to_i(tyc_x, tyc_y, 255)];                    // pozadi
  if ( temp_povrch == 7) temp_povrch = 1;                                       // misto pole bude trava
  surface_texture texture;
  switch (temp_povrch){
        case 0: texture = TEXTURE_MUD; break;
        case 1: texture = TEXTURE_GRASS; break;
        case 2: texture = TEXTURE_SNOW25; break;
        case 3: texture = TEXTURE_SNOW50; break;
        case 4: texture = TEXTURE_SNOW75; break;
        case 5: texture = TEXTURE_SNOW; break;
        case 6: texture = TEXTURE_MUD; break;
        default: texture = TEXTURE_MUD; break;
  }
  return terrainTextures->getSurfaceTexture(texture);

}

void teren::nastavTextury(int tyc_x, int tyc_y){


    // POVRCH
  int temp = ((silnice[xy_to_i(tyc_x+1, tyc_y, 255)]) & 0xF0) >> 4;             // typ silnice
  if ((temp == 1) || (temp == 2) || (temp == 4) || (temp == 8)) temp = 0;       // odstraneni koncu silnic
  int temp_povrch = typ_povrchu[xy_to_i(tyc_x, tyc_y, 255)];                    // pozadi
  if ( temp_povrch == 7) temp_povrch = 1;                                       // misto pole bude trava

  surface_texture texture;
  switch (temp_povrch){
        case 0: texture = TEXTURE_MUD; break;
        case 1: texture = TEXTURE_GRASS; break;
        case 2: texture = TEXTURE_SNOW25; break;
        case 3: texture = TEXTURE_SNOW50; break;
        case 4: texture = TEXTURE_SNOW75; break;
        case 5: texture = TEXTURE_SNOW; break;
        case 6: texture = TEXTURE_MUD; break;
        default: texture = TEXTURE_MUD; break;
  }

  typedef void (APIENTRY * glActiveTextureFunc)(GLenum  	texture);
  glActiveTextureFunc glActiveTexture = NULL;
  glActiveTexture = (glActiveTextureFunc) SDL_GL_GetProcAddress("glActiveTexture");
  if (!glActiveTexture)
  {
     cout << "glActiveTexture not available! " << endl;
     exit(1);
  }
  typedef void (APIENTRY * glUniform1iFunc)(GLint location, GLint v0);
  glUniform1iFunc glUniform1i = NULL;
  glUniform1i = (glUniform1iFunc) SDL_GL_GetProcAddress("glUniform1i");
  if (!glUniform1i)
  {
     cout << "glUniform1i not available! " << endl;
     exit(1);
  }
  typedef GLint (APIENTRY * glGetUniformLocationARBFunc)(GLhandleARB program, const char *name);
  glGetUniformLocationARBFunc glGetUniformLocationARB = NULL;
  glGetUniformLocationARB = (glGetUniformLocationARBFunc) SDL_GL_GetProcAddress("glGetUniformLocationARB");
  if (!glGetUniformLocationARB)
  {
     cout << "glGetUniformLocationARB not available! " << endl;
     exit(1);
  }

  glActiveTexture(GL_TEXTURE0);
  terrainTextures->bindSurfaceTexture(texture);

  GLuint location = glGetUniformLocationARB(shader.getID(), "surface");
  glUniform1i(location, 0);


/*
  // NORMALOVA MAPA POVRCHU
  glActiveTexture(GL_TEXTURE2);
  terrainTextures->bindSurfaceNormalTexture(TEXTURE_GRASS);

  location = glGetUniformLocation(shader.getID(), "surface_normal");
  glUniform1i(location, 2);
*/


  // SILNICE
  location = glGetUniformLocationARB(shader.getID(), "use_road");
  if (temp == 0) {
      glUniform1i(location, 0);
      return;
  } else {
      glUniform1i(location, 1);
  }



  road_texture rtexture = TEXTURE_CROSS;
  double rotate = 0.0;
  switch (temp){
      case 5:
          rtexture = TEXTURE_STRAIGHT;
          break;
      case 10:
          rtexture = TEXTURE_STRAIGHT;
          rotate = 90.0;
          break;
      case 9:
          rtexture = TEXTURE_CURVE;
          rotate = 0.0;
          break;
      case 3:
          rtexture = TEXTURE_CURVE;
          rotate = 90.0;
          break;
      case 6:
          rtexture = TEXTURE_CURVE;
          rotate = 180.0;
          break;
      case 12:
          rtexture = TEXTURE_CURVE;
          rotate = -90.0;
          break;
      case 7:
          rtexture = TEXTURE_HALFCROSS;
          rotate = 180.0;
          break;
      case 11:
          rtexture = TEXTURE_HALFCROSS;
          rotate = 90.0;
          break;
      case 14:
          rtexture = TEXTURE_HALFCROSS;
          rotate = -90.0;
          break;
      case 13:
          rtexture = TEXTURE_HALFCROSS;
          rotate = 0.0;
          break;
      case 15:
          rtexture = TEXTURE_CROSS;
          break;

      default:break;
  }


  glActiveTexture(GL_TEXTURE1);
  terrainTextures->bindRoadTexture(rtexture);

  glMatrixMode(GL_TEXTURE);
  glPushMatrix();
  glLoadIdentity();
  glRotatef(rotate,0.0, 0.0, 1.0);
  glMatrixMode(GL_MODELVIEW);

  location = glGetUniformLocationARB(shader.getID(), "road");
  glUniform1i(location, 1);

  glActiveTexture(GL_TEXTURE0);
}



void teren::kresli(){
  if (!init_ok) return;

  glDisable(GL_TEXTURE_2D);

// hvezdy
  hvezdy->kresli();


// slunce
  slunce->kresli();



  //obloha
  glPushMatrix();
  glEnable(GL_COLOR_MATERIAL);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_CULL_FACE);
  glTranslatef(pozice_kamery[0],pozice_kamery[1],pozice_kamery[2]);

  glColor3f(sqrtf(slunce->Ra), sqrtf(slunce->Ga), sqrtf(slunce->Ba));
  //glutSolidSphere(velikost*2.0, 20,20);
    GLUquadric *quadric;
    quadric=gluNewQuadric();
    gluQuadricDrawStyle(quadric, GLU_FILL);
    gluQuadricNormals(quadric, GLU_SMOOTH);

    gluSphere(quadric, velikost*2.0, 20, 20);
    gluDeleteQuadric(quadric);

  glDisable(GL_COLOR_MATERIAL);
  glEnable(GL_TEXTURE_2D);
  glPopMatrix();

  // teren
    // vytvoreni displaylistu (v pripade zmeny kvality vlivem presunu pozorovatele)
  for (int i = 0; i < pocet_ctvercu; i++){
    if (ctverec_q1[i] != ctverec_q2[i]){
      if (ctverec_dl[i] != 0) glDeleteLists(ctverec_dl[i], 1);
      ctverec_dl[i] = 0;
      if (ctverec_q1[i] == 0) continue;
      ctverec_dl[i] = glGenLists(1);
      glNewList(ctverec_dl[i], GL_COMPILE);
      kresli_ctverec((i%(sirka/VELIKOST_CTVERCE))*VELIKOST_CTVERCE, (i/(sirka/VELIKOST_CTVERCE))*VELIKOST_CTVERCE, ctverec_q1[i]);
      glEndList();
    }
  }
  kopni_pole(ctverec_q1, ctverec_q2, pocet_ctvercu);


  typedef GLint (APIENTRY * glGetUniformLocationARBFunc)(GLhandleARB program, const char *name);
  glGetUniformLocationARBFunc glGetUniformLocationARB = NULL;
  glGetUniformLocationARB = (glGetUniformLocationARBFunc) SDL_GL_GetProcAddress("glGetUniformLocationARB");
  if (!glGetUniformLocationARB)
  {
     cout << "glGetUniformLocationARB not available! " << endl;
     exit(1);
  }
  typedef void (APIENTRY * glUniform4fFunc)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
  glUniform4fFunc glUniform4f = NULL;
  glUniform4f = (glUniform4fFunc) SDL_GL_GetProcAddress("glUniform4f");
  if (!glUniform4f)
  {
     cout << "glUniform4f not available! " << endl;
     exit(1);
  }

  shader.bind();
  GLuint location = glGetUniformLocationARB(shader.getID(), "light_position");
  glUniform4f(location, slunce->pozice[0], slunce->pozice[1], slunce->pozice[2], 1.0);

  int counter = 0;
  for (int i = 0; i < pocet_ctvercu; i++){
        if (ctverec_q1[i] != 0) {
            glCallList(ctverec_dl[i]);
            counter++;
        }
  }


  // mezery mezi ctverci
   glColor3f(1.0,1.0,1.0);
   const int SIRKA_CTVERCE = sirka/VELIKOST_CTVERCE;
   int curr;
   int next;
   int px;
   int py;
   int min_q;
   int max_q;
   double textura_min_krok;
   double textura_max_krok;
   double coord[2];
   double bod[3];
   double min_z; // hodnota nejmensi z souradnice (referencni hodnota pro texturovani)
   double min_xy;
   double pole_z[20];
   int pozice = 0;
   for (int y = 1; y < SIRKA_CTVERCE-1; y++)
     for (int x = 1; x < SIRKA_CTVERCE-1; x++){
       // vertikalne
       curr = ctverec_q1[xy_to_i(x-1,y, SIRKA_CTVERCE)];
       next = ctverec_q1[xy_to_i(x,y, SIRKA_CTVERCE)];
       if ((curr != 0) && (next != 0) && (curr != next)){
         px = x * VELIKOST_CTVERCE;
         py = y * VELIKOST_CTVERCE;
         min_q = max(curr, next);
         max_q = min(curr, next);


         min_xy = (2*py) - vyska;
         for (int y1 = py; y1 < py + VELIKOST_CTVERCE; y1+=min_q){
           // zjisteni nejmensi z hodnoty
           if (y1 - py == 0){
             pozice = 0;
             for (int i = py; i <= py + VELIKOST_CTVERCE/2; i+=min_q)
               pole_z[pozice++] = dekodujf(body[2*(xy_to_i(px,i,sirka))], body[2*(xy_to_i(px,i,sirka))+1]);
             min_z = MatPoleMin(pole_z, pozice);
             //glBindTexture (GL_TEXTURE_2D, zjisti_texturu(px/10, y1/10));
             nastavTextury(px/10, y1/10);
           }
           if (y1 - py == 10){
             pozice = 0;
             for (int i = py+10; i <= py + VELIKOST_CTVERCE; i+=min_q)
               pole_z[pozice++] = dekodujf(body[2*(xy_to_i(px,i,sirka))], body[2*(xy_to_i(px,i,sirka))+1]);
             min_z = MatPoleMin(pole_z, pozice);
             //glBindTexture (GL_TEXTURE_2D, zjisti_texturu(px/10, y1/10));
             nastavTextury(px/10, y1/10);
           }

           glBegin(GL_TRIANGLE_FAN);
           bod[0] = (2*px) - sirka;
           bod[1] = (2*y1) - vyska;
           bod[2] = dekodujf(body[2*(xy_to_i(px,y1,sirka))], body[2*(xy_to_i(px,y1,sirka))+1]);
           coord[1] = (bod[1] - min_xy)/20.0;
           coord[0] = (bod[2] - min_z)/20.0;
           glTexCoord2dv(coord);
           glNormal3f(0.0,0.0,1.0);
           glVertex3dv(bod);
           for (int y2 = y1 + max_q; y2 <= y1 + min_q; y2+=max_q){
             bod[0] = (2*px) - sirka;
             bod[1] = (2*y2) - vyska;
             bod[2] = dekodujf(body[2*(xy_to_i(px,y2,sirka))], body[2*(xy_to_i(px,y2,sirka))+1]);
             coord[1] = (bod[1] - min_xy)/20.0;
             coord[0] = (bod[2] - min_z)/20.0;
             glTexCoord2dv(coord);
             glNormal3f(0.0,0.0,1.0);
             glVertex3dv(bod);
           }
           glEnd();



           glMatrixMode(GL_TEXTURE);
           glPopMatrix();
           glMatrixMode(GL_MODELVIEW);

         }
       }

       // horizontalne
       curr = ctverec_q1[xy_to_i(x,y-1, SIRKA_CTVERCE)];
       next = ctverec_q1[xy_to_i(x,y, SIRKA_CTVERCE)];
       if ((curr != 0) && (next != 0) && (curr != next)){
         px = x * VELIKOST_CTVERCE;
         py = y * VELIKOST_CTVERCE;
         min_q = max(curr, next);
         max_q = min(curr, next);

         min_xy = (2*px) - sirka;
         for (int x1 = px; x1 < px + VELIKOST_CTVERCE; x1+=min_q){
           // zjisteni nejmensi z hodnoty
           if (x1 - px == 0){
             pozice = 0;
             for (int i = px; i <= px + VELIKOST_CTVERCE/2; i+=min_q)
               pole_z[pozice++] = dekodujf(body[2*(xy_to_i(i,py,sirka))], body[2*(xy_to_i(i,py,sirka))+1]);
             min_z = MatPoleMin(pole_z, pozice);
             //glBindTexture (GL_TEXTURE_2D, zjisti_texturu(x1/10, py/10));
             nastavTextury(x1/10, py/10);
           }
           if (x1 - px == 10){
             pozice = 0;
             for (int i = px+10; i <= px + VELIKOST_CTVERCE; i+=min_q)
               pole_z[pozice++] = dekodujf(body[2*(xy_to_i(i,py,sirka))], body[2*(xy_to_i(i,py,sirka))+1]);
             min_z = MatPoleMin(pole_z, pozice);
             //glBindTexture (GL_TEXTURE_2D, zjisti_texturu(x1/10, py/10));
             nastavTextury(x1/10, py/10);
           }
           glBegin(GL_TRIANGLE_FAN);
           bod[0] = (2*x1) - sirka;
           bod[1] = (2*py) - vyska;
           bod[2] = dekodujf(body[2*(xy_to_i(x1,py,sirka))], body[2*(xy_to_i(x1,py,sirka))+1]);
           coord[0] = (bod[0] - min_xy)/20.0;
           coord[1] = (bod[2] - min_z)/10.0;
           glTexCoord2dv(coord);
           glNormal3f(0.0,0.0,1.0);
           glVertex3dv(bod);
           for (int x2 = x1 + max_q; x2 <= x1 + min_q; x2+=max_q){
             bod[0] = (2*x2) - sirka;
             bod[1] = (2*py) - vyska;
             bod[2] = dekodujf(body[2*(xy_to_i(x2,py,sirka))], body[2*(xy_to_i(x2,py,sirka))+1]);
             coord[0] = (bod[0] - min_xy)/20.0;
             coord[1] = (bod[2] - min_z)/10.0;
             glTexCoord2dv(coord);
             glNormal3f(0.0,0.0,1.0);
             glVertex3dv(bod);
           }
           glEnd();

           glMatrixMode(GL_TEXTURE);
           glPopMatrix();
           glMatrixMode(GL_MODELVIEW);
         }
       }
   }

    shader.unbind();
  glDisable(GL_COLOR_MATERIAL);
  glEnable(GL_CULL_FACE);

  // stromy
  stromy->Kresli(ctverec_q1, sirka/VELIKOST_CTVERCE);
}


double teren::Povrch(pozice3D p, bool& isOnRoad){
  int x = GetIndex(p[X]) / 10;
  int y = GetIndex(p[Y]) / 10;
  int index = xy_to_i(x, y, 255);
  int povrch = typ_povrchu[index];


  double pov = 1000.0;
  switch (povrch){
    case HLINA:
      pov = 1500.0;
    break;
    case TRAVA:
      pov = 1000.0;
    break;
    case SNIH25:
      pov = 800.0;
    break;
    case SNIH50:
      pov = 700.0;
    break;
    case SNIH75:
      pov = 600.0;
    break;
    case SNIH:
      pov = 500.0;
    break;
    case KAMENY:
      pov = 2000.0;
    break;
  }

  index = xy_to_i(x+1, y, 255);
  unsigned char temp = (silnice[index])&0xF0;

  x = GetIndex(p[X])%10;
  y = GetIndex(p[Y])%10;
  const double SILNICE = 2000.0f;
  bool jetam = false;
  if ((temp == 1) || (temp == 2) || (temp == 4) || (temp == 8)) temp = 0;
  if (temp != 0){
    if ((x >= 3) && (x <= 6) && (y >= 3) && (y <= 6))
      jetam = true;
    if (temp&SILNICE_JIH)
      if ((x >= 3) && (x <= 6) && (y >= 6))
        jetam = true;
    if (temp&SILNICE_SEVER)
      if ((x >= 3) && (x <= 6) && (y <= 3))
        jetam = true;
    if (temp&SILNICE_ZAPAD)
      if ((x >= 6) && (y >= 3) && (y <= 6))
        jetam = true;
    if (temp&SILNICE_VYCHOD)
      if ((x <= 3) && (y >= 3) && (y <= 6))
        jetam = true;
  }
  isOnRoad = false;
  if (jetam)
  {
    pov = SILNICE;
    isOnRoad = true;
  }
  return pov;
}


void teren::Rovnice_roviny(int x, int y, double *rovnice1, double *rovnice2){
   pozice3D bod[4];
   vektor3D normal[2];

   // zjisteni absolutnich souradnic bodu
   NactiBod(x,y, bod[0]);
   NactiBod(x+1,y, bod[1]);
   NactiBod(x+1,y+1, bod[2]);
   NactiBod(x,y+1, bod[3]);

   // vypocet normaloveho vektoru
   vektor3D v1;
   vektor3D v2;
   vektor3D v3;
   vektor3D v4;

   for (int i = 0; i < 3; i++){
     v1[i] = bod[1][i] - bod[0][i];
     v2[i] = bod[2][i] - bod[1][i];
     v3[i] = bod[3][i] - bod[2][i];
     v4[i] = bod[0][i] - bod[3][i];
   }

   MatVektorJednot(v1);
   MatVektorJednot(v2);
   MatVektorJednot(v3);
   MatVektorJednot(v4);
   MatVektorNormal(v1, v2, normal[0]);
   MatVektorNormal(v3, v4, normal[1]);

   if (normal[0][2] == 0.0) exit(0);
   if (normal[1][2] == 0.0) exit(0);

   for (int i = 0; i < 3; i++){
     rovnice1[i] = normal[0][i];
     rovnice2[i] = normal[1][i];
   }
   rovnice1[3] = - (normal[0][0] * bod[0][0] + normal[0][1] * bod[0][1] + normal[0][2] * bod[0][2]);
   rovnice2[3] = - (normal[1][0] * bod[0][0] + normal[1][1] * bod[0][1] + normal[1][2] * bod[0][2]);
}


void teren::NactiBod(int x, int y, pozice3D souradnice){
  souradnice[0] = x*2 - sirka;
  souradnice[1] = y*2 - vyska;
  souradnice[2] = (double)dekodujf(body[2*(xy_to_i(x,y,sirka))], body[2*(xy_to_i(x,y,sirka))+1]);
}


int teren::GetIndex(double s){
  int zaok = ((int)floor(fabs(s))) / 2;
  if (s < 0.0) zaok = - zaok - 1;
  return (zaok + sirka/2);
}

bool teren::Prusecik_n(const Vec3D& bod, Vec3D* vysledek, Vec3D* normal){
  int x = GetIndex(bod.getX());
  int y = GetIndex(bod.getY());
  double rovnice[2][4];
  int trojuhelnik;
  pozice3D bod1;
  pozice3D bod3;
  NactiBod(x+1, y, bod1);
  NactiBod(x, y+1, bod3);
  bod1[2] = 0.0;
  bod3[2] = 0.0;
  pozice3D prusecik = {bod.getX(),bod.getY(),0.0};
  double vzd1 = MatPoziceVzdalenost(prusecik, bod1);
  double vzd3 = MatPoziceVzdalenost(prusecik, bod3);
  if (vzd1 < vzd3) trojuhelnik = 0;
    else trojuhelnik = 1;
  Rovnice_roviny(x, y, rovnice[0], rovnice[1]);
  double array[3];
  bod.toArray(array);
  double vysledekArray[3];

  MatRovinaPrusecik(rovnice[trojuhelnik], array, rovnice[trojuhelnik], vysledekArray);
  normal->setX(rovnice[trojuhelnik][0]);
  normal->setY(rovnice[trojuhelnik][1]);
  normal->setZ(rovnice[trojuhelnik][2]);

  vysledek->setX(vysledekArray[0]);
  vysledek->setY(vysledekArray[1]);
  vysledek->setZ(vysledekArray[2]);

  return JeTam(bod);
}

void teren::Normala_roviny(pozice3D bod, vektor3D vysledek){
  int x = GetIndex(bod[0]);
  int y = GetIndex(bod[1]);
  double rovnice[2][4];
  int trojuhelnik;
  pozice3D bod1;
  pozice3D bod3;
  NactiBod(x+1, y, bod1);
  NactiBod(x, y+1, bod3);
  bod1[2] = 0.0;
  bod3[2] = 0.0;
  pozice3D prusecik = {bod[0],bod[1],0.0};
  double vzd1 = MatPoziceVzdalenost(prusecik, bod1);
  double vzd3 = MatPoziceVzdalenost(prusecik, bod3);
  if (vzd1 < vzd3) trojuhelnik = 0;
    else trojuhelnik = 1;
  Rovnice_roviny(x, y, rovnice[0], rovnice[1]);
  MatVektorKopni(rovnice[trojuhelnik], vysledek);
  MatVektorJednot(vysledek);
}

bool teren::Prusecik(pozice3D bod, vektor3D smer, double vzd, pozice3D prusecik){
  // zatim jen amaterska verze!!!


  const int POCET_KROKU = 100;

  double krok = vzd / ((double)(POCET_KROKU));
  double vzdalenost = vzd;
  bool je = false;
  do {
    for (int i = 0; i < 3; i++)
      prusecik[i] = bod[i] + smer[i] * vzdalenost;
    if (!JeTam(prusecik)) break;
//    if (prusecik[2] >= 25.0) break;
    je = true;
    vzdalenost -= krok;
  } while (vzdalenost > 0.0);
  return je;
}


double teren::Z(double xx, double yy){
  int x = GetIndex(xx);
  int y = GetIndex(yy);
  double rovnice[2][4];
  int trojuhelnik;
  pozice3D bod1;
  pozice3D bod3;
  NactiBod(x+1, y, bod1);
  NactiBod(x, y+1, bod3);
  bod1[2] = 0.0;
  bod3[2] = 0.0;
  pozice3D prusecik = {xx,yy,0.0};
  vektor3D nahoru = {0.0,0.0,1.0};
  double vzd1 = MatPoziceVzdalenost(prusecik, bod1);
  double vzd3 = MatPoziceVzdalenost(prusecik, bod3);
  if (vzd1 < vzd3) trojuhelnik = 0;
    else trojuhelnik = 1;
  Rovnice_roviny(x, y, rovnice[0], rovnice[1]);
  pozice3D vysledek;
  MatRovinaPrusecik(rovnice[trojuhelnik], prusecik, nahoru, vysledek);
  return vysledek[2];
}

double teren::Tvrdost(pozice3D pozice){
  return 10000.0;
}

void teren::Normala_bodu(int x, int y, vektor3D normala){
  pozice3D b[4];
  NactiBod(x-1, y, b[0]);
  NactiBod(x+1, y, b[1]);
  NactiBod(x, y-1, b[2]);
  NactiBod(x, y+1, b[3]);
  vektor3D v[2];
  for (int i = 0; i < 3; i++){
    v[0][i] = b[1][i] - b[0][i];
    v[1][i] = b[3][i] - b[2][i];
  }
  MatVektorJednot(v[0]);
  MatVektorJednot(v[1]);
  MatVektorNormal(v[0], v[1], normala);
}


bool teren::JeTam(const Vec3D& bod){
  double z = Z(bod.getX(), bod.getY());
  if (z > bod.getZ()) return true;
  return false;
}





//------------------------------------------------------------------------------
//                                  STROMY
//------------------------------------------------------------------------------

CStromy::CStromy(bool *_mapa, teren *ter){
  for (int i = 0; i < 255*255; i++){
    mapa[i] = _mapa[i];
    strom[i] = NULL;
  }
  t = ter;
}

CStromy::~CStromy()
{
  for (int i = 0; i < 255*255; i++)
  {
    if (strom[i])
    {
      delete strom[i];
      strom[i] = NULL;
    }
  }
}

void CStromy::init(GLuint *textury){
  glDisable(GL_CULL_FACE);
  glEnable(GL_TEXTURE_2D);
  for (int n = 0; n < STROM_POCET_TYPU; n++)
    for (int i = 0; i < 3; i++)
      DL[n][i] = glGenLists(1);

  // vytvoreni DL
  for (int n = 0; n < STROM_POCET_TYPU; n++){
    // zadni cast
    glNewList(DL[n][2], GL_COMPILE);
      glBindTexture(GL_TEXTURE_2D, textury[n]);
      glBegin(GL_QUADS);
        glTexCoord2f(0.5, 0.0); glVertex3f(0.0, 0.0, 0.0);
        glTexCoord2f(0.5, 1.0); glVertex3f(0.0, 0.0, 1.0);
        glTexCoord2f(1.0, 1.0); glVertex3f(0.0, 0.5, 1.0);
        glTexCoord2f(1.0, 0.0); glVertex3f(0.0, 0.5, 0.0);
      glEnd();
    glEndList();
    // stred
    glNewList(DL[n][1], GL_COMPILE);
      glBindTexture(GL_TEXTURE_2D, textury[n]);
      glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0); glVertex3f(-0.5, 0.0, 0.0);
        glTexCoord2f(1.0, 0.0); glVertex3f(0.5, 0.0, 0.0);
        glTexCoord2f(1.0, 1.0); glVertex3f(0.5, 0.0, 1.0);
        glTexCoord2f(0.0, 1.0); glVertex3f(-0.5, 0.0, 1.0);
      glEnd();
    glEndList();
    // predni cast
    glNewList(DL[n][0], GL_COMPILE);
      glBindTexture(GL_TEXTURE_2D, textury[n]);
      glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0); glVertex3f(0.0, -0.5, 0.0);
        glTexCoord2f(0.5, 0.0); glVertex3f(0.0, 0.0, 0.0);
        glTexCoord2f(0.5, 1.0); glVertex3f(0.0, 0.0, 1.0);
        glTexCoord2f(0.0, 1.0); glVertex3f(0.0, -0.5, 1.0);
      glEnd();
    glEndList();
  }
  glEnable(GL_CULL_FACE);
}

void CStromy::deinit()
{
  for (int n = 0; n < STROM_POCET_TYPU; n++)
    for (int i = 0; i < 3; i++)
      glDeleteLists(DL[n][i], 1);
}

void CStromy::Cyklus(pozice3D _pozice_kamery){

  MatPoziceKopni(_pozice_kamery, pozice_kamery);
  int tyc_x = (t->GetIndex(pozice_kamery[X]) / 10) + 1;
  int tyc_y = t->GetIndex(pozice_kamery[Y]) / 10;

  // urceni viditelnych stromu
  bool viditelny = false;
  int index = 0;
  for (int y = 0; y < 255; y++)
    for (int x = 0; x < 255; x++){
        index = xy_to_i(x,y,255);
        if ((iabs(x-tyc_x) < STROM_DOHLED) && (iabs(y-tyc_y) < STROM_DOHLED))
        {  // policko je v dosahu
            if ((mapa[index]) && (strom[index] == NULL))
            {   // na policku je strom, ktery jeste nebyl vytvoren
                pozice3D temp;
                int povrch = t->typ_povrchu[index];
                bool snih = false;
                if ((povrch == SNIH50) || (povrch == SNIH75) || (povrch == SNIH)) snih = true;
                int typ = 0;
                if (!snih) typ = randomi(3);
                else typ = 4 + randomi(1);
                t->NactiBod(-10 + 2 + randomi(6) +(index%255)*10, 2 + randomi(6)+(index/255)*10, temp);
                strom[index] = new CStrom(temp, DL[typ][2], DL[typ][1], DL[typ][0]);
                strom[index]->Cyklus(pozice_kamery);
            }   // konec vytvoreni stromu
            if (strom[index] != NULL)
            {   // na policku uz je vytvoreny strom
                strom[index]->Cyklus(pozice_kamery);
            }
        }  // "konec policko je v dosahu"
        else
        {   // policko neni v dosahu
            if (strom[index] != NULL){
                delete strom[index];
                strom[index] = NULL;
            }
        }
    } // konec prochazeni stromu

}

void CStromy::Kresli(int* ctverec_q1, int sirka){

  int tyc_x = (t->GetIndex(pozice_kamery[X]) / 10)+1;
  int tyc_y = t->GetIndex(pozice_kamery[Y]) / 10;

  for (int i = STROM_DOHLED; i >= 0; i--)
    for (int x = tyc_x - STROM_DOHLED; x < STROM_DOHLED + tyc_x; x++)
      for (int y = tyc_y - STROM_DOHLED; y < STROM_DOHLED + tyc_y; y++){
        if ((iabs(x - tyc_x) != i) && (iabs(y - tyc_y) != i)) continue;
        if ((x < 0) || (x > 255) || (y < 0) || (y > 255)) continue;
        if (strom[xy_to_i(x, y, 255)] != NULL)
        {
          int qIndex = xy_to_i(x/2, y/2, sirka);
          if (ctverec_q1[qIndex] == 0)
            continue;
          strom[xy_to_i(x, y, 255)]->Kresli();
        }
      }
}



void teren::InitStromy(GLuint *textury){
  stromy->init(textury);
}




//------------------------------------------------------------------------------
//                               SLUNCE
//------------------------------------------------------------------------------

const double rych_otaceni = 0.0001;


tslunce::tslunce(double _uhel){
  uhel = _uhel;
  R = 1.0;
  G = 1.0;
  B = 1.0;
  delta = rych_otaceni;
  double nic[3] = {0.0,0.0,0.0};
  vektor[0] = 0.0;
  vektor[1] = 0.0;
  vektor[2] = 1.0;
}


void tslunce::cyklus(bool plus, bool minus, double *pozice_kamery){
  vektor[0] = 0.0;
  vektor[1] = 0.0;
  vektor[2] = 1.0;

  kopni_pole(pozice_kamery, pozice, 3);

  double vzd = velikost * 0.9;
  if (plus+minus == false){
    if (delta > rych_otaceni) delta -= fabs(delta - rych_otaceni) / 10.0;
    if (delta < rych_otaceni) delta += fabs(delta - rych_otaceni) / 10.0;
  }
  if (plus)
    if (delta < rych_otaceni * 100.0) delta += fabs(rych_otaceni*100.0 - delta) / 10.0;
  if (minus)
    if (delta > -rych_otaceni * 100.0) delta -= fabs(- rych_otaceni*100.0 - delta) / 10.0;
  uhel += delta;

  while (uhel < 0.0) uhel += 2 * PI;
  while (uhel > 2.0 * PI) uhel -= 2.0 * PI;


  rotace(vektor, uhel, 1);
  for (int i = 0; i < 3; i++)
    pozice[i] = pozice[i] + vzd * vektor[i] * 2.0;

  // barva svetla
  double u = fabs(cos(uhel/2.0));



  R = u*0.9;
  G = u*u*u*0.9;
  B = u*u*u*u*u*1.0;

  Ra = R * 0.6;
  Ga = G * 0.6;
  Ba = B * 0.7;
  if (Ra < 0.1) Ra = 0.1;
  if (Ga < 0.1) Ga = 0.1;
  if (Ba < 0.3) Ba = 0.3;
}


void tslunce::kresli(){
  glDisable(GL_BLEND);
  glDisable(GL_CULL_FACE);
  glDisable(GL_TEXTURE_2D);
  float mat[4] = {R,G,B,0.5};
  float nic[4] = {0.0,0.0,0.0,0.5};
  glMaterialfv (GL_FRONT_AND_BACK, GL_EMISSION, mat);
  glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT, mat);
  glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, mat);

  glPushMatrix();
    glTranslatef(pozice[0], pozice[1], pozice[2]);
    glRotatef(stupne(uhel), 0.0,1.0,0.0);
    glBegin(GL_TRIANGLE_FAN);
      glVertex3f(0.0, 0.0, 0.0);
      double polomer = 30.0;
      for (double i = 0.0; i <= 2*PI; i+=radiany(10.0))
        glVertex3f(polomer * cos(i), polomer * sin(i), 0.0);
    glEnd();


  glEnable(GL_TEXTURE_2D);
  glEnable(GL_CULL_FACE);
  glPopMatrix();
  glMaterialfv (GL_FRONT_AND_BACK, GL_EMISSION, nic);
  glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT, nic);
  glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, nic);




  float v[4];
  for (int i = 0; i < 3; i++)
    v[i] = vektor[i];
  v[3] = 0.0;

  glLightfv(GL_LIGHT1, GL_POSITION,v);

  float amb[4] = {Ra*1.5,Ga*1.5,Ba*1.5,0.5};
  float dif[4] = {R*1.5,G*1.5,B*1.5,0.5};
  float spec[4] = {0.5,0.5,0.5,0.5};

  glLightfv(GL_LIGHT1, GL_AMBIENT, amb);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, dif);
  glLightfv(GL_LIGHT1, GL_SPECULAR, dif);
}



//------------------------------------------------------------------------------
//                               HVEZDY
//------------------------------------------------------------------------------

thvezdy::thvezdy(bool enable){
  zobraz = enable;
  double def[3] = {0.0,0.0, 1.0};
  double temp[3] = {0.0,0.0,0.0};
  for (int i = 0; i < pocet_hvezd; i++){
    kopni_pole(def, temp, 3);
    rotace(temp, PI - random(2.0*PI), 0);
    rotace(temp, PI - random(2.0*PI), 1);
    for (int n = 0; n < 3; n++)
      pozice[i][n] = temp[n] * velikost * 0.95 * 2.0;
  }
}

void thvezdy::kresli(){
  if (!zobraz) return;
  glPushMatrix();
  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_COLOR_MATERIAL);

  float mat[4] = {1.0,1.0,1.0,1.0};
  float nic[4] = {0.0,0.0,0.0,0.5};
  glMaterialfv (GL_FRONT_AND_BACK, GL_EMISSION, mat);
  glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT, mat);
  glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, mat);

  glTranslatef(pozice_kamery[X], pozice_kamery[Y], pozice_kamery[Z]);
  glBegin(GL_POINTS);
  for (int i = 0; i < pocet_hvezd; i++)
    glVertex3f(pozice[i][0], pozice[i][1], pozice[i][2]);
  glEnd();

  glMaterialfv (GL_FRONT_AND_BACK, GL_EMISSION, nic);
  glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT, nic);
  glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, nic);

  glEnable(GL_TEXTURE_2D);
  glPopMatrix();
}

void thvezdy::cyklus(pozice3D _pozice_kamery){
  MatPoziceKopni(_pozice_kamery, pozice_kamery);
}






