#version 330
precision highp float;

out vec4 Color;

uniform vec2 Resolution;
uniform sampler2D Kernel;
uniform sampler2D Texture;

void main()
{
	vec2 uv = gl_FragCoord.xy / Resolution;

	Color = mix(texture(Texture, uv), texture(Kernel, uv), 0.5);
}
