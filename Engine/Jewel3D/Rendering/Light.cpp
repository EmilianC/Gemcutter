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

		color.Set(vec3(1.0f));
		attenuationConstant.Set(0.01f);
		attenuationLinear.Set(0.25f);
		attenuationQuadratic.Set(0.01f);
		cosAngle.Set(cos(ToRadian(angle)));
	}

	Light::Light(Entity& _owner, const vec3& _color)
		: Component(_owner)
	{
		CreateUniformBuffer();
		CreateUniformHandles();

		color.Set(_color);
		attenuationConstant.Set(0.01f);
		attenuationLinear.Set(0.25f);
		attenuationQuadratic.Set(0.01f);
		cosAngle.Set(cos(ToRadian(angle)));
	}

	Light::Light(Entity& _owner, const vec3& _color, Type _type)
		: Component(_owner)
		, type(_type)
	{
		CreateUniformBuffer();
		CreateUniformHandles();

		color.Set(_color);
		attenuationConstant.Set(0.01f);
		attenuationLinear.Set(0.25f);
		attenuationQuadratic.Set(0.01f);
		cosAngle.Set(cos(ToRadian(angle)));
	}

	Light& Light::operator=(const Light& other)
	{
		lightBuffer->Copy(*other.lightBuffer);

		// Regenerate the handles.
		CreateUniformHandles();

		// Copy the remaining data.
		type = other.type;
		color.Set(other.color.Get());
		attenuationConstant.Set(other.attenuationConstant.Get());
		attenuationLinear.Set(other.attenuationLinear.Get());
		attenuationQuadratic.Set(other.attenuationQuadratic.Get());
		angle = other.angle;
		cosAngle.Set(cos(ToRadian(angle)));
		position.Set(other.position.Get());
		direction.Set(other.direction.Get());

		return *this;
	}

	void Light::Update()
	{
		auto transform = owner.GetWorldTransform();
		
		switch (type)
		{
		case Type::Spot:
			ASSERT(angle >= 0.0f && angle <= 360.0f, "Invalid SpotLight cone angle ( %f )", angle);
			direction.Set(-transform.GetForward());
			position.Set(transform.GetTranslation());
			cosAngle.Set(cos(ToRadian(angle * 0.5f)));
			break;
		case Type::Point:
			direction.Set(vec3(0.0f));
			position.Set(transform.GetTranslation());
			break;
		case Type::Directional:
			direction.Set(-transform.GetForward());
			position.Set(vec3(0.0f));
			break;
		}
	}

	UniformBuffer::Ptr& Light::GetBuffer()
	{
		return lightBuffer;
	}

	void Light::CreateUniformBuffer()
	{
		lightBuffer = UniformBuffer::MakeNew();
		lightBuffer->AddUniform("Color", sizeof(vec3));
		lightBuffer->AddUniform("Position", sizeof(vec3));
		lightBuffer->AddUniform("Direction", sizeof(vec3));
		lightBuffer->AddUniform("AttenuationConstant", sizeof(float));
		lightBuffer->AddUniform("AttenuationLinear", sizeof(float));
		lightBuffer->AddUniform("AttenuationQuadratic", sizeof(float));
		lightBuffer->AddUniform("Angle", sizeof(float));
		lightBuffer->InitBuffer();
	}

	void Light::CreateUniformHandles()
	{
		ASSERT(lightBuffer, "Light Uniform Buffer is not initialized.");

		color = lightBuffer->MakeHandle<vec3>("Color");
		attenuationConstant = lightBuffer->MakeHandle<float>("AttenuationConstant");
		attenuationLinear = lightBuffer->MakeHandle<float>("AttenuationLinear");
		attenuationQuadratic = lightBuffer->MakeHandle<float>("AttenuationQuadratic");
		cosAngle = lightBuffer->MakeHandle<float>("Angle");
		position = lightBuffer->MakeHandle<vec3>("Position");
		direction = lightBuffer->MakeHandle<vec3>("Direction");
	}
}
