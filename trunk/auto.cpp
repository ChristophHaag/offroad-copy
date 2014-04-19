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
#include "vstup.h"
#include "objekt.h"
#include "loaddata.h"
#include "auto.h"
#include "3Dmath/Mat3D.h"
#include "3Dmath/Vec3D.h"
#include <iostream>

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
using namespace std;

static bool env_textures_init = false;
GLuint env_textures[6];

typedef GLint (APIENTRY * glGetUniformLocationARBFunc)(GLhandleARB program, const char *name);
typedef void (APIENTRY * glUniform4fFunc)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
typedef void (APIENTRY * glUniform1iFunc)(GLint location, GLint v0);

#if _WIN32
typedef void (APIENTRY * glActiveTextureFunc)(GLenum texture);
#endif

glGetUniformLocationARBFunc glGetUniformLocationARB = NULL;
glUniform4fFunc glUniform4f = NULL;
glUniform1iFunc glUniform1i = NULL;

#if _WIN32
glActiveTextureFunc glActiveTexture = NULL;
#endif

int whiteTexture = -1;
int sphereDL = -1;

vehikl::~vehikl()
{
  SmazListy();
  delete kola[0]->DejPacejka();
  for (int i = 0; i < 4; i++)
  {
    delete tlumice[i];
    delete kola[i];
  }

  delete zataceni;
  delete motor;
  delete prevodovka;

  delete zadnisvetla;
  delete prednisvetla;
  delete levesmerovky;
  delete pravesmerovky;
  delete zpateckasvetlo;
  delete brzdovesvetlo;

  shader.release();
}

vehikl::vehikl(int _typ, pozice3D _pozice, float azimut, teren *ter):objekt(_pozice, azimut){
  typ = _typ;
  vehicle_data data(typ);
  vytvoritlist = true;
  useTexture = data.Textura();
  texture = 0;
  kresleni_karoserie = true;
  double vaha[8];
  for (int i = 0; i < 8; i++)
    vaha[i] = data.Hmotnost(i);
  Init(data.Sirka(), data.Delka(), data.Vyska(), vaha, ter);

  POZICE_RIDICE = Vec3D(data.PoziceRidice());
  POZICE_VYSTUPU = Vec3D(data.PoziceVystupu());

  for (int i = 0; i < 4; i++)
      kola_pozice_os[i] = Vec3D(data.TlumicPozice(i));

  VypocitejPozice();

  // zadni kola
  for (int i = 0; i < 2; i++){
      tlumice[i] = new suspension(data.TlumicTvrdostZadni(), data.TlumicTlumeniZadniOdraz(), data.TlumicTlumeniZadniNaraz(), data.TlumicDelkaZadni());
      kola[i] = new wheel(data.PolomerZadni(), data.HmotnostKolaZadni(), data.TlumicPohonZadni(), false, ter, tlumice[i], kola_pozice[i], data.dejPacejku());
  }
  // predni kola
  for (int i = 2; i < 4; i++){
      tlumice[i] = new suspension(data.TlumicTvrdostPredni(), data.TlumicTlumeniPredniOdraz(), data.TlumicTlumeniPredniNaraz(), data.TlumicDelkaPredni());
      kola[i] = new wheel(data.PolomerPredni(), data.HmotnostKolaPredni(), data.TlumicPohonPredni(), true, ter, tlumice[i], kola_pozice[i], data.dejPacejku());
  }

  antirollbar_zadni.init(data.AntirollbarZadni(), kola[0], kola[1]);
  antirollbar_predni.init(data.AntirollbarPredni(), kola[3], kola[2]);


  zataceni = new tvolant(data.MaxZat(), data.RychZat());

  motor = new cmotor(data.MaxOtacky(), data.Vykon());
  prevodovka = new cprevodovka(data.PocetStupnu() + 2, data.Pomery());


  InitObal(&(data.ObaloveKoule));

  barva3 oranzovav = {0.5, 0.2, 0.0};
  barva3 oranzovaz = {0.8, 0.2, 0.0};
  barva3 bilav = {0.4, 0.4, 0.4};
  barva3 bilaz = {1.0, 0.9, 0.3};
  barva3 cervenav = {0.3, 0.1, 0.0};
  barva3 cervenaz = {0.4, 0.0, 0.0};
  barva3 cervenabrzda = {1.0, 0.1, 0.0};
  barva3 zpateckav = {0.4, 0.4, 0.4};
  barva3 zpateckaz = {0.8, 0.8, 0.8};

  zadnisvetla = new tsvetlo(0, cervenav, cervenaz);
  prednisvetla = new tsvetlo(0, bilav, bilaz);
  levesmerovky = new tsvetlo(BLIKANI_INTERVAL, oranzovav, oranzovaz);
  pravesmerovky = new tsvetlo(BLIKANI_INTERVAL, oranzovav, oranzovaz);
  zpateckasvetlo = new tsvetlo(0, zpateckav, zpateckaz);
  brzdovesvetlo = new tsvetlo(0, cervenav, cervenabrzda);

  svetla = false;
  plyn = brzda = doleva = doprava = razeni_n = razeni_d = false;
  rucnibrzda = true;

  spojka = 0.0;

}


void vehikl::Sily(){
  for (int i = 0; i < 4; i++) {
      PridejSilu(kola_pozice[i], - R.getE3() * tlumice[i]->dejSilu());
      PridejSilu(kola[i]->dejKontaktPneu(), kola[i]->dejSiluPneu());
      PridejSilu(kola[i]->dejPozici(), kola[i]->dejSiluKola());
  }
}



//------------------------------------------------------------------------------
//                         ***   CYKLUS   ***
//------------------------------------------------------------------------------

void vehikl::Cyklus(keys *klavesy){
  if (vytvoritlist){
    VytvorListy();
    vytvoritlist = false;
  }

  if (klavesy != NULL) aktivni = true;
    else aktivni = false;

  if (aktivni)
  {
    Klavesy(klavesy);
  }
  else
  {
    brzda = true;
    prevodovka->stupen = 1;
    kolize = false;
    motor->stav = MOTOR_VYP;
  }

  zataceni->Cyklus(doleva, doprava);
  prevodovka->Cyklus(razeni_n, razeni_d);

  // HLAVNI CYKLUS
  // krok 0

  RK0();
  VypocitejPozice();
  motor->RK0(plyn, starter);
  bool zadnikolo = true;
  for (int i = 0; i < 4; i++){
    //tlumice[i]->RK0(zataceni->zatoceni, brzda|(zadnikolo&rucnibrzda));
      kola[i]->rk0(zataceni->zatoceni, brzda|(zadnikolo&rucnibrzda));
    if (i == 1) zadnikolo = false;
  }


  // krok 1
  VypocitejPozice();
/*
  if (plyn) lin_sila -= Vec3D(0.0, 0.0, 100000.0);
 */
/*
  if (doleva){
      Vec3D smer = R * Vec3D(0.0, 10000.0,0.0);
      Vec3D misto = R * Vec3D(1.5, 0.0, -0.5) + bod;
      PridejSilu(misto, smer);
  }

  if (doprava){
      Vec3D smer = R * Vec3D(0.0, -10000.0,0.0);
      Vec3D misto = R * Vec3D(1.5, 0.0, -0.5) + bod;
      PridejSilu(misto, smer);
  }


  if (plyn){
      Vec3D smer = R * Vec3D(20000.0,0.0, 0.0);
      Vec3D misto = R * Vec3D(-1.5, 0.0, -0.7) + bod;
      PridejSilu(misto, smer);
  }

  if (klavesy->down){
      Vec3D smer = R * Vec3D(-20000.0,0.0, 0.0);
      Vec3D misto = R * Vec3D(-1.5, 0.0, -0.7) + bod;
      PridejSilu(misto, smer);
  }
*/


  motor->RK1(spojka);
  SilaNaKola();
  antirollbar_predni.cyklus();
  antirollbar_zadni.cyklus();

  for (int i = 0; i < 4; i++) {
      kola[i]->rk1(kola_pozice[i], kola_rychlost[i], R, rot_sila_kolo[i]);
  }
  Sily();
  RK1();


  // krok 2
  VypocitejPozice();
  motor->RK2(spojka);
  SilaNaKola();

  antirollbar_predni.cyklus();
  antirollbar_zadni.cyklus();


  for (int i = 0; i < 4; i++) {
      kola[i]->rk2(kola_pozice[i], kola_rychlost[i], R, rot_sila_kolo[i]);

  }

  Sily();
  RK2();


  // krok 3

  VypocitejPozice();
  motor->RK3(spojka);
  SilaNaKola();

  antirollbar_predni.cyklus();
  antirollbar_zadni.cyklus();

  for (int i = 0; i < 4; i++) {
      kola[i]->rk3(kola_pozice[i], kola_rychlost[i], R, rot_sila_kolo[i]);
  }

  Sily();
  RK3();

  // krok 4
  VypocitejPozice();
  motor->RKFINAL(spojka);
  SilaNaKola();
  antirollbar_predni.cyklus();
  antirollbar_zadni.cyklus();

  for (int i = 0; i < 4; i++) {
      kola[i]->rkfinal(kola_pozice[i], kola_rychlost[i], R, rot_sila_kolo[i]);
  }


  Sily();
  RKFINAL();

/*
  motor->Cyklus(keyup, spojka);
  spojka = 0.0;
  double otacky_kol = 0.0;
  int pocet = 0;
  for (int n = 0; n < 4; n++)
    if (tlumice[n]->POHON_KOLA){
      pocet++;
      otacky_kol += tlumice[n]->otacky;
    }
  otacky_kol /= (double)pocet;
  if (prevodovka->stupen == 0) otacky_kol *= -1.0;
  spojka = 0.0;
  tah = 0.0;
  if (prevodovka->stupen != 1){
    double otacky = motor->otacky / fabs(prevodovka->Pomer());
    spojka = 0.1 * (otacky - otacky_kol);

    cout << "motor : " << otacky << endl;
    cout << "kola  : " << otacky_kol << endl;
    cout << "spojka: " << spojka << endl << endl;

    tah = 0.5 * (spojka / prevodovka->Pomer()) / CELKOVA_HMOTNOST;
  }
  prevodovka->Cyklus(keyshiftup, keyshiftdown);
*/

  // svetla
  bool zpatecka = false;
  if (prevodovka->stupen == 0) zpatecka = true;

  levesmerovky->Cyklus(doleva, R, bod);
  pravesmerovky->Cyklus(doprava, R, bod);
  zadnisvetla->Cyklus(svetla, R, bod);
  prednisvetla->Cyklus(svetla, R, bod);
  zpateckasvetlo->Cyklus(zpatecka, R, bod);
  brzdovesvetlo->Cyklus(brzda&aktivni, R, bod);
}


void vehikl::Kresli(const Vec3D& kamera){
  return;
}

void DrawObject::Draw(const Vec3D& cameraPos, const Vec3D& sunPos)
{
  glDisable(GL_CULL_FACE);
  glDisable(GL_COLOR_MATERIAL);
  glEnable(GL_TEXTURE_2D);

  if (useColor)
  {
    float c[4] = {color.getX(),color.getY(),color.getZ(), 1.0};
    glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT, c);
    glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, c);

    if (useEmissive)
      glMaterialfv (GL_FRONT_AND_BACK, GL_EMISSION, c);
  }

  shader.unbind();
  shader.bind();

  Mat3D inv_R = orient.invert();

  int location_light = glGetUniformLocationARB(shader.getID(), "light_position");
  int location_camera = glGetUniformLocationARB(shader.getID(), "camera_position");

  Vec3D sun = inv_R * (sunPos - pos);
  Vec3D cam = inv_R * (cameraPos - pos);

  glUniform4f(location_light,  (float) sun.getX(),   (float) sun.getY(),  (float) sun.getZ(), 1.0);
  glUniform4f(location_camera, (float) cam.getX(), (float)cam.getY(), (float)cam.getZ(), 1.0);

  glActiveTexture(GL_TEXTURE0);
  glMatrixMode(GL_TEXTURE);
  glPushMatrix();
  double inv_m[16];
  inv_R.transpose().convertToOpenGL(inv_m);
  glLoadMatrixd(inv_m);
  glMatrixMode(GL_MODELVIEW);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, texture >= 0 ? texture : whiteTexture);
  GLuint location_texture = glGetUniformLocationARB(shader.getID(), "texture");
  glUniform1i(location_texture, 1);

  glBindTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_X, env_textures[0]);
  glBindTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, env_textures[1]);
  glBindTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, env_textures[2]);
  glBindTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, env_textures[3]);
  glBindTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, env_textures[4]);
  glBindTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, env_textures[5]);
  glEnable(GL_TEXTURE_CUBE_MAP);

  GLuint location_cube = glGetUniformLocationARB(shader.getID(), "cubeMap");
  glUniform1i(location_cube, 0);

  glPushMatrix();
  glTranslatef(pos.getX(), pos.getY(), pos.getZ());
  double mat[16];
  orient.convertToOpenGL(mat);
  glMultMatrixd(mat);
  glCallList(dl);
  glPopMatrix();

  glMatrixMode(GL_TEXTURE);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);


  shader.unbind();

  if (useColor)
  {
    float black[4] = {0.0, 0.0, 0.0, 1.0};
    glMaterialfv (GL_FRONT_AND_BACK, GL_EMISSION, black);
    glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT, black);
    glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, black);
  }
  glActiveTexture(GL_TEXTURE0);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_TEXTURE_CUBE_MAP);

  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_TEXTURE_2D);
}


void vehikl::GetDrawObjects(std::vector<DrawObject>& objectList) const
{
  if (kresleni_karoserie)
    objekt::GetDrawObjects(objectList);

  for (int i = 0; i < 4; i++)
  {
    DrawObject obj;
    obj.dl = DL_kola;
    obj.pos = kola[i]->dejPozici();
    obj.shader = shader;

    Mat3D rot;
    if ((i == 1) || (i == 2))
    {
      rot.rotateZ(M_PI);
    }
    rot.rotateY(kola[i]->dejRotaci());
    obj.orient = rot * kola[i]->dejOrientaci();
    obj.useEmissive = false;
    obj.useColor = false;
    obj.texture = useTexture ? texture : -1;
    objectList.push_back(obj);
  }

  std::vector<tsvetlo*> lights;
  lights.push_back(levesmerovky);
  lights.push_back(pravesmerovky);
  lights.push_back(zadnisvetla);
  lights.push_back(prednisvetla);
  lights.push_back(zpateckasvetlo);
  lights.push_back(brzdovesvetlo);

  for (int i = 0; i < lights.size(); i++)
  {
    DrawObject obj;
    obj.dl = lights[i]->DL;
    obj.shader = shader;
    obj.orient = lights[i]->matice_rotace;
    obj.pos = lights[i]->pozice;
    obj.useEmissive = lights[i]->sviti;
    obj.useColor = true;
    obj.color = obj.useEmissive ? lights[i]->barva_zap : lights[i]->barva_vyp;
    obj.texture = -1;
    objectList.push_back(obj);
  }
}



//------------------------------------------------------------------------------
//                           ***   RESET   ***
//------------------------------------------------------------------------------

void vehikl::Reset(){
    w.clear();
    v.clear();
    Q.clear();
    R = Mat3D();
    inv_R = R.invert();
    lin_sila.clear();
    rot_sila.clear();

    prevodovka->stupen = 1;

    Vec3D prusecik, normal;
    t->Prusecik_n(x, &prusecik, &normal);
    prusecik += Vec3D(0.0, 0.0, 1.5);

    for (int i =0; i < 4; i++)
        kola[i]->reset();

    x = prusecik;
    VypocitejPozice();
}


//------------------------------------------------------------------------------
//                          ***   KLAVESY   ***
//------------------------------------------------------------------------------

void vehikl::Klavesy(keys *klavesy){
  plyn = false;
  brzda = false;
  doleva = false;
  doprava = false;
  //svetla = false;
  starter = false;
  rucnibrzda = false;
  razeni_n = false;
  razeni_d = false;
  static int waittime = 0;


  if (klavesy->down) brzda = true;
  if (klavesy->up) plyn = true;
  if (klavesy->left) doleva = true;
  if (klavesy->right) doprava = true;
  if (klavesy->key['r']) Reset();

  if (klavesy->key['a']) razeni_n = true;
  if (klavesy->key['z']) razeni_d = true;
  if (klavesy->key['y']) razeni_d = true;
  if (klavesy->key[32]) rucnibrzda = true;


  if (waittime > 0)
  {
    waittime--;
    return;
  }
  if (klavesy->key['o'])
  {
    kresleni_obalu++;
    waittime = 20;
  }

  if (klavesy->key['k'])
  {
    kresleni_karoserie = !kresleni_karoserie;
    waittime = 20;
  }

  if (klavesy->key['l'])
  {
    svetla = !svetla;
    waittime = 10;
  }

  if (klavesy->key['s'])
  {
    starter = true;
    waittime = 10;
  }


  if (kresleni_obalu > 2) kresleni_obalu = 0;


  if (klavesy->key['m']){
    Vec3D sila = R * Vec3D(0.0, 0.0, 50000.0);
    PridejSilu(body[0], sila);
    PridejSilu(body[3], sila);
  }
/*
  if (klavesy->key['l']){
    vektor3D sila = {10000.0, 0.0, 0.0};
    PridejSilu(body[0], sila);
    PridejSilu(body[1], sila);
    PridejSilu(body[4], sila);
    PridejSilu(body[5], sila);
  }
*/
}



//------------------------------------------------------------------------------
//                        ***   VYTVORLISTY   ***
//------------------------------------------------------------------------------

void vehikl::VytvorListy(){

    if (!glGetUniformLocationARB)
      glGetUniformLocationARB = (glGetUniformLocationARBFunc) SDL_GL_GetProcAddress("glGetUniformLocationARB");

    if (!glUniform4f)
      glUniform4f = (glUniform4fFunc) SDL_GL_GetProcAddress("glUniform4f");

    if (!glUniform1i)
      glUniform1i = (glUniform1iFunc) SDL_GL_GetProcAddress("glUniform1i");

#if _WIN32
    if (!glActiveTexture)
      glActiveTexture = (glActiveTextureFunc) SDL_GL_GetProcAddress("glActiveTexture");
#endif

    if (whiteTexture < 0)
    {
      unsigned char data[] = {255,255,255,255};
      GLuint tex = 0;

      glGenTextures(1, &tex);
      glBindTexture(GL_TEXTURE_2D, tex);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
      whiteTexture = tex;
    }

    if (sphereDL < 0)
    {
      sphereDL = CreateSphereDisplayList();
    }

    DL =                 VytvorDisplayList(typ, MODEL_KAROSERIE ,true);
    DL_kola =            VytvorDisplayList(typ, MODEL_KOLO, true);
    zadnisvetla->DL =    VytvorDisplayList(typ, MODEL_ZSVETLA, false);
    prednisvetla->DL =   VytvorDisplayList(typ, MODEL_PSVETLA, false);
    levesmerovky->DL =   VytvorDisplayList(typ, MODEL_LSMER, false);
    pravesmerovky->DL =  VytvorDisplayList(typ, MODEL_PSMER, false);
    zpateckasvetlo->DL = VytvorDisplayList(typ, MODEL_ZPSVETLA, false);
    brzdovesvetlo->DL =  VytvorDisplayList(typ, MODEL_BSVETLA, false);

    if (useTexture)
    {
      texture = VytvorTexturu(typ);
    }

    shader.create("data/shaders/car");

    if (!env_textures_init)
    {
      env_textures_init = true;
      EnvTextures(env_textures);
    }
}

void vehikl::SmazListy()
{
  if (whiteTexture != -1)
  {
    GLuint t = whiteTexture;
    glDeleteTextures(1, &t);
    whiteTexture = -1;
  }

  if (sphereDL != -1)
  {
    GLuint t = sphereDL;
    glDeleteLists(t, 1);
    sphereDL = -1;
  }

  glDeleteLists(DL, 1);
  glDeleteLists(DL_kola, 1);

  glDeleteLists(zadnisvetla->DL, 1);
  glDeleteLists(levesmerovky->DL, 1);
  glDeleteLists(pravesmerovky->DL, 1);
  glDeleteLists(brzdovesvetlo->DL, 1);
  glDeleteLists(zpateckasvetlo->DL, 1);
  glDeleteLists(prednisvetla->DL, 1);

  if (useTexture)
  {
    glDeleteTextures(1, &texture);
  }

  if (env_textures_init)
  {
    glDeleteTextures(6, env_textures);
    env_textures_init = false;
  }
}

//------------------------------------------------------------------------------
//                      ***   VYPOCITEJPOZICE   ***
//------------------------------------------------------------------------------

void vehikl::VypocitejPozice(){
    pozice_ridice = R * POZICE_RIDICE + bod;
    pozice_vystupu = R * POZICE_VYSTUPU + bod;

    pozice_ridice_stare[0] = pozice_ridice.getX();
    pozice_ridice_stare[1] = pozice_ridice.getY();
    pozice_ridice_stare[2] = pozice_ridice.getZ();

    pozice_vystupu_stare[0] = pozice_vystupu.getX();
    pozice_vystupu_stare[1] = pozice_vystupu.getY();
    pozice_vystupu_stare[2] = pozice_vystupu.getZ();


    for (int n = 0; n < 4; n++){
        kola_pozice[n] = bod + R * kola_pozice_os[n];
        kola_rychlost[n] = RychlostBodu(kola_pozice[n]);
    }
}



double vehikl::OtackyKol(){
  double max = 0.0;
  int ktere = 0;
  for (int i = 0; i < 4; i++){
    if (!kola[i]->pohon()) continue;
    if (fabs(kola[i]->dejW()) >= fabs(max)){
      max = kola[i]->dejW();
      ktere = i;
    }
  }
  pohon_kolo = ktere;
  return max;

 return 0.0;
}


const double SPOJKA_MAX = 2000.0;


void vehikl::SilaNaKola(){
  for (int i = 0; i < 4; i++)
    rot_sila_kolo[i] = 0.0;


  spojka = 0.0;
	if (brzda) return;


  double otacky_kol = prevodovka->Pomer() * OtackyKol();
  //if (prevodovka->stupen == 0) otacky_kol *= -1.0;

  double sila = 0.0;

  double otacky = motor->w;

  if (prevodovka->stupen != 1){
    spojka =  5.0 * (otacky - otacky_kol);
    if (spojka > SPOJKA_MAX) spojka = SPOJKA_MAX;
    if (spojka < -SPOJKA_MAX) spojka = -SPOJKA_MAX;
    sila = (spojka * prevodovka->Pomer());
    //if (prevodovka->stupen == 0) sila = -sila;
  }

  int count = 0;
  for (int i = 0; i < 4; i++){
      if (kola[i]->pohon()) count++;
  }
  if (count == 0) return;   // zadne kolo neni napojene na motor
  for (int i = 0; i < 4; i++){
        if (kola[i]->pohon()){
            rot_sila_kolo[i] = sila / (double)count;
        }
  }

}




double *vehikl::Pozice(){
    pozice_old[0] = bod.getX();
    pozice_old[1] = bod.getY();
    pozice_old[2] = bod.getZ();
    return pozice_old;
}

double *vehikl::Pozice_ridice(){
  return pozice_ridice_stare;
}

double *vehikl::Pozice_vystupu(){
  return pozice_vystupu_stare;
}

double vehikl::Spd(){
  double spd = v.length();
  return spd * 3.6; /// KROK;
}

double vehikl::Otacky(){
  return motor->w / motor->MAX_W;
}

int vehikl::Stupen(){
  return prevodovka->stupen - 1;
}


double vehikl::getSlipRatio() const {
    double slipRatio = 0.0;

    for (int i = 0; i < 4; i++){
        if (fabs(kola[i]->SlipRatio()) > fabs(slipRatio))
            slipRatio = kola[i]->SlipRatio();
    }
    return slipRatio;
}

double vehikl::getSlipRatioRoad() const
{
    double slipRatio = 0.0;

    for (int i = 0; i < 4; i++){
        if (fabs(kola[i]->SlipRatioRoad()) > fabs(slipRatio))
            slipRatio = kola[i]->SlipRatioRoad();
    }
    return slipRatio;
}
