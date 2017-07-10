// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "ParticleBuffer.h"
#include "Jewel3D/Application/Logging.h"
#include "Jewel3D/Math/Math.h"

#include <GLEW/GL/glew.h>

namespace Jwl
{
	ParticleBuffer::ParticleBuffer()
	{
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glBindVertexArray(GL_NONE);

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
	}

	ParticleBuffer::ParticleBuffer(const ParticleBuffer& other)
	{
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glBindVertexArray(GL_NONE);

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

		*this = other;
	}

	ParticleBuffer::ParticleBuffer(ParticleBuffer&& other)
		: positions(other.positions)
		, velocities(other.velocities)
		, ages(other.ages)
		, lifetimes(other.lifetimes)
		, sizes(other.sizes)
		, colors(other.colors)
		, alphas(other.alphas)
		, rotations(other.rotations)
		, ageRatios(other.ageRatios)
		, buffers(other.buffers)
		, numParticles(other.numParticles)
		, VBO(other.VBO)
		, VAO(other.VAO)
	{
		other.positions = nullptr;
		other.velocities = nullptr;
		other.ages = nullptr;
		other.lifetimes = nullptr;
		other.sizes = nullptr;
		other.colors = nullptr;
		other.alphas = nullptr;
		other.rotations = nullptr;
		other.ageRatios = nullptr;

		other.buffers = ParticleBuffers::None;

		other.numParticles = 0;

		other.VBO = GL_NONE;
		other.VAO = GL_NONE;
	}

	ParticleBuffer& ParticleBuffer::operator=(const ParticleBuffer& other)
	{
		Unload();

		SetBuffers(other.numParticles, other.buffers);

		return *this;
	}

	ParticleBuffer::~ParticleBuffer()
	{
		glDeleteBuffers(1, &VBO);
		VBO = GL_NONE;

		glDeleteVertexArrays(1, &VAO);
		VAO = GL_NONE;

		Unload();
	}

	void ParticleBuffer::Unload()
	{
		free(positions);
		free(velocities);
		free(ages);
		free(lifetimes);
		free(sizes);
		free(colors);
		free(alphas);
		free(rotations);
		free(ageRatios);

		positions	= nullptr;
		velocities	= nullptr;
		ages		= nullptr;
		lifetimes	= nullptr;
		sizes		= nullptr;
		colors		= nullptr;
		alphas		= nullptr;
		rotations	= nullptr;
		ageRatios	= nullptr;
	}

	void ParticleBuffer::SetBuffers(u32 _numParticles, EnumFlags<ParticleBuffers> _buffers)
	{
		if (numParticles != _numParticles)
		{
			positions	= (vec3*)realloc(positions, sizeof(vec3) * _numParticles);
			velocities	= (vec3*)realloc(velocities, sizeof(vec3) * _numParticles);
			ages		= (f32*)realloc(ages, sizeof(f32) * _numParticles);
			lifetimes	= (f32*)realloc(lifetimes, sizeof(f32) * _numParticles);
		}

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		// Position buffer is always present.
		glEnableVertexAttribArray(0);
		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(0));
		u32 bufferSize = sizeof(vec3);

		if (_buffers.Has(ParticleBuffers::Size))
		{
			glVertexAttribPointer((GLuint)1, 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(bufferSize * _numParticles));
			bufferSize += sizeof(vec2);

			if (sizes == nullptr)
			{
				glEnableVertexAttribArray(1);
				sizes = (vec2*)malloc(sizeof(vec2) * _numParticles);
			}
			else if (numParticles != _numParticles)
			{
				sizes = (vec2*)realloc(sizes, sizeof(vec2) * _numParticles);
			}
		}
		else
		{
			// The effect will require the age of the particle in order to LERP in-shader
			// based on the global size values. The same is true for Colors and Alpha.
			buffers |= ParticleBuffers::AgeRatio;
			glDisableVertexAttribArray(1);
		}

		if (_buffers.Has(ParticleBuffers::Color))
		{
			glVertexAttribPointer((GLuint)2, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(bufferSize * _numParticles));
			bufferSize += sizeof(vec3);

			if (colors == nullptr)
			{
				glEnableVertexAttribArray(2);
				colors = (vec3*)malloc(sizeof(vec3) * _numParticles);
			}
			else if (numParticles != _numParticles)
			{
				colors = (vec3*)realloc(colors, sizeof(vec3) * _numParticles);
			}
		}
		else
		{
			buffers |= ParticleBuffers::AgeRatio;
			glDisableVertexAttribArray(2);
		}

		if (_buffers.Has(ParticleBuffers::Alpha))
		{
			glVertexAttribPointer((GLuint)3, 1, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(bufferSize * _numParticles));
			bufferSize += sizeof(f32);

			if (alphas == nullptr)
			{
				glEnableVertexAttribArray(3);
				alphas = (f32*)malloc(sizeof(f32) * _numParticles);
			}
			else if (numParticles != _numParticles)
			{
				alphas = (f32*)realloc(alphas, sizeof(f32) * _numParticles);
			}
		}
		else
		{
			buffers |= ParticleBuffers::AgeRatio;
			glDisableVertexAttribArray(3);
		}

		if (_buffers.Has(ParticleBuffers::Rotation))
		{
			glVertexAttribPointer((GLuint)4, 1, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(bufferSize * _numParticles));
			bufferSize += sizeof(f32);

			if (rotations == nullptr)
			{
				glEnableVertexAttribArray(4);
				rotations = (f32*)malloc(sizeof(f32) * _numParticles);
			}
			else if (numParticles != _numParticles)
			{
				rotations = (f32*)realloc(rotations, sizeof(f32) * _numParticles);
			}
		}
		else
		{
			glDisableVertexAttribArray(4);
		}

		if (_buffers.Has(ParticleBuffers::AgeRatio))
		{
			glVertexAttribPointer((GLuint)5, 1, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(bufferSize * _numParticles));
			bufferSize += sizeof(f32);

			if (ageRatios == nullptr)
			{
				glEnableVertexAttribArray(5);
				ageRatios = (f32*)malloc(sizeof(f32) * _numParticles);
			}
			else if (numParticles != _numParticles)
			{
				ageRatios = (f32*)realloc(ageRatios, sizeof(f32) * _numParticles);
			}
		}
		else
		{
			glDisableVertexAttribArray(5);
		}

		glBufferData(GL_ARRAY_BUFFER, bufferSize * _numParticles, NULL, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
		glBindVertexArray(GL_NONE);

		numParticles = _numParticles;
		buffers = _buffers;
	}

	void ParticleBuffer::Update(u32 _numParticles)
	{
		if (_numParticles == 0)
		{
			return;
		}

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		void* buffer = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

		memcpy(buffer, &positions[0], sizeof(vec3) * _numParticles);
		buffer = static_cast<vec3*>(buffer) + numParticles;

		if (buffers.Has(ParticleBuffers::Size))
		{
			memcpy(buffer, &sizes[0], sizeof(vec2) * _numParticles);
			buffer = static_cast<vec2*>(buffer) + numParticles;
		}

		if (buffers.Has(ParticleBuffers::Color))
		{
			memcpy(buffer, &colors[0], sizeof(vec3) * _numParticles);
			buffer = static_cast<vec3*>(buffer) + numParticles;
		}

		if (buffers.Has(ParticleBuffers::Alpha))
		{
			memcpy(buffer, &alphas[0], sizeof(f32) * _numParticles);
			buffer = static_cast<f32*>(buffer) + numParticles;
		}

		if (buffers.Has(ParticleBuffers::Rotation))
		{
			memcpy(buffer, &rotations[0], sizeof(f32) * _numParticles);
			buffer = static_cast<f32*>(buffer) + numParticles;
		}

		if (buffers.Has(ParticleBuffers::AgeRatio))
		{
			memcpy(buffer, &ageRatios[0], sizeof(f32) * _numParticles);
		}

		glUnmapBuffer(GL_ARRAY_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
	}

	void ParticleBuffer::Kill(u32 index, u32 last)
	{
		ASSERT(index < numParticles, "Index out of bounds.");
		ASSERT(last  < numParticles, "Index out of bounds.");
		ASSERT(index <= last, "Dead particle cannot be further than the last particle in the list.");

		positions[index] = positions[last];
		velocities[index] = velocities[last];
		ages[index] = ages[last];
		lifetimes[index] = lifetimes[last];

		if (buffers.Has(ParticleBuffers::Size))
		{
			sizes[index] = sizes[last];
		}

		if (buffers.Has(ParticleBuffers::Color))
		{
			colors[index] = colors[last];
		}

		if (buffers.Has(ParticleBuffers::Alpha))
		{
			alphas[index] = alphas[last];
		}

		if (buffers.Has(ParticleBuffers::Rotation))
		{
			rotations[index] = rotations[last];
		}
	}

	bool ParticleBuffer::IsAlive(u32 index)
	{
		ASSERT(index < numParticles, "Index out of bounds.");
		return ages[index] < lifetimes[index];
	}

	EnumFlags<ParticleBuffers> ParticleBuffer::GetBuffers() const
	{
		return buffers;
	}

	u32 ParticleBuffer::GetVAO() const
	{
		return VAO;
	}

	u32 ParticleBuffer::GetNumParticles() const
	{
		return numParticles;
	}
}