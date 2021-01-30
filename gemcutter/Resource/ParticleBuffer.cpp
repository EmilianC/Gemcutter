// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "ParticleBuffer.h"
#include "Jewel3D/Application/Logging.h"
#include "Jewel3D/Math/Vector.h"

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

	ParticleBuffer::ParticleBuffer(ParticleBuffer&& other) noexcept
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
		, VAO(other.VAO)
		, VBO(other.VBO)
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

		positions  = nullptr;
		velocities = nullptr;
		ages       = nullptr;
		lifetimes  = nullptr;
		sizes      = nullptr;
		colors     = nullptr;
		alphas     = nullptr;
		rotations  = nullptr;
		ageRatios  = nullptr;
	}

	void ParticleBuffer::SetBuffers(unsigned _numParticles, EnumFlags<ParticleBuffers> _buffers)
	{
		if (numParticles != _numParticles)
		{
			positions  = static_cast<vec3*>(realloc(positions, sizeof(vec3) * _numParticles));
			velocities = static_cast<vec3*>(realloc(velocities, sizeof(vec3) * _numParticles));
			ages       = static_cast<float*>(realloc(ages, sizeof(float) * _numParticles));
			lifetimes  = static_cast<float*>(realloc(lifetimes, sizeof(float) * _numParticles));
		}

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		// Position buffer is always present.
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0u, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(0));
		unsigned bufferSize = sizeof(vec3);

		if (_buffers.Has(ParticleBuffers::Size))
		{
			glVertexAttribPointer(1u, 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(bufferSize * _numParticles));
			bufferSize += sizeof(vec2);

			if (sizes == nullptr)
			{
				glEnableVertexAttribArray(1);
				sizes = static_cast<vec2*>(malloc(sizeof(vec2) * _numParticles));
			}
			else if (numParticles != _numParticles)
			{
				sizes = static_cast<vec2*>(realloc(sizes, sizeof(vec2) * _numParticles));
			}
		}
		else
		{
			glDisableVertexAttribArray(1);
		}

		if (_buffers.Has(ParticleBuffers::Color))
		{
			glVertexAttribPointer(2u, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(bufferSize * _numParticles));
			bufferSize += sizeof(vec3);

			if (colors == nullptr)
			{
				glEnableVertexAttribArray(2);
				colors = static_cast<vec3*>(malloc(sizeof(vec3) * _numParticles));
			}
			else if (numParticles != _numParticles)
			{
				colors = static_cast<vec3*>(realloc(colors, sizeof(vec3) * _numParticles));
			}
		}
		else
		{
			glDisableVertexAttribArray(2);
		}

		if (_buffers.Has(ParticleBuffers::Alpha))
		{
			glVertexAttribPointer(3u, 1, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(bufferSize * _numParticles));
			bufferSize += sizeof(float);

			if (alphas == nullptr)
			{
				glEnableVertexAttribArray(3);
				alphas = static_cast<float*>(malloc(sizeof(float) * _numParticles));
			}
			else if (numParticles != _numParticles)
			{
				alphas = static_cast<float*>(realloc(alphas, sizeof(float) * _numParticles));
			}
		}
		else
		{
			glDisableVertexAttribArray(3);
		}

		if (_buffers.Has(ParticleBuffers::Rotation))
		{
			glVertexAttribPointer(4u, 1, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(bufferSize * _numParticles));
			bufferSize += sizeof(float);

			if (rotations == nullptr)
			{
				glEnableVertexAttribArray(4);
				rotations = static_cast<float*>(malloc(sizeof(float) * _numParticles));
			}
			else if (numParticles != _numParticles)
			{
				rotations = static_cast<float*>(realloc(rotations, sizeof(float) * _numParticles));
			}
		}
		else
		{
			glDisableVertexAttribArray(4);
		}

		if (_buffers.Has(ParticleBuffers::AgeRatio))
		{
			glVertexAttribPointer(5u, 1, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(bufferSize * _numParticles));
			bufferSize += sizeof(float);

			if (ageRatios == nullptr)
			{
				glEnableVertexAttribArray(5);
				ageRatios = static_cast<float*>(malloc(sizeof(float) * _numParticles));
			}
			else if (numParticles != _numParticles)
			{
				ageRatios = static_cast<float*>(realloc(ageRatios, sizeof(float) * _numParticles));
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

	void ParticleBuffer::Update(unsigned _numParticles)
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
			memcpy(buffer, sizes, sizeof(vec2) * _numParticles);
			buffer = static_cast<vec2*>(buffer) + numParticles;
		}

		if (buffers.Has(ParticleBuffers::Color))
		{
			memcpy(buffer, colors, sizeof(vec3) * _numParticles);
			buffer = static_cast<vec3*>(buffer) + numParticles;
		}

		if (buffers.Has(ParticleBuffers::Alpha))
		{
			memcpy(buffer, alphas, sizeof(float) * _numParticles);
			buffer = static_cast<float*>(buffer) + numParticles;
		}

		if (buffers.Has(ParticleBuffers::Rotation))
		{
			memcpy(buffer, rotations, sizeof(float) * _numParticles);
			buffer = static_cast<float*>(buffer) + numParticles;
		}

		if (buffers.Has(ParticleBuffers::AgeRatio))
		{
			memcpy(buffer, ageRatios, sizeof(float) * _numParticles);
		}

		glUnmapBuffer(GL_ARRAY_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
	}

	void ParticleBuffer::Kill(unsigned index, unsigned last)
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

	bool ParticleBuffer::IsAlive(unsigned index) const
	{
		ASSERT(index < numParticles, "Index out of bounds.");
		return ages[index] < lifetimes[index];
	}

	EnumFlags<ParticleBuffers> ParticleBuffer::GetBuffers() const
	{
		return buffers;
	}

	unsigned ParticleBuffer::GetVAO() const
	{
		return VAO;
	}

	unsigned ParticleBuffer::GetNumParticles() const
	{
		return numParticles;
	}
}
