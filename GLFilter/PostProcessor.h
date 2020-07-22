#pragma once
#include "shader.h"
class CPostProcessor
{
public:
	Shader m_PostProcessingShader;
	unsigned int m_texture2D;
	GLuint Width, Height;
	GLboolean Confuse, Chaos, Shake;
	CPostProcessor(Shader shader, GLuint width, GLuint height);

	void BeginRender();
	void EndRneder();
	void Render(GLfloat time);
private:
	GLuint MSFBO, FBO;
	GLuint RBO;
	GLuint VAO;
	GLuint rbo;
	void InitRenderData();
};

