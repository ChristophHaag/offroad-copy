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
 * File:   AudioSystem.h
 * Author: Milan Timko
 *
 * Created on 16. říjen 2009, 17:19
 */

#ifndef _AUDIOSYSTEM_H
#define	_AUDIOSYSTEM_H


#include <AL/al.h>
#include <AL/alut.h>

#include "auto.h"
#include "objekty.h"

class AudioSystem{
    objekty* objs;
    bool initialized;
    bool idle_play;
    bool engine_play;
    bool skid_play;
    bool roadskid_play;

    vehikl* car;
    bool nosound;

    float skidVolume;

    float roadskidVolume;

    int stupen;

    ALuint engine_starter_buffer;
    ALuint engine_starter_source;

    ALuint engine_idle_buffer;
    ALuint engine_idle_source;

    ALuint engine_buffer;
    ALuint engine_source;

    ALuint skid_buffer;
    ALuint skid_source;

    ALuint roadskid_buffer;
    ALuint roadskid_source;

    ALuint shift_buffer;
    ALuint shift_source;

    ALuint crash_buffer;
    ALuint crash_source;

    unsigned long long int lastTime;
    unsigned long long int lastTimeCollision;

    void deleteSounds();
    void changeCar(vehikl* new_car);

public:
    void init(int argc, char *argv[]);

    AudioSystem(objekty* objlist);
    ~AudioSystem();

    void Cyklus();

};



#endif	/* _AUDIOSYSTEM_H */

