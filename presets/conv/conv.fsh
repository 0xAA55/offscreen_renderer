#version 330
precision highp float;

out vec4 Color;

uniform vec2 Resolution;
uniform sampler2D Kernel;
uniform sampler2D Texture;

void main()
{
	ivec2 FragCoord = ivec2(gl_FragCoord.xy);
	ivec2 KernelSize = textureSize(Kernel, 0);
	ivec2 KernelMid = KernelSize / 2;
	vec4 Value = vec4(0);

	for(int y = 0; y < KernelSize.y; y++)
	{
		for(int x = 0; x < KernelSize.x; x++)
		{
			ivec2 xy = ivec2(x, y);
			vec4 O = texelFetch(Texture, FragCoord + xy - KernelMid, 0);
			vec4 K = texelFetch(Kernel, xy, 0);
			Value += O * K;
		}
	}

	Color = Value / float(KernelSize.x * KernelSize.y);
}
