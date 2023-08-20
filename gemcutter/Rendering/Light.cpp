// Copyright (c) 2017 Emilian Cioca
#include "Light.h"
#include "gemcutter/Math/Math.h"

namespace gem
{
	Light::Light(Entity& _owner)
		: Component(_owner)
	{
		CreateUniformBuffer();
		CreateUniformHandles();
		InitializeUniformValues();

		color.Set(vec3(1.0f));
		type.Set(Type::Point);
	}

	Light::Light(Entity& _owner, const vec3& _color)
		: Component(_owner)
	{
		CreateUniformBuffer();
		CreateUniformHandles();
		InitializeUniformValues();

		color.Set(_color);
		type.Set(Type::Point);
	}

	Light::Light(Entity& _owner, const vec3& _color, Type _type)
		: Component(_owner)
	{
		CreateUniformBuffer();
		CreateUniformHandles();
		InitializeUniformValues();

		color.Set(_color);
		type.Set(_type);
	}

	void Light::Update()
	{
		switch (type.Get())
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
		lightBuffer->AddUniform<vec3>("Color");
		lightBuffer->AddUniform<vec3>("Position");
		lightBuffer->AddUniform<vec3>("Direction");
		lightBuffer->AddUniform<float>("AttenuationLinear");
		lightBuffer->AddUniform<float>("AttenuationQuadratic");
		lightBuffer->AddUniform<float>("Angle");
		lightBuffer->AddUniform<Type>("Type");
		lightBuffer->InitBuffer(BufferUsage::Dynamic);
	}

	void Light::CreateUniformHandles()
	{
		ASSERT(lightBuffer, "Light Uniform Buffer is not initialized.");

		color = lightBuffer->MakeHandle<vec3>("Color");
		position = lightBuffer->MakeHandle<vec3>("Position");
		direction = lightBuffer->MakeHandle<vec3>("Direction");
		attenuationLinear = lightBuffer->MakeHandle<float>("AttenuationLinear");
		attenuationQuadratic = lightBuffer->MakeHandle<float>("AttenuationQuadratic");
		cosAngle = lightBuffer->MakeHandle<float>("Angle");
		type = lightBuffer->MakeHandle<Type>("Type");
	}

	void Light::InitializeUniformValues()
	{
		attenuationLinear.Set(0.0f);
		attenuationQuadratic.Set(1.0f);
		cosAngle.Set(cos(ToRadian(angle)));
	}
}

REFLECT(gem::Light::Type)
	ENUM_VALUES {
		REF_VALUE(Point)
		REF_VALUE(Directional)
		REF_VALUE(Spot)
	}
REF_END;

REFLECT_SIMPLE(gem::UniformHandle<gem::Light::Type>);

REFLECT(gem::Light) BASES { REF_BASE(gem::ComponentBase) }
	MEMBERS {
		REF_MEMBER(type)
		REF_MEMBER(color)
		REF_MEMBER(attenuationLinear)
		REF_MEMBER(attenuationQuadratic)
		REF_MEMBER(angle)
	}
REF_END;
