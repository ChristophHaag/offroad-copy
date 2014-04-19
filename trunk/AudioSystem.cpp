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


#include "AudioSystem.h"
#include "loaddata.h"
#include <iostream>
using namespace std;

#include <SDL/SDL.h>

const double IDLE_MIN = 0.085;
const double IDLE_DEFAULT = 0.17;
const double IDLE_MAX = 0.3;
const double IDLE_FADE_UP = 0.5;

const double ENGINE_MIN = 0.2;
const double ENGINE_DEFAULT = 0.8;
const double ENGINE_FADE_DOWN = 0.3;

const double SKID_DOWN_TIME = 2.0f;
const double SKID_UP_TIME = 2.0f;

const double IDLE_VOLUME = 0.5f;
const unsigned int COLLISION_TIME = 2000;  // how many ms is ignored next collision

AudioSystem::AudioSystem(objekty* objs){
    this->objs = objs;
    initialized = false;
    car = NULL;
    nosound = false;
    idle_play = false;
    engine_play = false;
    skid_play = false;
    skidVolume = 0.0f;
    roadskidVolume = 0.0f;
    roadskid_play = false;
    lastTime = 0;
    lastTimeCollision = 0;
    stupen = 0;
}

void AudioSystem::Cyklus(){
    if (nosound) return;

    if (objs->pesky || objs->aktivni == nic_aktivni)
    {
      if (car)
      {
        changeCar(NULL);
      }
    }
    else
    {
      if (objs->prostredek[objs->aktivni] != car)
        changeCar((vehikl*)objs->prostredek[objs->aktivni]);
    }


    if (car == NULL) return;

    unsigned long long int time = SDL_GetTicks();
    float deltaT = (time - lastTime) * 0.001f;
    lastTime = time;

    // KOLIZE
    if (car->IsCollision())
    {
      car->ClearCollision();

      if (time - lastTimeCollision >= COLLISION_TIME)
      {
        alSourcePlay(crash_source);
        lastTimeCollision = time;
      }
    }

    // STARTER
    if (car->getMotor()->starter_sound){
        car->getMotor()->starter_sound = false;
        alSourcePlay(engine_starter_source);
    }

    // RAZENI
    if (stupen != car->Stupen())
    {
      stupen = car->Stupen();
      if (stupen != 0)
        alSourcePlay(shift_source);
    }

    // OTACKY MOTORU
    if (car->getMotor()->stav == MOTOR_BEZI)
    {
        double otacky = car->getMotor()->w / car->getMotor()->MAX_W;
        // IDLE
        if ((otacky > IDLE_MIN) && (otacky < IDLE_MAX)){
            double hlasitost = 1.0;
            if (otacky > IDLE_FADE_UP){
                hlasitost = 1.0 - (otacky - IDLE_FADE_UP) / (IDLE_MAX - IDLE_FADE_UP);
            }

            alSourcef (engine_idle_source, AL_GAIN, hlasitost * IDLE_VOLUME);
            alSourcef(engine_idle_source, AL_PITCH, otacky / IDLE_DEFAULT);

            if (!idle_play) {
                alSourcePlay(engine_idle_source);
                idle_play = true;
            }
        } else {
            if (idle_play){
                alSourceStop(engine_idle_source);
                idle_play = false;
            }
        }

        // LOW
        if (otacky > ENGINE_MIN){
            double hlasitost = 1.0;
            if (otacky < ENGINE_FADE_DOWN){
                hlasitost = (otacky - ENGINE_MIN) / (ENGINE_FADE_DOWN - ENGINE_MIN);
            }

            alSourcef (engine_source, AL_GAIN, hlasitost);

            alSourcef(engine_source, AL_PITCH, otacky / ENGINE_DEFAULT);
            if (!engine_play) {
                alSourcePlay(engine_source);
                engine_play = true;
            }
        } else {
            if (engine_play){
                alSourceStop(engine_source);
                engine_play = false;
            }
        }

    }


    if (car->getMotor()->stav == MOTOR_VYP){
        if (idle_play){
            idle_play = false;
            alSourceStop(engine_idle_source);
        }
        if (engine_play){
            engine_play = false;
            alSourceStop(engine_source);
        }

    }

    // SMYK
    double slip = car->getSlipRatio();
    if (fabs(slip) > 0.001f)
    {
      skidVolume += deltaT * SKID_UP_TIME;
      if (skidVolume > 1.0f) skidVolume = 1.0f;
    }
    else
    {
      skidVolume -= deltaT * SKID_DOWN_TIME;
      if (skidVolume < 0.0f) skidVolume = 0.0f;
    }

    if (skidVolume > 0.001f && !skid_play)
    {
      skid_play = true;
      alSourcePlay(skid_source);
    }

    if (skidVolume < 0.001f && skid_play)
    {
      skid_play = false;
      alSourceStop(skid_source);
    }

    if (skid_play)
    {
      alSourcef (skid_source, AL_GAIN, skidVolume);
    }


    // SMYK -ROAD
    double slipRoad = car->getSlipRatioRoad();
    if (fabs(slipRoad) > 0.001f)
    {
      roadskidVolume += deltaT * SKID_UP_TIME;
      if (roadskidVolume > 1.0f) roadskidVolume = 1.0f;
    }
    else
    {
      roadskidVolume -= deltaT * SKID_DOWN_TIME;
      if (roadskidVolume < 0.0f) roadskidVolume = 0.0f;
    }

    if (roadskidVolume > 0.001f && !roadskid_play)
    {
      roadskid_play = true;
      alSourcePlay(roadskid_source);
    }

    if (roadskidVolume < 0.001f && roadskid_play)
    {
      roadskid_play = false;
      alSourceStop(roadskid_source);
    }

    if (roadskid_play)
    {
      alSourcef (roadskid_source, AL_GAIN, roadskidVolume);
    }


/*
        if (!skid_play){
            alSourcePlay(skid_source);
            skid_play = true;
        }
    } else {
        if (skid_play){
            alSourceStop(skid_source);
            skid_play = false;
        }

    }
    */
}

void AudioSystem::init(int argc, char *argv[]){
    alutInit(&argc, argv);

    int error = alGetError();
    if (error) {
        printf("%s\n", alutGetErrorString(error));
        nosound = true;
    }

    initialized = true;
    lastTime = SDL_GetTicks();

}

void AudioSystem::changeCar(vehikl* new_car){
    if (nosound) return;
    deleteSounds();
    car = new_car;
    if (car == NULL) return;

    lastTimeCollision = 0;
    car->ClearCollision();

    stupen = car->Stupen();

    char* path = getSoundPath(car->Typ(), ZVUK_STARTER);
    engine_starter_buffer = alutCreateBufferFromFile(path);
    alGenSources(1, &engine_starter_source);
    alSourcei(engine_starter_source, AL_BUFFER, engine_starter_buffer);
    delete path;

    path = getSoundPath(car->Typ(), ZVUK_MOTOR_IDLE);
    engine_idle_buffer = alutCreateBufferFromFile(path);
    alGenSources(1, &engine_idle_source);
    alSourcei(engine_idle_source, AL_BUFFER, engine_idle_buffer);
    alSourcei (engine_idle_source, AL_LOOPING,  true);
    delete path;

    path = getSoundPath(car->Typ(), ZVUK_MOTOR);
    engine_buffer = alutCreateBufferFromFile(path);
    alGenSources(1, &engine_source);
    alSourcei(engine_source, AL_BUFFER, engine_buffer);
    alSourcei (engine_source, AL_LOOPING,  true);
    delete path;

    path = getSoundPath(car->Typ(), ZVUK_SMYK);
    skid_buffer = alutCreateBufferFromFile(path);
    alGenSources(1, &skid_source);
    alSourcei(skid_source, AL_BUFFER, skid_buffer);
    alSourcei (skid_source, AL_LOOPING,  true);
    delete path;

    path = getSoundPath(car->Typ(), ZVUK_SMYK_ASFALT);
    roadskid_buffer = alutCreateBufferFromFile(path);
    alGenSources(1, &roadskid_source);
    alSourcei(roadskid_source, AL_BUFFER, roadskid_buffer);
    alSourcei (roadskid_source, AL_LOOPING,  true);
    delete path;

    path = getSoundPath(car->Typ(), ZVUK_RAZENI);
    shift_buffer = alutCreateBufferFromFile(path);
    alGenSources(1, &shift_source);
    alSourcei(shift_source, AL_BUFFER, shift_buffer);
    delete path;

    path = getSoundPath(car->Typ(), ZVUK_KOLIZE);
    crash_buffer = alutCreateBufferFromFile(path);
    alGenSources(1, &crash_source);
    alSourcei(crash_source, AL_BUFFER, crash_buffer);
    delete path;


    int error = alGetError();
    if (error) {
        printf("%s\n", alutGetErrorString(error));
        nosound = true;
    }
}


void AudioSystem::deleteSounds(){
    if (car != NULL){
        alDeleteSources(1, &engine_starter_source);
        alDeleteBuffers(1, &engine_starter_buffer);
        alDeleteSources(1, &engine_idle_source);
        alDeleteBuffers(1, &engine_idle_buffer);
        alDeleteSources(1, &engine_source);
        alDeleteBuffers(1, &engine_buffer);
        alDeleteSources(1, &skid_source);
        alDeleteBuffers(1, &skid_buffer);
        alDeleteSources(1, &roadskid_source);
        alDeleteBuffers(1, &roadskid_buffer);
        alDeleteSources(1, &shift_source);
        alDeleteBuffers(1, &shift_buffer);
        alDeleteSources(1, &crash_source);
        alDeleteBuffers(1, &crash_buffer);

    }
}



AudioSystem::~AudioSystem(){
    deleteSounds();

    ALCcontext* context = alcGetCurrentContext();
    ALCdevice* device = alcGetContextsDevice(context);

    alcDestroyContext(context);
    alcCloseDevice(device);
    alutExit();
 }
