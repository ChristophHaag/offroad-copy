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

uniform samplerCube cubeMap;
uniform sampler2D texture;

void main(void)
{
	vec3 L = normalize(light - position);
	vec3 N = normalize(normal);
	vec3 C = normalize(- position);
	vec3 R = - reflect(C, N);
	vec4 Rtr = normalize(m * vec4(R, 1.0));

	float nxDir = max(0.0, dot(N, L));

  vec4 textureColor = texture2D(texture, textureCoord);

	vec4 diffuse_color = textureColor * gl_LightSource[1].diffuse * gl_FrontMaterial.diffuse * nxDir;

  vec4 ambient_color = textureColor * gl_LightSource[1].ambient * gl_FrontMaterial.ambient + gl_LightSource[1].ambient * textureCube(cubeMap, Rtr.xyz) * gl_FrontMaterial.specular * 0.2;

	float specular = max(0.0, dot(R, L));
	specular = pow(specular, gl_FrontMaterial.shininess);
	vec4 specular_color = (gl_LightSource[1].specular * gl_FrontMaterial.specular) * specular;

	gl_FragColor = diffuse_color + ambient_color + specular_color + gl_FrontMaterial.emission;
}
