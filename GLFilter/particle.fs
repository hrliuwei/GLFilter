#version 330 core
in vec2 TexCoords;

out vec4 Fragcolor;
in vec4 ParticleColor;

uniform sampler2D sprite;


void main()
{
    //Fragcolor =  vec4(vec3(texture(sprite, TexCoords)),ParticleColor.a);
	//Fragcolor = vec4(0.5f,0.3,0.1,ParticleColor.a);
	vec4 ss = vec4(0.5f,0.3f,0.1f,ParticleColor.a);
	Fragcolor = (texture(sprite, TexCoords))*vec4(1.0f,0.2f,0.2f,ParticleColor.a);
		       
}