#version 330 core
in vec2 Texcoords;
out vec4 color;

uniform sampler2D image;
uniform vec3 spriteColor;
uniform float ud;
//float ud = 80.0f;
vec2 back = vec2(800.0f,600.0f);
vec2 position = vec2(370,350);
vec2 target = vec2(385,284);
vec2 masac = vec2(10,10);
float R = 18.0;



vec2 changeTex(vec2 positonNow, float len)
{
	float xishu = (1-(len/R)*(len/R))*1.0;
	vec2 delta = positonNow - target;
	float aw = atan(delta.y,delta.x);
	aw += -0.20;
	vec2 result = vec2(target.x+length(delta)*cos(aw),target.y + length(delta)*sin(aw));
	return result;
}

vec2 GetTexCoords(vec2 positonNow, float len)
{
	positonNow = changeTex(positonNow, len);
	vec2 delta = target - positonNow;	
	float xishu = (1-(len/R)*(len/R))*ud;
	delta = xishu*delta;
	vec2 result = positonNow + delta;
	return result;
}

void main()
{
	vec2 positionTex = vec2(Texcoords.s,Texcoords.t);
	vec2 positionTex_ori = vec2(positionTex.s*back.x,positionTex.t*back.y);
	vec2 delta = positionTex_ori - target;
	float len = length(delta);
	if(len < R)
	{
		positionTex_ori = GetTexCoords(positionTex_ori, len);
		positionTex = vec2(positionTex_ori.s/back.x, positionTex_ori.t/back.y);
		color = vec4(vec3(texture(image, positionTex))*vec3(1.0,1.0,1.0),1.0);
	}
	else{
		positionTex = vec2(positionTex_ori.s/back.x, positionTex_ori.t/back.y);
		color = vec4(vec3(texture(image, positionTex)),1.0);
	}
	
	//color = vec4(vec3(texture(image, Texcoords)),1.0);
	
}