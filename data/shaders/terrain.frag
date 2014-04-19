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

uniform sampler2D surface;
uniform sampler2D road;

uniform int use_road;
uniform int snow;

varying vec3 normal;
varying vec3 position;
varying vec3 light;

const float shininess = 5.0;
const float ambient = 0.2;
const float diffuse = 0.65;
const vec4 specular = vec4(0.4, 0.4, 0.4, 1.0);

void main(void)
{

	float road_ratio = 0.0;
	float surface_ratio = 1.0;

	vec4 texel_surface = texture2D(surface, gl_TexCoord[0].st);
	vec4 texel_road = texture2D(road, gl_TexCoord[1].st);

	if (use_road == 1){
		road_ratio = texel_road.a;
		surface_ratio = 1.0 - road_ratio;
	}

	vec4 color = texel_surface * surface_ratio + texel_road * road_ratio;

	vec3 N = normalize(normal);
	vec3 L = normalize(light);

	float nxDir = max(0.0, dot(N, L));
  	vec4 diffuse_color = gl_LightSource[1].diffuse * color * nxDir * diffuse;

	vec4 ambient_color = gl_LightSource[1].ambient * color * ambient;

	vec4 specular_color = vec4(0.0, 0.0, 0.0, 0.0);
        vec3 C = normalize(- position);
	vec3 R = - reflect(C, N);
        float specular_ratio = max(0.0, dot(R, L));

        if (road_ratio > 0.0){
		float specular_ratio_pow = pow(specular_ratio, shininess);
		specular_color = (gl_LightSource[1].specular * specular) * specular_ratio_pow * road_ratio;
	}

	gl_FragColor = diffuse_color + specular_color + ambient_color;
}


