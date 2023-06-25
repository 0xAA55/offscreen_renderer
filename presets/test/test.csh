#version 430
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(rgba8, binding = 0) uniform image2D in_image;
layout(rgba8, binding = 1) uniform image2D out_image;

void main()
{
	ivec2 xy = ivec2(gl_WorkGroupID.xy);
	vec4 src = imageLoad(in_image, xy);
	imageStore(out_image, xy, vec4(src.x, 1.0 - src.y, 0.0, 1.0));
}
