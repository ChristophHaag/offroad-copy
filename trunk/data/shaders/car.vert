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

varying vec3 normal;
varying vec3 position;
varying vec3 light;
varying mat4 m;
varying vec2 textureCoord;

uniform vec4 light_position;


void main(void)
{
  textureCoord = vec2(gl_MultiTexCoord0);
	m = gl_TextureMatrix[0] * transpose(gl_ModelViewMatrix);
	normal = gl_NormalMatrix * gl_Normal;
	position = (gl_ModelViewMatrix * gl_Vertex).xyz;
	light = (gl_ModelViewMatrix * light_position).xyz;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

}
