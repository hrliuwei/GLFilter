#include "Particle.h"
#include <math.h>

double calcExp(int x, int u) {
	double det = 0.2;
	float fx = (float)x/50;
	double result = exp((-1.0)*(fx)*(fx) / (2 * det*det));
	double t = (sqrt(2 * 3.14))*det;
	result = result / t * 50;
	return result;
}

ParticleGenerator::ParticleGenerator(Shader shader, unsigned int texture_2D, GLuint amount)
	:m_shader(shader),
	m_Texture2D(texture_2D),
	m_amount(amount)
{
	init();
}


GLuint lastUnusedParticle = 0;
GLuint ParticleGenerator::firstUnusedParticle()
{
	for (GLuint i = lastUnusedParticle; i < m_amount; ++i)
	{
		if (m_Particle[i].Life <= 0.0f)
		{
			lastUnusedParticle = i;
			return i;
		}
	}

	for (GLuint i = 0; i < lastUnusedParticle; ++i)
	{
		if (m_Particle[i].Life <= 0.0f)
		{
			lastUnusedParticle = i;
			return i;
		}
	}
	lastUnusedParticle = 0;
	return 0;
}

void ParticleGenerator::respawPartcile(Particle& particle, glm::vec2 object, glm::vec2 offset)
{
	GLfloat randomx = (rand() % 200 - 100) / 1.0f;
	GLfloat randomy = calcExp(randomx, 0);
	GLint delta = randomy;
	GLfloat randomEy = rand()*1.0 / RAND_MAX * randomy;
	
	
	GLfloat rColor = (rand() % 100) / 100.f;
	GLfloat gColor = (rand() % 100) / 100.f;
	GLfloat bColor = (rand() % 100) / 100.f;
 	particle.Position = object  + glm::vec2(randomx, -randomEy) + glm::vec2(0.0f,-100.0f);
	particle.Color = glm::vec4(rColor, gColor, bColor, 1.0f);
	particle.Life = 0.6f;
	particle.Velocity = glm::vec2(0.0f,-5.0f);
}


void ParticleGenerator::Update(GLfloat dt, glm::vec2  object, GLuint newParticles, glm::vec2 offset)
{
	for (GLuint i = 0; i < newParticles; ++i)
	{
		int unusedParticle = firstUnusedParticle();
		respawPartcile(m_Particle[unusedParticle], object, offset);
	}
	for (int i = 0; i < m_amount; ++i)
	{
		Particle &p = m_Particle[i];
		p.Life -= dt*1.0f;
		if (p.Life > 0.0f)
		{
			p.Position += p.Velocity*dt*5.0f;
			p.Color.a -= dt * 2.5f;
			//p.Color.a = 0.0f;
		}
	}
}

void ParticleGenerator::Draw() {
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	m_shader.use();
	int nsize = 10;
	for (Particle particle:m_Particle)
	{
		if (particle.Life > 0.0f)
		{
// 			glm::mat4 model(1.0f);
// 			model = glm::translate(model, glm::vec3(particle.Position, 0.0f));
// 
// 			model = glm::translate(model, glm::vec3(0.5f*nsize, 0.5f*nsize, 0.0f));
// 			model = glm::rotate(model, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
// 			model = glm::translate(model, glm::vec3(-0.5f*nsize, -0.5f*nsize, 0.0f));
// 
// 			model = glm::scale(model, glm::vec3(glm::vec2(nsize, nsize), 1.0f));
// 
// 			m_shader.setMat4("model", model);
// 			m_shader.setVec3("spriteColor", glm::vec3(1.0,1.0f,0.0f));
			
			m_shader.setVec2("offset", particle.Position);
			m_shader.setVec4("color", particle.Color);
			glBindTexture(GL_TEXTURE_2D, m_Texture2D);
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
		}
	}
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC1_ALPHA);
}

void ParticleGenerator::init()
{
	GLuint VBO;
	GLfloat particle_quad[] = {
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
	glBindVertexArray(0);
	for (GLuint i = 0; i < m_amount; ++i)
	{
		m_Particle.push_back(Particle());
	}
}


