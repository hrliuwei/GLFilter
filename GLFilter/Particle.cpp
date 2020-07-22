#include "Particle.h"

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
	GLfloat random = (rand() % 200 - 100) / 1.0f;
	GLfloat randomEx = (rand() % 200 - 100) / 1.0f;
	int prefix[2] = { -1,1 };
	int nIndex = rand() % 2;
	int R = 100;
	glm::vec2 Positon_circle = object + glm::vec2(0.0f, -200.0f);
	GLfloat result = 100 * 100 - abs(random*random);
	randomEx = -glm::sqrt(result);
	randomEx = randomEx * prefix[nIndex];
	randomEx = -random * random/100;
	GLfloat rColor = (rand() % 100) / 100.f;
	GLfloat gColor = (rand() % 100) / 100.f;
	GLfloat bColor = (rand() % 100) / 100.f;
	particle.Position = object  + glm::vec2(random, randomEx) + offset + glm::vec2(0.0f,-100.0f);
	particle.Color = glm::vec4(rColor, gColor, bColor, 1.0f);
	particle.Life = 2.0f;
	particle.Velocity = glm::vec2(0.0f,5.0f);
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
		p.Life -= dt*0.001f;
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


