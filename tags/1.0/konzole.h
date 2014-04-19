//*************** KONZOLE *****************
//* funkce pro vystup textu na obrazovku	*
//*			multiplatformnni implementace			*
//* 					Milan Timko 2007						*
//*					milantimko@seznam.cz					*
//*****************************************


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

#ifndef _KONZOLE_
#define _KONZOLE_



void Vypis(char *text);

							// vypise text na obrazovku a odradkuje
void VypisStatusBar(char *nazev, int stav, int delka);	  // zobrazi primitivni statusbar zadane delky a stavu v %
void Chyba(char *text);																		// vypise text na obrazovku, odradkuje a ukonci aplikaci
void Cls();																								// vymaze obrazovku (pozor! vyrazne zpomaluje program)


#endif
