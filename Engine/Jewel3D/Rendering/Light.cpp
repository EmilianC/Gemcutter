// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "Light.h"
#include "Jewel3D/Math/Math.h"

namespace Jwl
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

	Light& Light::operator=(const Light& other)
	{
		lightBuffer->Copy(*other.lightBuffer);

		// Regenerate the handles.
		CreateUniformHandles();

		// Copy the remaining data.
		type.Set(other.type.Get());
		color.Set(other.color.Get());
		attenuationLinear.Set(other.attenuationLinear.Get());
		attenuationQuadratic.Set(other.attenuationQuadratic.Get());
		position.Set(other.position.Get());
		direction.Set(other.direction.Get());
		angle = other.angle;
		cosAngle.Set(cos(ToRadian(angle)));

		return *this;
	}

	void Light::Update()
	{
		switch (type.Get())
		{
		case Type::Spot:
		{
			ASSERT(angle >= 0.0f && angle <= 360.0f, "Invalid SpotLight cone angle ( %f )", angle);

			mat4 transform = owner.GetWorldTransform();
			direction.Set(-transform.GetForward());
			position.Set(transform.GetTranslation());
			cosAngle.Set(cos(ToRadian(angle * 0.5f)));
			break;
		}
		case Type::Point:
		{
			mat4 transform = owner.GetWorldTransform();
			direction.Set(vec3(0.0f));
			position.Set(transform.GetTranslation());
			break;
		}
		case Type::Directional:
		{
			quat rotation = owner.GetWorldRotation();
			direction.Set(-rotation.GetForward());
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
		lightBuffer->InitBuffer(VertexBufferUsage::Dynamic);
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
