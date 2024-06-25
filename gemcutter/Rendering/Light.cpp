// Copyright (c) 2017 Emilian Cioca
#include "Light.h"
#include "gemcutter/Math/Math.h"

namespace gem
{
	Light::Light(Entity& _owner)
		: Component(_owner)
	{
		CreateUniformBuffer();
		color.Set(vec3(1.0f));
	}

	Light::Light(Entity& _owner, const vec3& _color)
		: Component(_owner)
	{
		CreateUniformBuffer();
		color.Set(_color);
	}

	Light::Light(Entity& _owner, const vec3& _color, Type _type)
		: Component(_owner)
	{
		CreateUniformBuffer();
		color.Set(_color);
	}

	void Light::Update()
	{
		// The type must be a full uint to match up with GLSL.
		typeHandle.Set(static_cast<unsigned int>(type));

		switch (type)
		{
		case Type::Spot:
			{
				ASSERT(angle >= 0.0f && angle <= 360.0f, "Invalid SpotLight cone angle ( %f )", angle);

				// We don't use GetWorldTransform() here because it includes scaling.
				quat rot = owner.GetWorldRotation();
				vec3 pos = owner.GetWorldPosition();
				direction.Set(rot.GetForward());
				position.Set(pos);
				cosAngle.Set(cos(ToRadian(angle * 0.5f)));
				break;
			}
		case Type::Point:
			{
				vec3 pos = owner.GetWorldPosition();
				direction.Set(vec3(0.0f));
				position.Set(pos);
				break;
			}
		case Type::Directional:
			{
				quat rot = owner.GetWorldRotation();
				direction.Set(rot.GetForward());
				position.Set(vec3(0.0f));
				break;
			}
		}
	}

	UniformBuffer::Ptr& Light::GetBuffer()
	{
		return lightBuffer;
	}

	void Light::CreateUniformBuffer()
	{
		lightBuffer = UniformBuffer::MakeNew();
		color     = lightBuffer->AddUniform<vec3>("Color");
		position  = lightBuffer->AddUniform<vec3>("Position");
		direction = lightBuffer->AddUniform<vec3>("Direction");
		attenuationLinear    = lightBuffer->AddUniform<float>("AttenuationLinear");
		attenuationQuadratic = lightBuffer->AddUniform<float>("AttenuationQuadratic");
		cosAngle   = lightBuffer->AddUniform<float>("Angle");
		typeHandle = lightBuffer->AddUniform<unsigned int>("Type");
		lightBuffer->InitBuffer(BufferUsage::Dynamic);

		attenuationLinear.Set(0.0f);
		attenuationQuadratic.Set(1.0f);
	}
}

REFLECT(gem::Light::Type)
	ENUM_VALUES {
		REF_VALUE(Point)
		REF_VALUE(Directional)
		REF_VALUE(Spot)
	}
REF_END;

REFLECT_COMPONENT(gem::Light, gem::ComponentBase)
	MEMBERS {
		REF_MEMBER(type)
		REF_MEMBER(color)
		REF_MEMBER(attenuationLinear)
		REF_MEMBER(attenuationQuadratic)
		REF_MEMBER(angle)
	}
REF_END;
