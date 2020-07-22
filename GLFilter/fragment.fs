#version 330 core
in vec2 Texcoords;
out vec4 color;

uniform sampler2D image;
uniform vec3 spriteColor;

void main()
{
	float kenrel[9] = {
		1,2,1,
		2,4,2,
		1,2,1
	};
	for(int i = 0; i < 9; ++i)
	{
		kenrel[i] = kenrel[i]/16;
	}
	float offset = 1.0f / 300.0f;
	vec2 offsets[9] = {
    vec2(-offset,  offset ),  // 左上
    vec2(  0.0f,    offset ),  // 中上
    vec2(  offset,  offset ),  // 右上
    vec2( -offset,  0.0f   ),  // 左中
    vec2(  0.0f,    0.0f   ),  // 正中
    vec2(  offset,  0.0f   ),  // 右中
    vec2( -offset, -offset ),  // 左下
    vec2(  0.0f,   -offset ),  // 中下
    vec2(  offset, -offset )   // 右下
	};
	vec3 sampleT[9];
	for(int i = 0; i < 9; i++)
	{
		sampleT[i] = vec3(texture(image,Texcoords.xy + offsets[i]));
	}
	vec3 colorT = vec3(0.0f);
	for(int i = 0; i < 9; ++i)
	{
		colorT += sampleT[i]*kenrel[i];
	}
	//color = vec4(spriteColor,1.0)*texture(image, Texcoords);
	color = vec4(colorT,1.0);
	color = color.bgra;
	//color = texture(image, Texcoords);
	
}