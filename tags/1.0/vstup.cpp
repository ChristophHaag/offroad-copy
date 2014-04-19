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

#include "vstup.h"

keys::keys(bool p){
  for (int i = 0; i < 256; i++) key[i] = p;
  up = false;
  down = false;
  left = false;
  right = false;
  delta_x = 0;
  delta_y = 0;
  delta_uhelxy = 0.0;
  delta_uhelz = 0.0;

  smer = 1.0;
  citlivost = 0.001;

  pohyb = false;
  cameraReset = false;

  zoomIn = false;
  zoomOut = false;

  timePlus = false;
  timeMinus = false;
}


void keys::obsluha_down(int klavesa){
    switch (klavesa){
        case SDLK_UP:
            up = true;
            break;
        case SDLK_DOWN:
            down = true;
            break;
        case SDLK_LEFT:
            left = true;
            break;
        case SDLK_RIGHT:
            right = true;
            break;
        case SDLK_LSHIFT:
        case SDLK_RSHIFT:
            shift = true;
            break;
        case SDLK_KP5:
            cameraReset = true;
            break;

        case SDLK_KP_PLUS:
          zoomIn = true;
          break;

        case SDLK_KP_MINUS:
          zoomOut = true;
          break;

        case SDLK_KP_DIVIDE:
          timeMinus = true;
          break;
        case SDLK_KP_MULTIPLY:
          timePlus = true;
          break;

        default:
            if (klavesa < 256)
              key[klavesa] = true;
            break;
    }
}


void keys::obsluha_up(int klavesa){
    switch (klavesa){
        case SDLK_UP:
            up = false;
            break;
        case SDLK_DOWN:
            down = false;
            break;
        case SDLK_LEFT:
            left = false;
            break;
        case SDLK_RIGHT:
            right = false;
            break;
        case SDLK_LSHIFT:
        case SDLK_RSHIFT:
            shift = false;
            break;
        case SDLK_KP5:
            cameraReset = false;
            break;
        case SDLK_KP_PLUS:
          zoomIn = false;
          break;

        case SDLK_KP_MINUS:
          zoomOut = false;
          break;

        case SDLK_KP_DIVIDE:
          timeMinus = false;
          break;
        case SDLK_KP_MULTIPLY:
          timePlus = false;
          break;

        default:
            if (klavesa < 256)
              key[klavesa] = false;
            break;
    }
}

void keys::obsluha_mys_pohyb(int deltax, int deltay){
  delta_x = deltax;
  delta_y = deltay;
  pohyb = true;
}


void keys::obsluha_mys_down(int button){
    if (button == SDL_BUTTON_LEFT) left_button = true;
    if (button == SDL_BUTTON_RIGHT) right_button = true;
    if (button == SDL_BUTTON_MIDDLE) middle_button = true;
}

void keys::obsluha_mys_up(int button){
    if (button == SDL_BUTTON_LEFT) left_button = false;
    if (button == SDL_BUTTON_RIGHT) right_button = false;
    if (button == SDL_BUTTON_MIDDLE) middle_button = false;
}



void keys::cyklus(){
  if (!pohyb) {
    delta_x = 0;
    delta_y = 0;
  }
  pohyb = false;

  delta_uhelxy = delta_x * citlivost;
  delta_uhelz = delta_y * citlivost * smer;


}




