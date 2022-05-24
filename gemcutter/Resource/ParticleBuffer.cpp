// Copyright (c) 2017 Emilian Cioca
#include "ParticleBuffer.h"
#include "gemcutter/Application/Logging.h"
#include "gemcutter/Math/Vector.h"

namespace gem
{
	ParticleBuffer::ParticleBuffer(unsigned _maxParticles)
		: maxParticles(_maxParticles)
	{
		array = VertexArray::MakeNew(VertexArrayFormat::Point);

		positions  = static_cast<vec3*>(malloc(sizeof(vec3) * maxParticles));
		velocities = static_cast<vec3*>(malloc(sizeof(vec3) * maxParticles));
		ages       = static_cast<float*>(malloc(sizeof(float) * maxParticles));
		lifetimes  = static_cast<float*>(malloc(sizeof(float) * maxParticles));
	}

	ParticleBuffer::~ParticleBuffer()
	{
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

	void ParticleBuffer::SetAttributes(EnumFlags<ParticleAttributes> _attributes)
	{
		if (attributes == _attributes)
			return;

		attributes = _attributes;

		array->RemoveStreams();
		buffer = VertexBuffer::MakeNew(TotalBufferSize(), VertexBufferUsage::Dynamic, VertexBufferType::Data);

		// Position attribute is always used.
		VertexStream stream {
			.buffer      = buffer,
			.bindingUnit = 0,
			.format      = VertexFormat::Vec3,
			.normalized  = false,
			.startOffset = 0,
			.stride      = 0
		};

		array->AddStream(stream);

		unsigned startOffset = sizeof(vec3);
		if (attributes.Has(ParticleAttributes::Size))
		{
			stream.bindingUnit = 1;
			stream.format = VertexFormat::Vec2;
			stream.startOffset = startOffset * maxParticles;

			array->AddStream(stream);
			startOffset += sizeof(vec2);

			if (!sizes)
			{
				sizes = static_cast<vec2*>(malloc(sizeof(vec2) * maxParticles));
			}
		}

		if (attributes.Has(ParticleAttributes::Color))
		{
			stream.bindingUnit = 2;
			stream.format = VertexFormat::Vec3;
			stream.startOffset = startOffset * maxParticles;

			array->AddStream(stream);
			startOffset += sizeof(vec3);

			if (!colors)
			{
				colors = static_cast<vec3*>(malloc(sizeof(vec3) * maxParticles));
			}
		}

		if (attributes.Has(ParticleAttributes::Alpha))
		{
			stream.bindingUnit = 3;
			stream.format = VertexFormat::Float;
			stream.startOffset = startOffset * maxParticles;

			array->AddStream(stream);
			startOffset += sizeof(float);

			if (!alphas)
			{
				alphas = static_cast<float*>(malloc(sizeof(float) * maxParticles));
			}
		}

		if (attributes.Has(ParticleAttributes::Rotation))
		{
			stream.bindingUnit = 4;
			stream.format = VertexFormat::Float;
			stream.startOffset = startOffset * maxParticles;

			array->AddStream(stream);
			startOffset += sizeof(float);

			if (!rotations)
			{
				rotations = static_cast<float*>(malloc(sizeof(float) * maxParticles));
			}
		}

		if (attributes.Has(ParticleAttributes::AgeRatio))
		{
			stream.bindingUnit = 5;
			stream.format = VertexFormat::Float;
			stream.startOffset = startOffset * maxParticles;

			array->AddStream(std::move(stream));

			if (!ageRatios)
			{
				ageRatios = static_cast<float*>(malloc(sizeof(float) * maxParticles));
			}
		}
	}

	void ParticleBuffer::Update(unsigned activeParticles)
	{
		array->SetVertexCount(activeParticles);

		if (activeParticles == 0)
		{
			return;
		}

		BufferMapping mapping = buffer->MapBuffer(VertexAccess::WriteOnly);
		unsigned char* data = mapping.GetPtr();

		memcpy(data, positions, sizeof(vec3) * activeParticles);
		data += sizeof(vec3) * maxParticles;

		if (attributes.Has(ParticleAttributes::Size))
		{
			memcpy(data, sizes, sizeof(vec2) * activeParticles);
			data += sizeof(vec2) * maxParticles;
		}

		if (attributes.Has(ParticleAttributes::Color))
		{
			memcpy(data, colors, sizeof(vec3) * activeParticles);
			data += sizeof(vec3) * maxParticles;
		}

		if (attributes.Has(ParticleAttributes::Alpha))
		{
			memcpy(data, alphas, sizeof(float) * activeParticles);
			data += sizeof(float) * maxParticles;
		}

		if (attributes.Has(ParticleAttributes::Rotation))
		{
			memcpy(data, rotations, sizeof(float) * activeParticles);
			data += sizeof(float) * maxParticles;
		}

		if (attributes.Has(ParticleAttributes::AgeRatio))
		{
			memcpy(data, ageRatios, sizeof(float) * activeParticles);
		}
	}

	void ParticleBuffer::Kill(unsigned index, unsigned last)
	{
		ASSERT(index < maxParticles, "Index out of bounds.");
		ASSERT(last  < maxParticles, "Index out of bounds.");
		ASSERT(index <= last, "Dead particle cannot be further than the last particle in the list.");

		positions[index] = positions[last];
		velocities[index] = velocities[last];
		ages[index] = ages[last];
		lifetimes[index] = lifetimes[last];

		if (attributes.Has(ParticleAttributes::Size))
		{
			sizes[index] = sizes[last];
		}

		if (attributes.Has(ParticleAttributes::Color))
		{
			colors[index] = colors[last];
		}

		if (attributes.Has(ParticleAttributes::Alpha))
		{
			alphas[index] = alphas[last];
		}

		if (attributes.Has(ParticleAttributes::Rotation))
		{
			rotations[index] = rotations[last];
		}

		// We don't need to copy the AgeRatio because it is recomputed every frame anyways.
	}

	bool ParticleBuffer::IsAlive(unsigned index) const
	{
		ASSERT(index < maxParticles, "Index out of bounds.");
		return ages[index] < lifetimes[index];
	}

	EnumFlags<ParticleAttributes> ParticleBuffer::GetAttributes() const
	{
		return attributes;
	}

	VertexArray::Ptr ParticleBuffer::GetArray() const
	{
		return array;
	}

	unsigned ParticleBuffer::GetMaxParticles() const
	{
		return maxParticles;
	}

	unsigned ParticleBuffer::TotalBufferSize() const
	{
		unsigned bufferSize = sizeof(vec3); // Position
		if (attributes.Has(ParticleAttributes::Size))     bufferSize += sizeof(vec2);
		if (attributes.Has(ParticleAttributes::Color))    bufferSize += sizeof(vec3);
		if (attributes.Has(ParticleAttributes::Alpha))    bufferSize += sizeof(float);
		if (attributes.Has(ParticleAttributes::Rotation)) bufferSize += sizeof(float);
		if (attributes.Has(ParticleAttributes::AgeRatio)) bufferSize += sizeof(float);

		return bufferSize * maxParticles;
	}
}
