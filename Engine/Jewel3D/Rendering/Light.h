// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Jewel3D/Entity/Entity.h"
#include "Jewel3D/Math/Vector.h"
#include "Jewel3D/Resource/UniformBuffer.h"

namespace Jwl
{
	class Light : public Component<Light>
	{
	public:
		enum class Type : unsigned
		{
			Point,
			Directional,
			Spot
		};

		Light(Entity& owner);
		Light(Entity& owner, const vec3& color);
		Light(Entity& owner, const vec3& color, Type type);
		Light& operator=(const Light&);

		// Defaults to a point light.
		UniformHandle<Type> type;
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
		void CreateUniformHandles();
		void InitializeUniformValues();

		UniformHandle<vec3> position;
		UniformHandle<vec3> direction;
		UniformHandle<float> cosAngle;
		UniformBuffer::Ptr lightBuffer;
	};
}
