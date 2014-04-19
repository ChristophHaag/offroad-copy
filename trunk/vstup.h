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

#ifndef _VSTUP_
#define _VSTUP_


struct keys{

  bool up;
  bool down;
  bool left;
  bool right;
  bool shift;

  bool cameraReset;

  int key[255];

  keys(bool p);

  // mys
  double smer;          // kvuli invertovani mysi
  double citlivost;
  int delta_x;          // zmena pozice mysi
  int delta_y;
  bool pohyb;           //  jestli doslo k pohybu mysi od posledniho cyklu
  double delta_uhelxy;
  double delta_uhelz;
  bool left_button;
  bool right_button;
  bool middle_button;
  bool zoomIn;
  bool zoomOut;
  bool timePlus;
  bool timeMinus;

  void obsluha_up(int klavesa);
  void obsluha_down(int klavesa);
  void obsluha_mys_pohyb(int deltax, int deltay);
  void obsluha_mys(int button, int state);

  void obsluha_mys_down(int button);
  void obsluha_mys_up(int button);

  void cyklus();
};



#endif
