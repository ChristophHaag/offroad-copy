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


#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>


#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>


#include <iostream>
using namespace std;
#include "hud.h"
#include "matika.h"
#include "teren.h"
#include "vstup.h"
#include "offroad.h"
#include "kamera.h"
#include "tgaload.h"
#include "objekty.h"
#include "mrizka.h"
#include "konzole.h"
#include "AudioSystem.h"
#include "frustum.h"


GLuint textury_stromu[6];
GLuint textury_hudu[4];

bool loop = true;
SDL_TimerID main_timer;
SDL_mutex* main_mutex;

bool VYPIS_TRVANI_VYPOCTU = false;
bool VYPIS_TRVANI_VYKRESLENI = false;

nastaveni nastaveni(false);
keys keys(false);
teren teren("data/map.td");
kamera kamera(&keys, &teren);
hud hud(false);
objekty objekty(0, &teren, &keys);
AudioSystem audioSystem(&objekty);
CMrizka mrizka(&teren, &objekty);
Frustum frustum;


//   ***************
//   *   Vypocty   *
//   ***************

void vypocty(){

  struct timeb time;
  ftime(&time);
  double cas_zacatek = 1000.0 * (double)(time.time) + (double)(time.millitm);

  if (keys.key[esc]) exit(0);

  SDL_LockMutex(main_mutex);

  if (!nastaveni.pauza) objekty.cyklus(kamera.Pozice());
  keys.cyklus();
  matice mat;
  objekty.Matice(mat);
  kamera.cyklus(objekty.Pozice_ridice(), mat, objekty.Zataceni(), objekty.Cam());
  mrizka.Kolize();
  hud.cyklus(objekty.Spd(), objekty.Ot(), objekty.St(), objekty.Plyn(), objekty.Alt(), objekty.Un(), objekty.Uz(), objekty.Deltaz(), 0.0f, !objekty.pesky);

  SDL_UnlockMutex(main_mutex);

  ftime(&time);
  double cas_konec = 1000.0 * (double)(time.time) + (double)(time.millitm);
  double rozdil = cas_konec - cas_zacatek;
  if (VYPIS_TRVANI_VYPOCTU) cout << "cas vypoctu: " << rozdil << endl;
}





//   ***************
//   *   DISPLAY   *
//   ***************


void display(void) {
  struct timeb time;
  ftime(&time);
  double cas_zacatek = 1000.0 * (double)(time.time) + (double)(time.millitm);

/*
  Uint32 flags = SDL_DOUBLEBUF | SDL_OPENGL;
  if (nastaveni.fullscreen) flags = flags | SDL_FULLSCREEN;
*/

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();

  SDL_LockMutex(main_mutex);
  Vec3D camPos = Vec3D(kamera.Pozice());

  frustum.Step(camPos, kamera.Orientation());
  kamera.PrepareForDrawing();
  objekty.PrepareForDrawing();
  teren.cyklus(keys.timePlus, keys.timeMinus, kamera.Pozice(), 0.0, frustum);
  audioSystem.Cyklus();
  SDL_UnlockMutex(main_mutex);

  kamera.kresli();
  objekty.kresli(camPos);

  teren.kresli();

  hud.kresli(textury_hudu);

  glFlush();
  SDL_GL_SwapBuffers();

  ftime(&time);
  double cas_konec = 1000.0 * (double)(time.time) + (double)(time.millitm);
  double rozdil = cas_konec - cas_zacatek;
  if (VYPIS_TRVANI_VYKRESLENI) cout << "cas vykresleni: " << rozdil << endl;
}




//   ***************
//   *     INIT    *
//   ***************


void init (void)
{
  glEnable(GL_DEPTH_TEST);
  glBlendFunc(GL_ONE,GL_ONE);
  glClearDepth(1.0);
  glDepthFunc(GL_SMOOTH);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
  glShadeModel(GL_SMOOTH);

  nactitextury();

  float amb[4] = {svetlo[0] / 2.0,svetlo[0] / 2.0,svetlo[0] / 2.0,0.5};

  glLightfv(GL_LIGHT1, GL_AMBIENT, amb);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, svetlo);
//  glLightfv(GL_LIGHT1, GL_SPECULAR, svetlo);

  glEnable(GL_LIGHT1);
  glEnable(GL_LIGHTING);
  //glEnable(GL_NORMALIZE);
  glCullFace(GL_BACK);

  glEnable(GL_CULL_FACE);
  glEnable (GL_TEXTURE_2D);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
  teren.InitStromy(textury_stromu);
  teren.init();
  SDL_ShowCursor(0);
}

void DeInit()
{
  objekty.DeInit();
  teren.deinit();
  smaztextury();
}

//   ***************
//   *   RESHAPE   *
//   ***************


void reshape(GLsizei w, GLsizei h) {
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  float aspectRatio = ( double ) w / (( double ) h + 1);
  gluPerspective ( FOV, aspectRatio, 0.1, Z_FAR );
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  frustum.DisplayChanged(Z_FAR, FOV, aspectRatio);
}




//   ***************
//   *   KEYBOARD  *
//   ***************




void vstup(){
    SDL_Event event;
    int key;
    while(SDL_PollEvent(&event)) {
        switch(event.type){
            case SDL_KEYDOWN:
                key = event.key.keysym.sym;
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    loop = false;
                    return;
                }
                keys.obsluha_down(key);
                if (key == 'f') nastaveni.setfullscreen();
                if (key == 'p') nastaveni.setpauza();
                if (key == 13) objekty.nastup();
                break;
            case SDL_KEYUP:
                key = event.key.keysym.sym;
                keys.obsluha_up(key);
                break;
            case SDL_MOUSEMOTION:
              if(event.motion.x != nastaveni.sirka >> 1
              || event.motion.y != nastaveni.vyska >> 1)
              {
                keys.obsluha_mys_pohyb(event.motion.xrel, event.motion.yrel);
                SDL_WarpMouse(nastaveni.sirka >> 1, nastaveni.vyska >> 1);
              }
                break;
            case SDL_MOUSEBUTTONDOWN:
                keys.obsluha_mys_down(event.button.button);
                break;

            case SDL_MOUSEBUTTONUP:
                keys.obsluha_mys_up(event.button.button);
                break;

            default:
                break;

        }

    }
}

void checkExtensions(){
  typedef GLint (APIENTRY * glGetUniformLocationARBFunc)(GLhandleARB program, const char *name);
  glGetUniformLocationARBFunc glGetUniformLocationARB = NULL;
  glGetUniformLocationARB = (glGetUniformLocationARBFunc) SDL_GL_GetProcAddress("glGetUniformLocationARB");
  if (!glGetUniformLocationARB)
  {
     cout << "glGetUniformLocationARB not available! " << endl;
     exit(1);
  }

  typedef void (APIENTRY * glActiveTextureFunc)(GLenum  	texture);
  glActiveTextureFunc glActiveTexture = NULL;
  glActiveTexture = (glActiveTextureFunc) SDL_GL_GetProcAddress("glActiveTexture");
  if (!glActiveTexture)
  {
     cout << "glActiveTexture not available! " << endl;
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
  typedef void (APIENTRY * glUniform1iFunc)(GLint location, GLint v0);
  glUniform1iFunc glUniform1i = NULL;
  glUniform1i = (glUniform1iFunc) SDL_GL_GetProcAddress("glUniform1i");
  if (!glUniform1i)
  {
     cout << "glUniform1i not available! " << endl;
     exit(1);
  }
    typedef GLuint (APIENTRY * glCreateShaderFunc)(GLenum shaderType);
    glCreateShaderFunc glCreateShader = NULL;
    glCreateShader = (glCreateShaderFunc) SDL_GL_GetProcAddress("glCreateShader");
    if (!glCreateShader)
    {
      cout << "glCreateShader not available! " << endl;
      exit(1);
    }
    typedef void (APIENTRY * glShaderSourceFunc)(	GLuint shader, GLsizei count, const GLchar **string, const GLint* length);
    glShaderSourceFunc glShaderSource = NULL;
    glShaderSource = (glShaderSourceFunc) SDL_GL_GetProcAddress("glShaderSource");
    if (!glShaderSource)
    {
      cout << "glShaderSource not available! " << endl;
      exit(1);
    }
    typedef void (APIENTRY * glCompileShaderFunc)(GLuint shader);
    glCompileShaderFunc glCompileShader = NULL;
    glCompileShader = (glCompileShaderFunc) SDL_GL_GetProcAddress("glCompileShader");
    if (!glCompileShader)
    {
      cout << "glCompileShader not available! " << endl;
      exit(1);
    }
    typedef void (APIENTRY * glAttachShaderFunc)(GLuint	program, GLuint shader);
    glAttachShaderFunc glAttachShader = NULL;
    glAttachShader = (glAttachShaderFunc) SDL_GL_GetProcAddress("glAttachShader");
    if (!glAttachShader)
    {
      cout << "glAttachShader not available! " << endl;
      exit(1);
    }
    typedef void (APIENTRY * glLinkProgramFunc)(GLuint shader);
    glLinkProgramFunc glLinkProgram = NULL;
    glLinkProgram = (glLinkProgramFunc) SDL_GL_GetProcAddress("glLinkProgram");
    if (!glLinkProgram)
    {
      cout << "glAttachShader not available! " << endl;
      exit(1);
    }
    typedef GLuint (APIENTRY * glCreateProgramFunc)();
    glCreateProgramFunc glCreateProgram = NULL;
    glCreateProgram = (glCreateProgramFunc) SDL_GL_GetProcAddress("glCreateProgram");
    if (!glCreateProgram)
    {
      cout << "glCreateProgram not available! " << endl;
      exit(1);
    }

}


//   ***************
//   *     MAIN    *
//   ***************

Uint32 simulationCallback(Uint32 interval, void *param)
{
  vypocty();
  //audioSystem.Cyklus();
  return interval;
}


int main(int argc, char *argv[])
{
    int status;
    status = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    if (status == -1){
        cout << "Unable to initialize SDL: " << SDL_GetError() << endl;
        SDL_Quit();
        exit(1);
    }

    Uint32 flags = SDL_DOUBLEBUF | SDL_OPENGL;
    if (nastaveni.fullscreen) flags = flags | SDL_FULLSCREEN;

    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, 0);

    SDL_Surface *screen = SDL_SetVideoMode(nastaveni.sirka, nastaveni.vyska, 24, flags);

    if (screen == NULL){
        cout << "Unable to initialize OpenGL window: " << SDL_GetError() << endl;
        SDL_Quit();
        exit(1);
    }

    SDL_WM_SetCaption("Offroad: 2008 Edition v1.0 - Milan Timko Copyright (C) 2014", NULL);
    reshape(nastaveni.sirka, nastaveni.vyska);

    checkExtensions();
    main_mutex = SDL_CreateMutex();

    init();
    audioSystem.init(argc, argv);

    vypocty(); // must be called in this thread for first time to generate OpenGL stuff

    main_timer = SDL_AddTimer((unsigned int)(KROK * 1000.0f * 0.9f), simulationCallback, NULL);
    if (!main_timer)
    {
      cout << "Unable to initialize main timer: " << SDL_GetError() << endl;
      SDL_Quit();
      return 1;
    }

    while (loop)
    {
      display();
      vstup();
    }

    SDL_RemoveTimer(main_timer);
    SDL_DestroyMutex(main_mutex);

    DeInit();

    SDL_Quit();
    return 0;
}



//------------------------------------------------------------------------------
//                               MOJE FUNKCE
//------------------------------------------------------------------------------

char HLASKA_TEXTURY_STROMY[] = "Nacitam textury stromu";
char HLASKA_TEXTURY_TEREN[] = "Nacitam textury terenu";
char HLASKA_TEXTURY_HUD[] = "Nacitam textury HUDu";
char HLASKA_TEXTURY_SILNICE[] = "Nacitam textury silnice";
const int STATUSBAR_DELKA = 20;

GLuint auta[50];
void nactitextury(){
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

  glGenTextures (6, textury_stromu);
  glGenTextures (4, textury_hudu);
  image_t   temp_image;


	//---------------------------------------------------
	//							TEXTURY HUDu
	//---------------------------------------------------

  VypisStatusBar(HLASKA_TEXTURY_HUD, 0, STATUSBAR_DELKA);
  glBindTexture ( GL_TEXTURE_2D, textury_hudu[0]);
  tgaLoad  ( "data/textury/hud/pismo.tga", &temp_image, TGA_FREE | TGA_DEFAULT);

  VypisStatusBar(HLASKA_TEXTURY_HUD, 25, STATUSBAR_DELKA);
  glBindTexture ( GL_TEXTURE_2D, textury_hudu[1]);
  tgaLoad  ( "data/textury/hud/tachac.tga", &temp_image, TGA_FREE | TGA_DEFAULT);

  VypisStatusBar(HLASKA_TEXTURY_HUD, 50, STATUSBAR_DELKA);
  glBindTexture ( GL_TEXTURE_2D, textury_hudu[2]);
  tgaLoad  ( "data/textury/hud/stoupani.tga", &temp_image, TGA_FREE | TGA_DEFAULT);

  VypisStatusBar(HLASKA_TEXTURY_HUD, 75, STATUSBAR_DELKA);
  glBindTexture ( GL_TEXTURE_2D, textury_hudu[3]);
  tgaLoad  ( "data/textury/hud/sklon.tga", &temp_image, TGA_FREE | TGA_DEFAULT);
  VypisStatusBar(HLASKA_TEXTURY_HUD, 100, STATUSBAR_DELKA);




	//---------------------------------------------------
	//							TEXTURY STROMU
	//---------------------------------------------------

  VypisStatusBar(HLASKA_TEXTURY_STROMY, 0, STATUSBAR_DELKA);
  glBindTexture ( GL_TEXTURE_2D, textury_stromu[0]);
  tgaLoad  ( "data/textury/stromy/strom0.tga", &temp_image, TGA_FREE | TGA_ALPHA);

  VypisStatusBar(HLASKA_TEXTURY_STROMY, 15, STATUSBAR_DELKA);
  glBindTexture ( GL_TEXTURE_2D, textury_stromu[1]);
  tgaLoad  ( "data/textury/stromy/strom1.tga", &temp_image, TGA_FREE | TGA_ALPHA);

  VypisStatusBar(HLASKA_TEXTURY_STROMY, 30, STATUSBAR_DELKA);
  glBindTexture ( GL_TEXTURE_2D, textury_stromu[2]);
  tgaLoad  ( "data/textury/stromy/strom2.tga", &temp_image, TGA_FREE | TGA_ALPHA);

  VypisStatusBar(HLASKA_TEXTURY_STROMY, 50, STATUSBAR_DELKA);
  glBindTexture ( GL_TEXTURE_2D, textury_stromu[3]);
  tgaLoad  ( "data/textury/stromy/strom3.tga", &temp_image, TGA_FREE | TGA_ALPHA);

  VypisStatusBar(HLASKA_TEXTURY_STROMY, 65, STATUSBAR_DELKA);
  glBindTexture ( GL_TEXTURE_2D, textury_stromu[4]);
  tgaLoad  ( "data/textury/stromy/strom4.tga", &temp_image, TGA_FREE | TGA_ALPHA);

  VypisStatusBar(HLASKA_TEXTURY_STROMY, 82, STATUSBAR_DELKA);
  glBindTexture ( GL_TEXTURE_2D, textury_stromu[5]);
  tgaLoad  ( "data/textury/stromy/strom5.tga", &temp_image, TGA_FREE | TGA_ALPHA);

  VypisStatusBar(HLASKA_TEXTURY_STROMY, 100, STATUSBAR_DELKA);
  cout << "Startuji simulaci... " << endl;
}


void smaztextury()
{
  glDeleteTextures (6, textury_stromu);
  glDeleteTextures (4, textury_hudu);
}


void nastaveni::setfullscreen(){
    if (fullscreen) fullscreen = false;
      else fullscreen = true;
}

void nastaveni::setpauza(){
    if (pauza) pauza = false;
      else pauza = true;
}

