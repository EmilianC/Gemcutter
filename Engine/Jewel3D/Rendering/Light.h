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
		enum class Type
		{
			Point,
			Directional,
			Spot
		};

		Light(Entity& owner);
		Light(Entity& owner, const vec3& color);
		Light(Entity& owner, const vec3& color, Type type);
		Light& operator=(const Light&);

		Type type = Type::Point;

		//- Main color of the light.
		UniformHandle<vec3> color;
		//- Base attenuation factor.
		UniformHandle<float> attenuationConstant;
		//- Linear attenuation factor.
		UniformHandle<float> attenuationLinear;
		//- Exponential attenuation factor.
		UniformHandle<float> attenuationQuadratic;
		//- Cone angle for spotlights, in degrees.
		float angle = 25.0f;

		//- Keeps the internal buffer up to date with the light's transform.
		void Update();

		UniformBuffer::Ptr& GetBuffer();

	private:
		void CreateUniformBuffer();
		void CreateUniformHandles();

		UniformHandle<vec3> position;
		UniformHandle<vec3> direction;
		UniformHandle<float> cosAngle;
		UniformBuffer::Ptr lightBuffer;
	};
}
