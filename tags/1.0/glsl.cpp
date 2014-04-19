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

#include "glsl.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
using namespace std;


glsl::glsl(){
    init = false;
}

glsl::~glsl()
{
}

void glsl::release()
{

  typedef void (APIENTRY * glDeleteShaderFunc)(GLuint shader);
  glDeleteShaderFunc glDeleteShader = NULL;
  glDeleteShader = (glDeleteShaderFunc) SDL_GL_GetProcAddress("glDeleteShader");
  if (!glDeleteShader)
  {
    cout << "glDeleteShader not available! " << endl;
  }

  typedef void (APIENTRY * glDeleteProgramFunc)(GLuint p);
  glDeleteProgramFunc glDeleteProgram = NULL;
  glDeleteProgram = (glDeleteProgramFunc) SDL_GL_GetProcAddress("glDeleteProgram");
  if (!glDeleteProgram)
  {
    cout << "glDeleteProgram not available! " << endl;
  }

  glDeleteShader(fragment_shader);
  glDeleteShader(vertex_shader);
  glDeleteProgram(program);

  init = false;
}

char *glsl::loadFile(const char *path)
{
	FILE *fd;
	long len, r;
	char *str;

	if (!(fd = fopen(path, "r")))
	{
            fprintf(stderr, "Can't open file '%s' for reading\n", path);
            return NULL;
	}

	fseek(fd, 0, SEEK_END);
	len = ftell(fd);

	fseek(fd, 0, SEEK_SET);

        str = new char[len];

	r = fread(str, sizeof(char), len, fd);

	str[r - 1] = '\0'; /* Shader sources have to term with null */

	fclose(fd);

	return str;
}





void glsl::create(const char* filename){
    string vertex_file = string(filename);
    string fragment_file = string(filename);

    vertex_file.append(".vert");
    fragment_file.append(".frag");

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
    typedef void (APIENTRY * glGetShaderivFunc)(GLuint shader, GLenum pname, GLint* params);
    glGetShaderivFunc glGetShaderiv = NULL;
    glGetShaderiv = (glGetShaderivFunc) SDL_GL_GetProcAddress("glGetShaderiv");
    if (!glGetShaderiv)
    {
      cout << "glGetShaderiv not available! " << endl;
      exit(1);
    }
    typedef void (APIENTRY * glGetInfoLogARBFunc)(GLhandleARB object, GLsizei maxLength, GLsizei *length, GLcharARB *infoLog);
    glGetInfoLogARBFunc glGetInfoLogARB = NULL;
    glGetInfoLogARB = (glGetInfoLogARBFunc) SDL_GL_GetProcAddress("glGetInfoLogARB");
    if (!glGetInfoLogARB)
    {
      cout << "glGetInfoLogARB not available! " << endl;
      exit(1);
    }

    const GLchar* vertex_shader_code = loadFile(vertex_file.c_str());
    const GLchar* fragment_shader_code = loadFile(fragment_file.c_str());

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_code, NULL);
    glCompileShader(vertex_shader);


    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_code, NULL);
    glCompileShader(fragment_shader);

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);



    delete vertex_shader_code;
    delete fragment_shader_code;


    init = true;
}


void glsl::bind(){
    if (!init) return;
    typedef void (APIENTRY * glUseProgramFunc)(GLuint shader);
    glUseProgramFunc glUseProgram = NULL;
    glUseProgram = (glUseProgramFunc) SDL_GL_GetProcAddress("glUseProgram");
    if (!glUseProgram)
    {
      cout << "glUseProgram not available! " << endl;
      exit(1);
    }
    glUseProgram(program);
}

void glsl::unbind(){
    typedef void (APIENTRY * glUseProgramFunc)(GLuint shader);
    glUseProgramFunc glUseProgram = NULL;
    glUseProgram = (glUseProgramFunc) SDL_GL_GetProcAddress("glUseProgram");
    if (!glUseProgram)
    {
      cout << "glUseProgram not available! " << endl;
      exit(1);
    }

    glUseProgram(0);
}
