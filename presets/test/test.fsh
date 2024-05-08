#version 330
precision highp float;

out vec4 Color;

uniform vec2 Resolution;

void main()
{
	vec2 uv = gl_FragCoord.xy / Resolution;
	Color = vec4(vec3(max(min(10.0 * (0.1 - abs(distance(uv, vec2(0.5)) * 2.0 - 0.75)), 1.0), 0.0)), 1.0);
}
