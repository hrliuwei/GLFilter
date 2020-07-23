#pragma once

#include <glm/glm.hpp>
#include "shader.h"
#include <vector>
struct Particle
{
	glm::vec2 Position, Velocity;
	glm::vec4 Color;
	GLfloat Life;
	Particle()
		:Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) {}
};


class ParticleGenerator
{
public:
	ParticleGenerator(Shader shader, unsigned int texture_2D, GLuint amount);
	void Update(GLfloat dt, glm::vec2 object, GLuint newParticles, glm::vec2 offset = glm::vec2(0.0f,0.0f));
	void Draw();
private:
	GLuint  m_amount;
	std::vector<Particle> m_Particle;
	unsigned int m_Texture2D;
	Shader m_shader;
	GLuint VAO;
	void init();
	GLuint firstUnusedParticle();
	void respawPartcile(Particle &particle, glm::vec2 object, glm::vec2 offset = glm::vec2(0.0f,0.0f));
};

