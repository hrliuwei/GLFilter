#version 330 core
in vec2 Texcoords;
out vec4 color;

uniform sampler2D image;
uniform vec3 spriteColor;
uniform float ud;
//float ud = 80.0f;
vec2 back = vec2(800.0f,600.0f);
vec2 masac = vec2(10,10);
void main()
{
	vec2 st = Texcoords.st;
	float R = 0.5;
	vec2 pos = st- vec2(0.5,0.5);
	float lsize = length(pos);
	float beta = atan(pos.y,pos.x) + radians(ud)*(1-(lsize/R)*(lsize/R))*2;
	
	if(lsize < 0.5)
	{
		st = vec2(0.5,0.5) + vec2(lsize*cos(beta),lsize*sin(beta));
	}
	float kenrel[9] = float[](
		1,2,1,
		2,4,2,
		1,2,1
	);
	for(int i = 0; i < 9; ++i)
	{
		kenrel[i] = kenrel[i]/16;
	}
	float offset = 1.0f / 300.0f;
	vec2 offsets[9] = vec2[](
    vec2(-offset,  offset ),  // 左上
    vec2(  0.0f,    offset ),  // 中上
    vec2(  offset,  offset ),  // 右上
    vec2( -offset,  0.0f   ),  // 左中
    vec2(  0.0f,    0.0f   ),  // 正中
    vec2(  offset,  0.0f   ),  // 右中
    vec2( -offset, -offset ),  // 左下
    vec2(  0.0f,   -offset ),  // 中下
    vec2(  offset, -offset )   // 右下
	);
	vec3 sampleT[9];
	for(int i = 0; i < 9; i++)
	{
		sampleT[i] = vec3(texture(image,st.xy + offsets[i]));
	}
	vec3 colorT = vec3(0.0f);
	for(int i = 0; i < 9; ++i)
	{
		colorT += sampleT[i]*kenrel[i];
	}
	//color = vec4(spriteColor,1.0)*texture(image, Texcoords);
	//color = vec4(colorT,1.0);
	//color = color.bgra;
	if(Texcoords.s > 0.3 && Texcoords.s < 0.55 && Texcoords.t > 0.2 && Texcoords.t < 0.5)
	{
		vec2 back_ori = vec2(Texcoords.s * back.x, Texcoords.t * back.y);
		vec2 masac_ori = vec2(floor(back_ori.x/masac.x)*masac.x, floor(back_ori.y/masac.y)*masac.y);
		vec2 masac_new = vec2(masac_ori.x/back.x, masac_ori.y/back.y);	
		color = vec4(vec3(texture(image, masac_new)),1.0);
	}
	else{
		color = vec4(vec3(texture(image, Texcoords)),1.0);
	}
}