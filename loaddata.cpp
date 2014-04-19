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


#include "glm.h"
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include "loaddata.h"
#include "tgaload.h"

#include <map>

#include "glm.h"

using namespace std;


void Error(char *text){
  cout << text << endl;
  system("pause");
  exit(1);
}



void EnvTextures(GLuint* textures){
    cout << "Loading environment textures..." << endl;

  glEnable(GL_TEXTURE_2D);
  glGenTextures (6, textures);
  image_t   temp_image;

  glBindTexture ( GL_TEXTURE_CUBE_MAP_POSITIVE_X, textures[0]);
  tgaLoad("data/textury/envmap/posx.tga", &temp_image, TGA_FREE, GL_TEXTURE_CUBE_MAP_POSITIVE_X);

  glBindTexture ( GL_TEXTURE_CUBE_MAP_NEGATIVE_X, textures[1]);
  tgaLoad("data/textury/envmap/negx.tga", &temp_image, TGA_FREE, GL_TEXTURE_CUBE_MAP_NEGATIVE_X);

  glBindTexture ( GL_TEXTURE_CUBE_MAP_POSITIVE_Y, textures[2]);
  tgaLoad("data/textury/envmap/posy.tga", &temp_image, TGA_FREE, GL_TEXTURE_CUBE_MAP_POSITIVE_Y);

  glBindTexture ( GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, textures[3]);
  tgaLoad("data/textury/envmap/negy.tga", &temp_image, TGA_FREE, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);

  glBindTexture ( GL_TEXTURE_CUBE_MAP_POSITIVE_Z, textures[4]);
  tgaLoad("data/textury/envmap/posz.tga", &temp_image, TGA_FREE, GL_TEXTURE_CUBE_MAP_POSITIVE_Z);

  glBindTexture ( GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, textures[5]);
  tgaLoad("data/textury/envmap/negz.tga", &temp_image, TGA_FREE, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);

}



int VelikostSouboru(char *jmeno){
  struct stat results;
  stat(jmeno, &results);
  return results.st_size;
}

// podle typu auta zjisti cestu k souboru
void ZjistiCestu(int typ, char *cesta){
  char *temp;
  switch (typ){
    case TYP_SKODAFAV:
      temp = "data/cars/skodafav/";
    break;
    case TYP_SKODA120:
      temp = "data/cars/skoda120/";
    break;
    case TYP_KAROSA:
      temp = "data/cars/karosa/";
    break;
    case TYP_SUPER120:
      temp = "data/cars/super120/";
    break;
    case TYP_SKODAOCT:
      temp = "data/cars/skodaoct/";
    break;
    case TYP_SKODAOCTTURBO:
      temp = "data/cars/skodaoctturbo/";
      break;
    case TYP_TOYOTA:
      temp = "data/cars/toyota/";
      break;
  }
  strcpy (cesta, temp);
}



// Prohleda buffer a najde zaznam. Do hodnoty ulozi jeho text.
bool NajdiZaznam(char *buffer, char *zaznam, char *hodnota){
  // zjisti delku hledaneho retezce
  int delka = strlen(zaznam);
  if (delka <= 0) return false;
  // prida k hledanemu retezci rovnitko
  char findstr[delka+2];
  for (int i = 0; i < delka; i++)
    findstr[i] = zaznam[i];
  findstr[delka] = '=';
  findstr[delka+1] = NULL;
  // najde hledany retezec
  char *pozice = strstr(buffer, findstr);
  if (pozice == NULL) return false;
  // posune se za rovnitko
  pozice = pozice + (delka+1);
  // zjisti delku zaznamu (je ukoncen \n)
  char *temp = pozice;
  delka = 0;
  while (*temp != '\n'){
    delka++;
    temp = temp+1;
  }
  if (delka == 0) return false;
  // zkopiruje zaznam do noveho stringu
  for (int i = 0; i < delka; i++)
    hodnota[i] = pozice[i];
  hodnota[delka] = NULL;
  return true;
}


// Vrati int hodnotu patrici k danemu zaznamu
bool NactiInt(char *buffer, char *zaznam, int *cislo){
  char hodnota[128];
  if (!NajdiZaznam(buffer, zaznam, hodnota)) return false;
  sscanf(hodnota, "%d", cislo);
  return true;
}

// Vrati double hodnotu patrici k danemu zaznamu
bool NactiFloat(char *buffer, char *zaznam, double *cislo){
  char hodnota[128];
  if (!NajdiZaznam(buffer, zaznam, hodnota)) return false;
  float c;
  sscanf(hodnota, "%f", &c);
  *cislo = c;
  return true;
}

// vrati pole 3 doubleu
bool NactiFloat3(char *buffer, char *zaznam, double *cisla){
  char hodnota[128];
  if (!NajdiZaznam(buffer, zaznam, hodnota)) return false;
  float c[3];
  sscanf(hodnota, "%f, %f, %f", c, c+1, c+2);
  for (int i = 0; i < 3; i++)
    cisla[i] = c[i];
  return true;
}


// vrati pole 11 doubleu
bool NactiFloat11(char *buffer, char *zaznam, double *c){
  char hodnota[128];
  if (!NajdiZaznam(buffer, zaznam, hodnota)) return false;
  float cisla[13];
  sscanf(hodnota, "%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f", cisla, cisla+1, cisla+2, cisla+3, cisla+4, cisla+5, cisla+6, cisla+7, cisla+8, cisla+9, cisla+10);
  for (int i = 0; i < 11; i++)
    c[i] = cisla[i];
  return true;
}
// vrati pole 15 doubleu
bool NactiFloat15(char *buffer, char *zaznam, double *c){
  char hodnota[128];
  if (!NajdiZaznam(buffer, zaznam, hodnota)) return false;
  float cisla[13];
  sscanf(hodnota, "%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f", cisla, cisla+1, cisla+2, cisla+3, cisla+4, cisla+5, cisla+6, cisla+7, cisla+8, cisla+9, cisla+10, cisla+11, cisla+12, cisla+13, cisla+14);
  for (int i = 0; i < 15; i++)
    c[i] = cisla[i];
  return true;
}

// KONSTRUKTOR - otevre soubor a nacte z nej vsechny potrebne data
vehicle_data::vehicle_data(int typ){

  // zjisteni cesty
  char soubor[] = "car.dat";
  char cesta[256];
  ZjistiCestu(typ, cesta);
  strcat (cesta, soubor);

  // otevreni souboru
  int velikost = VelikostSouboru(cesta);
  char buffer[velikost];
  ifstream file(cesta, ios::in | ios::binary);
  if (file == NULL) {
    cout << "Nepodarilo se otevrit DAT soubor." << endl;
    exit(1);
  }
  file.read(buffer, velikost);
  file.close();


  // NACTENI DAT

  // hmotnost bodu
  char temp[] = "HMOTNOST_B ";
  for (int i = 0; i < 8; i++){
    temp[10] = i + 48;
    if (!NactiFloat(buffer, temp, &hmotnost[i])) Error("Chybny car.dat! (HMOTNOST_Bx)");
  }

  // rozmery
  if (!NactiFloat(buffer, "DELKA", &rozmery[X])) Error("Chybny car.dat! (DELKA)");
  if (!NactiFloat(buffer, "SIRKA", &rozmery[Y])) Error("Chybny car.dat! (SIRKA)");
  if (!NactiFloat(buffer, "VYSKA", &rozmery[Z])) Error("Chybny car.dat! (VYSKA)");

  int tempInt;
  if (!NactiInt(buffer, "TEXTURA", &tempInt)) Error("Chybny car.dat! (TEXTURA)");
  textura = (bool)tempInt;

  // zataceni
  if (!NactiFloat(buffer, "MAXZAT", &maxzat)) Error("Chybny car.dat! (MAXZAT)");
  if (!NactiFloat(buffer, "RYCHZAT", &rychzat)) Error("Chybny car.dat! (RYCHZAT)");
  maxzat = MatStupenRadian(maxzat);
  rychzat = MatStupenRadian(rychzat);

  // pozice
  if (!NactiFloat3(buffer, "POZICERIDICE", pozice_ridice)) Error("Chybny car.dat! (POZICERIDICE)");
  if (!NactiFloat3(buffer, "POZICEVYSTUPU", pozice_vystupu)) Error("Chybny car.dat! (POZICEVYSTUPU)");

  // motor
  if (!NactiFloat(buffer, "VYKON", &vykon)) Error("Chybny car.dat! (VYKON)");
  if (!NactiFloat(buffer, "MAXOTACKY", &max_otacky)) Error("Chybny car.dat! (MAXOTACKY)");

  // prevodovka
  if (!NactiInt(buffer, "POCETSTUPNU", &pocet_stupnu)) Error("Chybny car.dat! (POCETSTUPNU)");
  if ((pocet_stupnu < 1) && (pocet_stupnu > 9)) Error("Pocet prevodovych stupnu mimo rozsah!");
  char tmp[] = "POMERR";
  if (!NactiFloat(buffer, tmp, &pomery[0])) Error("Chybny car.dat! (POMERR)");
  pomery[1] = 0.0;
  for (int i = 1; i < pocet_stupnu + 1; i++){
    tmp[5] = i + 48;
    if (!NactiFloat(buffer, tmp, &pomery[i+1])) Error("Chybny car.dat! (POMER)");
  }

  // svetla
  int temp_sv;
  if (!NactiInt(buffer, "SVETLA", &temp_sv)) Error("Chybny car.dat! (SVETLA)");
  svetla = (bool) temp_sv;
  if (!NactiInt(buffer, "SMEROVKY", &temp_sv)) Error("Chybny car.dat! (SMEROVKY)");
  smerovky = (bool) temp_sv;
  if (!NactiInt(buffer, "BRZDA", &temp_sv)) Error("Chybny car.dat! (BRZDA)");
  brzda = (bool) temp_sv;
  if (!NactiInt(buffer, "ZPATECKA", &temp_sv)) Error("Chybny car.dat! (ZPATECKA)");
  zpatecka = (bool) temp_sv;

  // kola
  int temp_kola;
  double temp_kola_f;
  if (!NactiInt(buffer, "TVRDOST_PREDNI", &temp_kola)) Error("Chybny car.dat! (TVRDOST_PREDNI)");
  tvrdost_predni = (double)temp_kola;

  if (!NactiInt(buffer, "TVRDOST_ZADNI", &temp_kola)) Error("Chybny car.dat! (TVRDOST_ZADNI)");
  tvrdost_zadni = (double)temp_kola;

  if (!NactiInt(buffer, "TLUMENI_ZADNI_NARAZ", &temp_kola)) Error("Chybny car.dat! (TLUMENI_ZADNI_NARAZ)");
  tlumeni_zadni_naraz = (double)temp_kola;

  if (!NactiInt(buffer, "TLUMENI_ZADNI_ODRAZ", &temp_kola)) Error("Chybny car.dat! (TLUMENI_ZADNI_ODRAZ)");
  tlumeni_zadni_odraz = (double)temp_kola;

  if (!NactiInt(buffer, "TLUMENI_PREDNI_NARAZ", &temp_kola)) Error("Chybny car.dat! (TLUMENI_PREDNI_NARAZ)");
  tlumeni_predni_naraz = (double)temp_kola;

  if (!NactiInt(buffer, "TLUMENI_PREDNI_ODRAZ", &temp_kola)) Error("Chybny car.dat! (TLUMENI_PREDNI_ODRAZ)");
  tlumeni_predni_odraz = (double)temp_kola;

  if (!NactiFloat(buffer, "POLOMERKOLA_PREDNI", &temp_kola_f)) Error("Chybny car.dat! (POLOMERKOLA_PREDNI)");
  polomer_predni = (double)temp_kola_f;

  if (!NactiFloat(buffer, "POLOMERKOLA_ZADNI", &temp_kola_f)) Error("Chybny car.dat! (POLOMERKOLA_ZADNI)");
  polomer_zadni = (double)temp_kola_f;

  if (!NactiInt(buffer, "POHON_PREDNI", &temp_kola)) Error("Chybny car.dat! (POHON_PREDNI)");
  pohon_predni = (bool)temp_kola;

  if (!NactiInt(buffer, "POHON_ZADNI", &temp_kola)) Error("Chybny car.dat! (POHON_ZADNI)");
  pohon_zadni = (bool)temp_kola;

  if (!NactiInt(buffer, "HMOTNOSTKOLA_PREDNI", &temp_kola)) Error("Chybny car.dat! (HMOTNOSTKOLA_PREDNI)");
  hmotnost_kola_predni = (double)temp_kola;

  if (!NactiInt(buffer, "HMOTNOSTKOLA_ZADNI", &temp_kola)) Error("Chybny car.dat! (HMOTNOSTKOLA_ZADNI)");
  hmotnost_kola_zadni = (double)temp_kola;

  if (!NactiInt(buffer, "ANTIROLLBAR_PREDNI", &temp_kola)) Error("Chybny car.dat! (ANTIROLLBAR_PREDNI)");
  antirollbar_predni = (double)temp_kola;

  if (!NactiInt(buffer, "ANTIROLLBAR_ZADNI", &temp_kola)) Error("Chybny car.dat! (ANTIROLLBAR_ZADNI)");
  antirollbar_zadni = (double)temp_kola;

  if (!NactiFloat(buffer, "TLUMIC_DELKA_ZADNI", &temp_kola_f)) Error("Chybny car.dat! (TLUMIC_DELKA_ZADNI)");
  delka_zadni = (double)temp_kola_f;

  if (!NactiFloat(buffer, "TLUMIC_DELKA_PREDNI", &temp_kola_f)) Error("Chybny car.dat! (TLUMIC_DELKA_PREDNI)");
  delka_predni = (double)temp_kola_f;

  double Ay, By, Py, Ax, Bx, Px;
  if (!NactiFloat(buffer, "Ay", &Ay)) Error("Chybny car.dat! (Ay (PACEJKA))");
  if (!NactiFloat(buffer, "By", &By)) Error("Chybny car.dat! (By (PACEJKA))");
  if (!NactiFloat(buffer, "Py", &Py)) Error("Chybny car.dat! (Py (PACEJKA))");
  if (!NactiFloat(buffer, "Ax", &Ax)) Error("Chybny car.dat! (Ax (PACEJKA))");
  if (!NactiFloat(buffer, "Bx", &Bx)) Error("Chybny car.dat! (Bx (PACEJKA))");
  if (!NactiFloat(buffer, "Px", &Px)) Error("Chybny car.dat! (Px (PACEJKA))");

  pacejka = new Pacejka(Ax, Bx, Px, Ay, By, Py);

  char temp1[] = "POZICE ";
  for (int i = 0; i < 4; i++){
    temp1[6] = i + 48;

    //pozice
    if (!NactiFloat3(buffer, temp1, pozice_tlumicu[i])) Error("Chybny car.dat! (POZICEx)");
  }

  // obaly
  int pocet_kouli;
  if (!NactiInt(buffer, "POCETKOULI", &pocet_kouli)) Error("Chybny car.dat! (POCETKOULI)");

  char tempkoulepozice[] = "OBALKOULE  _POZICE";
  char tempkoulerodic[] = "OBALKOULE  _RODIC";
  char tempkoulepolomer[] = "OBALKOULE  _POLOMER";
  for (int i = 0; i < pocet_kouli; i++)
  {
    tempkoulepozice[9] = (i/10)+48;
    tempkoulepozice[10] = (i%10)+48;
    tempkoulerodic[9] = (i/10)+48;
    tempkoulerodic[10] = (i%10)+48;
    tempkoulepolomer[9] = (i/10)+48;
    tempkoulepolomer[10] = (i%10)+48;

    pozice3D pozice_koule;
    int rodic_koule;
    double polomer_koule;

    if (!NactiFloat3(buffer, tempkoulepozice, pozice_koule)) Error("Chybny car.dat! (OBALKOULE  _POZICE)");
    if (!NactiInt(buffer, tempkoulerodic, &rodic_koule)) Error("Chybny car.dat! (OBALKOULE  _RODIC");
    if (!NactiFloat(buffer, tempkoulepolomer, &polomer_koule)) Error("Chybny car.dat! (OBALKOULE  _POLOMER)");

    obal_koule obalkoule;
    obalkoule.ID = i;
    obalkoule.rodic = rodic_koule;
    obalkoule.polomer = polomer_koule;
    for (int n = 0; n < 3; n++)
      obalkoule.pozice[n] = pozice_koule[n];
    ObaloveKoule.hierarchie_obalu.push_back(obalkoule);
  }

}



char* getSoundPath(int car, int sound){
    char* cesta = new char[256];            // 256 znaku musi stacit vsem :-)
    ZjistiCestu(car, cesta);

    char* soubor;
    switch (sound){
        case ZVUK_STARTER:
            soubor="starter.wav";
        break;
        case ZVUK_MOTOR_IDLE:
            soubor="idle.wav";
        break;
        case ZVUK_MOTOR:
            soubor="motor.wav";
        break;
        case ZVUK_SMYK:
            soubor="skid.wav";
        break;

        case ZVUK_SMYK_ASFALT:
            soubor="roadskid.wav";
        break;
        case ZVUK_RAZENI:
            soubor="shift.wav";
        break;
        case ZVUK_KOLIZE:
            soubor="crash.wav";
        break;

        default:
            soubor=" ";
        break;
    }
    strcat(cesta, soubor);

    return cesta;
}




GLuint VytvorDisplayList(int typ, int soucastka, bool mat){
  char cesta[256];
  char *soubor;
  ZjistiCestu(typ, cesta);
  switch (soucastka){
    case MODEL_KAROSERIE:
      soubor = "karoserie.obj";
    break;
    case MODEL_KOLO:
      soubor = "kolo.obj";
    break;
    case MODEL_ZSVETLA:
      soubor = "zsvetla.obj";
    break;
    case MODEL_PSVETLA:
      soubor = "psvetla.obj";
    break;
    case MODEL_LSMER:
      soubor = "lsmer.obj";
    break;
    case MODEL_PSMER:
      soubor = "psmer.obj";
    break;
    case MODEL_ZPSVETLA:
      soubor = "zpsvetla.obj";
    break;
    case MODEL_BSVETLA:
      soubor = "bsvetla.obj";
    break;
  }
  glDisable(GL_COLOR_MATERIAL);
  glEnable(GL_TEXTURE_2D);


  strcat(cesta, soubor);


  GLMmodel* model = glmReadOBJ(cesta);

  glmFacetNormals(model);
  glmVertexNormals(model, 80.0);

  GLuint dl;

  if ((soucastka == MODEL_KAROSERIE) || (soucastka == MODEL_KOLO)){
    dl = glmList(model,  GLM_SMOOTH | GLM_MATERIAL | GLM_TEXTURE);
  } else dl = glmList(model,  GLM_SMOOTH);

  glmDelete(model);
  return dl;
}


GLuint CreateSphereDisplayList()
{
  GLuint sphere = glGenLists(1);
  glNewList(sphere, GL_COMPILE);
  GLUquadricObj* quadric;
  quadric = gluNewQuadric();
  gluQuadricDrawStyle(quadric, GLU_LINE);
  gluSphere(quadric, 1.0f, 16, 16);
  gluDeleteQuadric(quadric);
  glEndList();
  return sphere;
}


GLuint VytvorTexturu(int typ)
{
  char cesta[256];
  ZjistiCestu(typ, cesta);
  strcat(cesta, "textura.tga");
  GLuint texture = 0;
  glEnable(GL_TEXTURE_2D);
  glGenTextures(1, &texture);
  glBindTexture ( GL_TEXTURE_2D, texture);
  image_t temp_image;
  tgaLoad(cesta, &temp_image, TGA_FREE);
  return texture;
}


//  -===============================================-
//              * TERRAIN TEXTURES *
//  -===============================================-
TerrainTextures::TerrainTextures(){
    cout << "Loading surface textures..." << endl;

    glEnable(GL_TEXTURE_2D);
    image_t   temp_image;

    float maxAnisotropy;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);

    // ROAD
    road[TEXTURE_STRAIGHT] = 0;
    glGenTextures(1, &(road[TEXTURE_STRAIGHT]));
    glBindTexture ( GL_TEXTURE_2D, road[TEXTURE_STRAIGHT]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
    tgaLoad("data/textury/silnice/silnice.tga", &temp_image, TGA_FREE | TGA_ALPHA);

    road[TEXTURE_CURVE] = 0;
    glGenTextures(1, &(road[TEXTURE_CURVE]));
    glBindTexture ( GL_TEXTURE_2D, road[TEXTURE_CURVE]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
    tgaLoad("data/textury/silnice/zatacka.tga", &temp_image, TGA_FREE | TGA_ALPHA);

    road[TEXTURE_HALFCROSS] = 0;
    glGenTextures(1, &(road[TEXTURE_HALFCROSS]));
    glBindTexture ( GL_TEXTURE_2D, road[TEXTURE_HALFCROSS]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
    tgaLoad("data/textury/silnice/malakrizovatka.tga", &temp_image, TGA_FREE | TGA_ALPHA);

    road[TEXTURE_CROSS] = 0;
    glGenTextures(1, &(road[TEXTURE_CROSS]));
    glBindTexture ( GL_TEXTURE_2D, road[TEXTURE_CROSS]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
    tgaLoad("data/textury/silnice/krizovatka.tga", &temp_image, TGA_FREE | TGA_ALPHA);

    // SURFACE
    surface[TEXTURE_GRASS] = 0;
    glGenTextures(1, &(surface[TEXTURE_GRASS]));
    glBindTexture ( GL_TEXTURE_2D, surface[TEXTURE_GRASS]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
    tgaLoad("data/textury/povrch/trava.tga", &temp_image, TGA_FREE);

    surface[TEXTURE_SNOW] = 0;
    glGenTextures(1, &(surface[TEXTURE_SNOW]));
    glBindTexture ( GL_TEXTURE_2D, surface[TEXTURE_SNOW]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
    tgaLoad("data/textury/povrch/snih.tga", &temp_image, TGA_FREE);

    surface[TEXTURE_SNOW25] = 0;
    glGenTextures(1, &(surface[TEXTURE_SNOW25]));
    glBindTexture ( GL_TEXTURE_2D, surface[TEXTURE_SNOW25]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
    tgaLoad("data/textury/povrch/snih25.tga", &temp_image, TGA_FREE);

    surface[TEXTURE_SNOW50] = 0;
    glGenTextures(1, &(surface[TEXTURE_SNOW50]));
    glBindTexture ( GL_TEXTURE_2D, surface[TEXTURE_SNOW50]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
    tgaLoad("data/textury/povrch/snih50.tga", &temp_image, TGA_FREE);

    surface[TEXTURE_SNOW75] = 0;
    glGenTextures(1, &(surface[TEXTURE_SNOW75]));
    glBindTexture ( GL_TEXTURE_2D, surface[TEXTURE_SNOW75]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
    tgaLoad("data/textury/povrch/snih75.tga", &temp_image, TGA_FREE);

    surface[TEXTURE_MUD] = 0;
    glGenTextures(1, &(surface[TEXTURE_MUD]));
    glBindTexture ( GL_TEXTURE_2D, surface[TEXTURE_MUD]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
    tgaLoad("data/textury/povrch/hlina.tga", &temp_image, TGA_FREE);
}

TerrainTextures::~TerrainTextures()
{
  glDeleteTextures(1, &road[TEXTURE_STRAIGHT]);
  glDeleteTextures(1, &road[TEXTURE_CURVE]);
  glDeleteTextures(1, &road[TEXTURE_HALFCROSS]);
  glDeleteTextures(1, &road[TEXTURE_CROSS]);

  glDeleteTextures(1, &surface[TEXTURE_GRASS]);
  glDeleteTextures(1, &surface[TEXTURE_SNOW]);
  glDeleteTextures(1, &surface[TEXTURE_SNOW25]);
  glDeleteTextures(1, &surface[TEXTURE_SNOW50]);
  glDeleteTextures(1, &surface[TEXTURE_SNOW75]);
  glDeleteTextures(1, &surface[TEXTURE_MUD]);
}


void TerrainTextures::bindRoadTexture(road_texture rt){
    glBindTexture(GL_TEXTURE_2D, road[rt]);
}

void TerrainTextures::bindSurfaceTexture(surface_texture st){
    glBindTexture(GL_TEXTURE_2D, surface[st]);
}

GLuint TerrainTextures::getRoadTexture(road_texture rt){
    return road[rt];
}

GLuint TerrainTextures::getSurfaceTexture(surface_texture st){
    return surface[st];
}

void TerrainTextures::bindSurfaceNormalTexture(surface_texture st){
    glBindTexture(GL_TEXTURE_2D, surface_normal[st]);
}


