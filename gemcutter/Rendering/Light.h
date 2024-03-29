// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "gemcutter/Entity/Entity.h"
#include "gemcutter/Math/Vector.h"
#include "gemcutter/Resource/UniformBuffer.h"

namespace gem
{
	// Defaults to a point light.
	class Light : public Component<Light>
	{
	public:
		enum class Type : uint16_t
		{
			Point,
			Directional,
			Spot
		};

		Light(Entity& owner);
		Light(Entity& owner, const vec3& color);
		Light(Entity& owner, const vec3& color, Type type);

		Type type = Type::Point;
		// Defaults to a white light.
		UniformHandle<vec3> color;
		// Defaults to zero.
		UniformHandle<float> attenuationLinear;
		// Defaults to one.
		UniformHandle<float> attenuationQuadratic;
		// Cone angle for spotlights, in degrees.
		float angle = 25.0f;

		// Keeps the internal buffer up to date with the light's transform.
		void Update();

		UniformBuffer::Ptr& GetBuffer();

	private:
		void CreateUniformBuffer();

		UniformHandle<vec3> position;
		UniformHandle<vec3> direction;
		UniformHandle<float> cosAngle;
		UniformHandle<unsigned int> typeHandle;
		UniformBuffer::Ptr lightBuffer;
	};
}
