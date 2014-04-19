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
 * File:   glsl.h
 * Author: Milan Timko
 * Email: milantimko@seznam.cz
 * Created on 26. červenec 2009, 18:12
 */

#ifndef _GLSL_H
#define	_GLSL_H

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

class glsl{
private:
    char* loadFile(const char *path);
    GLuint vertex_shader;
    GLuint fragment_shader;
    GLuint program;


    bool init;      // jestli byl shader uspesne vytvoren
public:
    glsl();
    ~glsl();
    void release();
    void create(const char* filename);
    void bind();
    void unbind();
    GLuint getID(){return program;}

};


#endif	/* _GLSL_H */

