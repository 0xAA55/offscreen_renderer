#version 330
precision highp float;

out vec4 Color;

float pi = 3.1415926535897932384626433832795;
float r2d2 = 0.70710678118654752440084436210485;

float UOffset = 0.0;

uniform vec2 Resolution;
uniform sampler2D xp;
uniform sampler2D yp;
uniform sampler2D zp;
uniform sampler2D xn;
uniform sampler2D yn;
uniform sampler2D zn;

vec4 AxisColor(sampler2D samp, vec3 Ray, mat3 Rotator)
{
	vec3 TV = Rotator * Ray;
	TV /= abs(TV.z);
	if (TV.x <= -1.0 ||
		TV.x >=  1.0 ||
		TV.y <= -1.0 ||
		TV.y >=  1.0 ||
		TV.z < 0.0) return vec4(0);
	vec2 UV = (TV.xy + 1.0) * 0.5;
	return texture(samp, UV);
}

void main()
{
	vec2 UV = gl_FragCoord.xy / Resolution;
	vec2 UVO = fract(UV + vec2(1.0 - UOffset, 0.0));
	vec2 AUV = (UVO * vec2(2.0, 1.0) - vec2(0.0, 0.5)) * pi;
	float su, sv, cu, cv;
	su = sin(AUV.x);
	sv = sin(AUV.y);
	cu = cos(AUV.x);
	cv = cos(AUV.y);
	vec3 VecCube = vec3(su * cv, -sv, cu * cv);

	Color = vec4(0);
	Color = mix(AxisColor(zn, VecCube, mat3(vec3( 1,  0,  0), vec3( 0, -1,  0), vec3( 0,  0,  1))), Color, Color.w);
	Color = mix(AxisColor(xp, VecCube, mat3(vec3( 0,  0,  1), vec3( 0, -1,  0), vec3(-1,  0,  0))), Color, Color.w);
	Color = mix(AxisColor(zp, VecCube, mat3(vec3(-1,  0,  0), vec3( 0, -1,  0), vec3( 0,  0, -1))), Color, Color.w);
	Color = mix(AxisColor(xn, VecCube, mat3(vec3( 0,  0, -1), vec3( 0, -1,  0), vec3( 1,  0,  0))), Color, Color.w);
	Color = mix(AxisColor(yp, VecCube, mat3(vec3( 1,  0,  0), vec3( 0,  0,  1), vec3( 0,  1,  0))), Color, Color.w);
	Color = mix(AxisColor(yn, VecCube, mat3(vec3( 0, -1,  0), vec3( 0,  0, -1), vec3(-1,  0,  0))), Color, Color.w);
}
