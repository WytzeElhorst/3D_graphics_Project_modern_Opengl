/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "particle_generator.h"
#include <glm/gtc/type_ptr.hpp>

ParticleGenerator::ParticleGenerator(GLuint program, GLuint texture, GLuint amount)
	: program(program), texture(texture), amount(amount)
{
	this->init();
}

void ParticleGenerator::Update(GLfloat dt)
{
	for (GLuint i = 0; i < this->amount; ++i)
	{
		Particle &p = this->particles[i];
		p.Life -= dt; // reduce life
		if (p.Life > 0.0f)
		{	// particle is alive, thus update
			p.Position += p.Velocity * dt;
			p.Color.a -= dt * 2.5;
		}
	}
	for (GLuint j = 0; j < centers.size(); ++j)
	{
		for (GLuint i = 0; i < 100; ++i)
		{
			int unusedParticle = this->firstUnusedParticle();
			this->respawnParticle(this->particles[unusedParticle], centers[j], glm::vec3(0,0,0));
		}
		centers.erase(centers.begin() + j);
	}
}

// Render all particles
void ParticleGenerator::Draw(glm::mat4 mvp)
{
	// Use additive blending to give it a 'glow' effect
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glUseProgram(program);
	for (Particle particle : this->particles)
	{
		if (particle.Life > 0.0f)
		{
			glBindVertexArray(this->VAO);
			glUniformMatrix4fv(glGetUniformLocation(program, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
			glUniform4f(glGetUniformLocation(program, "offset"), particle.Position.x, particle.Position.y, particle.Position.z, 0);
			glUniform4f(glGetUniformLocation(program, "color"), particle.Color.x, particle.Color.y, particle.Color.z, particle. Color.w);

			GLint texture_unit6 = 6;
			glActiveTexture(GL_TEXTURE0 + texture_unit6);
			glBindTexture(GL_TEXTURE_2D, texture);
			glUniform1i(glGetUniformLocation(program, "sprite"), texture_unit6);

			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
		}
	}
	// Don't forget to reset to default blending mode
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleGenerator::AddCenter(glm::vec3 pos)
{
	centers.push_back(pos);
}

void ParticleGenerator::init()
{
	// Set up mesh and attribute properties
	GLuint VBO;
	GLfloat particle_quad[] = {
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(this->VAO);
	// Fill mesh buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
	// Set mesh attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);

	// Create this->amount default particle instances
	for (GLuint i = 0; i < this->amount; ++i)
		this->particles.push_back(Particle());
}

// Stores the index of the last particle used (for quick access to next dead particle)
GLuint lastUsedParticle = 0;
GLuint ParticleGenerator::firstUnusedParticle()
{
	// First search from last used particle, this will usually return almost instantly
	for (GLuint i = lastUsedParticle; i < this->amount; ++i) {
		if (this->particles[i].Life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	// Otherwise, do a linear search
	for (GLuint i = 0; i < lastUsedParticle; ++i) {
		if (this->particles[i].Life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	// All particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
	lastUsedParticle = 0;
	return 0;
}

void ParticleGenerator::respawnParticle(Particle &particle, glm::vec3 position, glm::vec3 offset)
{
	GLfloat R = 0.02;
	GLfloat pi = 3.14159265359;

	GLfloat a = ((rand() % 100) - 50) / 100.0f;
	if (a == 0)
		a = 1.0f;
	GLfloat b = ((rand() % 100) - 50) / 100.0f;
	if (b == 0)
		b = 1.0f;
	GLfloat c = ((rand() % 100) - 50) / 100.0f;
	if (c == 0)
		c = 1.0f;
	//glm::vec3 c = glm::vec3(b*R*cos(2 * pi*a / b), b*R*sin(2 * pi*a / b), 0);
	GLfloat mult = R * ( 1 / sqrt(pow(a, 2) + pow(b, 2) + pow(c, 2)));
	glm::vec3 d = glm::vec3(a * mult, b * mult, c* mult);
	GLfloat rColor = 0.5 + ((rand() % 100) / 100.0f);
	particle.Position = position + d + offset;
	particle.Color = glm::vec4(rColor, rColor, rColor, 1.0f);
	particle.Life = 1.0f;
	particle.Velocity = (d + offset) * 10.0f;
}