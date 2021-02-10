// Copyright (c) 2017 Emilian Cioca
#include "Camera.h"
#include "gemcutter/Application/Logging.h"
#include "gemcutter/Math/Math.h"
#include "gemcutter/Math/Vector.h"
#include "gemcutter/Rendering/Rendering.h"
#include "gemcutter/Rendering/Viewport.h"

namespace gem
{
	Camera::Camera(Entity& _owner)
		: Component(_owner)
	{
		CreateUniformBuffer();
		CreateUniformHandles();
		SetPerspective(fovyDegrees, aspectRatio, zNear, zFar);
	}

	Camera::Camera(Entity& _owner, float _fovyDegrees, float _aspectRatio, float _zNear, float _zFar)
		: Component(_owner)
	{
		CreateUniformBuffer();
		CreateUniformHandles();
		SetPerspective(_fovyDegrees, _aspectRatio, _zNear, _zFar);
	}

	Camera::Camera(Entity& _owner, float _left, float _right, float _top, float _bottom, float _zNear, float _zFar)
		: Component(_owner)
	{
		CreateUniformBuffer();
		CreateUniformHandles();
		SetOrthographic(_left, _right, _top, _bottom, _zNear, _zFar);
	}

	Camera& Camera::operator=(const Camera& other)
	{
		buffer.Copy(other.buffer);

		// Regenerate the handles.
		CreateUniformHandles();

		projection = other.projection;
		invProjection = other.invProjection;

		isPerspective = other.isPerspective;
		fovyDegrees = other.fovyDegrees;
		aspectRatio = other.aspectRatio;
		zNear = other.zNear;
		zFar = other.zFar;
		left = other.left;
		right = other.right;
		top = other.top;
		bottom = other.bottom;

		return *this;
	}

	void Camera::Bind() const
	{
		mat4 InvView = owner.GetWorldTransform();
		mat4 view = InvView.GetFastInverse();

		uniformView.Set(view);
		uniformViewProj.Set(projection * view);
		uniformInvView.Set(InvView);
		uniformCameraPosition.Set(InvView.GetTranslation());

		buffer.Bind(static_cast<unsigned>(UniformBufferSlot::Camera));
	}

	void Camera::UnBind()
	{
		UniformBuffer::UnBind(static_cast<unsigned>(UniformBufferSlot::Camera));
	}

	mat4 Camera::GetViewMatrix() const
	{
		return owner.GetWorldTransform().GetFastInverse();
	}

	mat4 Camera::GetViewProjMatrix() const
	{
		return projection * owner.GetWorldTransform().GetFastInverse();
	}

	mat4 Camera::GetProjMatrix() const
	{
		return projection;
	}

	mat4 Camera::GetInverseProjMatrix() const
	{
		return invProjection;
	}

	float Camera::GetNearPlaneWidth() const
	{
		// Get horizontal FOV.
		float tanFovy_over2 = tan(ToRadian(fovyDegrees) / 2.0f);
		float fovXRadians = 2.0f * atan(tanFovy_over2 * aspectRatio);

		float nearPlaneWidth = 2.0f * (zNear * tan(fovXRadians / 2.0f));

		return nearPlaneWidth;
	}

	float Camera::GetNearPlaneHeight() const
	{
		// Get horizontal FOV.
		float tanFovy_over2 = tan(ToRadian(fovyDegrees) / 2.0f);

		float nearPlaneHeight = 2.0f * (zNear * tanFovy_over2);

		return nearPlaneHeight;
	}

	float Camera::GetFarPlaneWidth() const
	{
		// Get horizontal FOV.
		float tanFovy_over2 = tan(ToRadian(fovyDegrees) / 2.0f);
		float fovXRadians = 2.0f * atan(tanFovy_over2 * aspectRatio);

		float farPlaneWidth = 2.0f * (zFar * tan(fovXRadians / 2.0f));

		return farPlaneWidth;
	}

	float Camera::GetFarPlaneHeight() const
	{
		// Get horizontal FOV.
		float tanFovy_over2 = tan(ToRadian(fovyDegrees) / 2.0f);

		float farPlaneHeight = 2.0f * (zFar * tanFovy_over2);

		return farPlaneHeight;
	}

	void Camera::GetFarPlaneCorners(vec3& bottomLeft, vec3& bottomRight, vec3& topRight, vec3& topLeft) const
	{
		// Get horizontal FOV.
		float tanFovy_over2 = tan(ToRadian(fovyDegrees) / 2.0f);
		float fovXRadians = 2.0f * atan(tanFovy_over2 * aspectRatio);

		float farPlaneHalfWidth = zFar * tan(fovXRadians / 2.0f);
		float farPlaneHalfHeight = zFar * tanFovy_over2;

		bottomLeft = vec3(-farPlaneHalfWidth, -farPlaneHalfHeight, -zFar);
		bottomRight = vec3(farPlaneHalfWidth, -farPlaneHalfHeight, -zFar);
		topRight = vec3(farPlaneHalfWidth, farPlaneHalfHeight, -zFar);
		topLeft = vec3(-farPlaneHalfWidth, farPlaneHalfHeight, -zFar);
	}

	void Camera::GetNearPlaneCorners(vec3& bottomLeft, vec3& bottomRight, vec3& topRight, vec3& topLeft) const
	{
		// Get horizontal FOV.
		float tanFovy_over2 = tan(ToRadian(fovyDegrees) / 2.0f);
		float fovXRadians = 2.0f * atan(tanFovy_over2 * aspectRatio);

		float nearPlaneHalfWidth = zNear * tan(fovXRadians / 2.0f);
		float nearPlaneHalfHeight = zNear * tanFovy_over2;

		bottomLeft = vec3(-nearPlaneHalfWidth, -nearPlaneHalfHeight, -zNear);
		bottomRight = vec3(nearPlaneHalfWidth, -nearPlaneHalfHeight, -zNear);
		topRight = vec3(nearPlaneHalfWidth, nearPlaneHalfHeight, -zNear);
		topLeft = vec3(-nearPlaneHalfWidth, nearPlaneHalfHeight, -zNear);
	}

	vec3 Camera::GetCenterOfVolume() const
	{
		vec3 farBottomLeft, farBottomRight, farTopRight, farTopLeft;
		vec3 nearBottomLeft, nearBottomRight, nearTopRight, nearTopLeft;

		GetFarPlaneCorners(farBottomLeft, farBottomRight, farTopRight, farTopLeft);
		GetNearPlaneCorners(nearBottomLeft, nearBottomRight, nearTopRight, nearTopLeft);

		return (farBottomLeft + farBottomRight + farTopRight + farTopLeft + nearBottomLeft + nearBottomRight + nearTopRight + nearTopLeft) / 8.0f;
	}

	void Camera::SetPerspective()
	{
		isPerspective = true;

		projection = mat4::PerspectiveProjection(fovyDegrees, aspectRatio, zNear, zFar);
		invProjection = mat4::InversePerspectiveProjection(fovyDegrees, aspectRatio, zNear, zFar);

		uniformProj.Set(projection);
		uniformInvProj.Set(invProjection);
	}

	void Camera::SetPerspective(float _fovyDegrees, float _aspectRatio, float _zNear, float _zFar)
	{
		isPerspective = true;
		fovyDegrees = _fovyDegrees;
		aspectRatio = _aspectRatio;
		zNear = _zNear;
		zFar = _zFar;

		projection = mat4::PerspectiveProjection(fovyDegrees, aspectRatio, zNear, zFar);
		invProjection = mat4::InversePerspectiveProjection(fovyDegrees, aspectRatio, zNear, zFar);

		uniformProj.Set(projection);
		uniformInvProj.Set(invProjection);
	}

	void Camera::SetOrthographic()
	{
		isPerspective = false;

		projection = mat4::OrthographicProjection(left, right, top, bottom, zNear, zFar);
		invProjection = mat4::InverseOrthographicProjection(left, right, top, bottom, zNear, zFar);

		uniformProj.Set(projection);
		uniformInvProj.Set(invProjection);
	}

	void Camera::SetOrthographic(float _left, float _right, float _top, float _bottom, float _zNear, float _zFar)
	{
		isPerspective = false;
		left = _left;
		right = _right;
		top = _top;
		bottom = _bottom;
		zNear = _zNear;
		zFar = _zFar;

		projection = mat4::OrthographicProjection(left, right, top, bottom, zNear, zFar);
		invProjection = mat4::InverseOrthographicProjection(left, right, top, bottom, zNear, zFar);

		uniformProj.Set(projection);
		uniformInvProj.Set(invProjection);
	}

	void Camera::SetOrthographic(const Viewport& viewport, float _zNear, float _zFar)
	{
		isPerspective = false;
		left = static_cast<float>(viewport.x);
		right = static_cast<float>(viewport.x + viewport.width);
		top = static_cast<float>(viewport.y + viewport.height);
		bottom = static_cast<float>(viewport.y);
		zNear = _zNear;
		zFar = _zFar;

		projection = mat4::OrthographicProjection(left, right, top, bottom, zNear, zFar);
		invProjection = mat4::InverseOrthographicProjection(left, right, top, bottom, zNear, zFar);

		uniformProj.Set(projection);
		uniformInvProj.Set(invProjection);
	}

	bool Camera::IsPerspective() const
	{
		return isPerspective;
	}

	bool Camera::IsOrthographic() const
	{
		return !isPerspective;
	}

	void Camera::SetFovy(float _fovyDegrees)
	{
		ASSERT(isPerspective, "Camera must be perspective to call this function.");

		fovyDegrees = _fovyDegrees;
		projection = mat4::PerspectiveProjection(fovyDegrees, aspectRatio, zNear, zFar);
		invProjection = mat4::InversePerspectiveProjection(fovyDegrees, aspectRatio, zNear, zFar);

		uniformProj.Set(projection);
		uniformInvProj.Set(invProjection);
	}

	void Camera::SetAspectRatio(float _aspectRatio)
	{
		ASSERT(isPerspective, "Camera must be perspective to call this function.");

		aspectRatio = _aspectRatio;
		projection = mat4::PerspectiveProjection(fovyDegrees, aspectRatio, zNear, zFar);
		invProjection = mat4::InversePerspectiveProjection(fovyDegrees, aspectRatio, zNear, zFar);

		uniformProj.Set(projection);
		uniformInvProj.Set(invProjection);
	}

	void Camera::SetNearPlane(float _zNear)
	{
		zNear = _zNear;

		if (isPerspective)
		{
			projection = mat4::PerspectiveProjection(fovyDegrees, aspectRatio, zNear, zFar);
			invProjection = mat4::InversePerspectiveProjection(fovyDegrees, aspectRatio, zNear, zFar);
		}
		else
		{
			projection = mat4::OrthographicProjection(left, right, top, bottom, zNear, zFar);
			invProjection = mat4::InverseOrthographicProjection(left, right, top, bottom, zNear, zFar);
		}

		uniformProj.Set(projection);
		uniformInvProj.Set(invProjection);
	}

	void Camera::SetFarPlane(float _zFar)
	{
		zFar = _zFar;

		if (isPerspective)
		{
			projection = mat4::PerspectiveProjection(fovyDegrees, aspectRatio, zNear, zFar);
			invProjection = mat4::InversePerspectiveProjection(fovyDegrees, aspectRatio, zNear, zFar);
		}
		else
		{
			projection = mat4::OrthographicProjection(left, right, top, bottom, zNear, zFar);
			invProjection = mat4::InverseOrthographicProjection(left, right, top, bottom, zNear, zFar);
		}

		uniformProj.Set(projection);
		uniformInvProj.Set(invProjection);
	}

	void Camera::SetPlanes(float _zNear, float _zFar)
	{
		zNear = _zNear;
		zFar = _zFar;

		if (isPerspective)
		{
			projection = mat4::PerspectiveProjection(fovyDegrees, aspectRatio, zNear, zFar);
			invProjection = mat4::InversePerspectiveProjection(fovyDegrees, aspectRatio, zNear, zFar);
		}
		else
		{
			projection = mat4::OrthographicProjection(left, right, top, bottom, zNear, zFar);
			invProjection = mat4::InverseOrthographicProjection(left, right, top, bottom, zNear, zFar);
		}

		uniformProj.Set(projection);
		uniformInvProj.Set(invProjection);
	}

	void Camera::SetLeftBound(float leftBound)
	{
		ASSERT(!isPerspective, "Camera must be orthographic to call this function.");

		left = leftBound;
		projection = mat4::OrthographicProjection(left, right, top, bottom, zNear, zFar);
		invProjection = mat4::InverseOrthographicProjection(left, right, top, bottom, zNear, zFar);

		uniformProj.Set(projection);
		uniformInvProj.Set(invProjection);
	}

	void Camera::SetRightBound(float rightBound)
	{
		ASSERT(!isPerspective, "Camera must be orthographic to call this function.");

		right = rightBound;
		projection = mat4::OrthographicProjection(left, right, top, bottom, zNear, zFar);
		invProjection = mat4::InverseOrthographicProjection(left, right, top, bottom, zNear, zFar);

		uniformProj.Set(projection);
		uniformInvProj.Set(invProjection);
	}

	void Camera::SetTopBound(float topBound)
	{
		ASSERT(!isPerspective, "Camera must be orthographic to call this function.");

		top = topBound;
		projection = mat4::OrthographicProjection(left, right, top, bottom, zNear, zFar);
		invProjection = mat4::InverseOrthographicProjection(left, right, top, bottom, zNear, zFar);

		uniformProj.Set(projection);
		uniformInvProj.Set(invProjection);
	}

	void Camera::SetBottomBound(float bottomBound)
	{
		ASSERT(!isPerspective, "Camera must be orthographic to call this function.");

		bottom = bottomBound;
		projection = mat4::OrthographicProjection(left, right, top, bottom, zNear, zFar);
		invProjection = mat4::InverseOrthographicProjection(left, right, top, bottom, zNear, zFar);

		uniformProj.Set(projection);
		uniformInvProj.Set(invProjection);
	}

	void Camera::SetBounds(const Viewport& viewport)
	{
		ASSERT(!isPerspective, "Camera must be orthographic to call this function.");

		left = static_cast<float>(viewport.x);
		right = static_cast<float>(viewport.x + viewport.width);
		top = static_cast<float>(viewport.y + viewport.height);
		bottom = static_cast<float>(viewport.y);

		projection = mat4::OrthographicProjection(left, right, top, bottom, zNear, zFar);
		invProjection = mat4::InverseOrthographicProjection(left, right, top, bottom, zNear, zFar);

		uniformProj.Set(projection);
		uniformInvProj.Set(invProjection);
	}

	float Camera::GetFovyDegrees() const
	{
		return fovyDegrees;
	}

	float Camera::GetAspectRatio() const
	{
		return aspectRatio;
	}

	float Camera::GetNearPlane() const
	{
		return zNear;
	}

	float Camera::GetFarPlane() const
	{
		return zFar;
	}

	float Camera::GetLeftBound() const
	{
		return left;
	}

	float Camera::GetRightBound() const
	{
		return right;
	}

	float Camera::GetTopBound() const
	{
		return top;
	}

	float Camera::GetBottomBound() const
	{
		return bottom;
	}

	void Camera::CreateUniformBuffer()
	{
		buffer.AddUniform<mat4>("View");
		buffer.AddUniform<mat4>("Proj");
		buffer.AddUniform<mat4>("ViewProj");
		buffer.AddUniform<mat4>("InvView");
		buffer.AddUniform<mat4>("InvProj");
		buffer.AddUniform<vec3>("CameraPosition");
		buffer.InitBuffer(VertexBufferUsage::Dynamic);
	}

	void Camera::CreateUniformHandles()
	{
		uniformView = buffer.MakeHandle<mat4>("View");
		uniformProj = buffer.MakeHandle<mat4>("Proj");
		uniformViewProj = buffer.MakeHandle<mat4>("ViewProj");
		uniformInvView = buffer.MakeHandle<mat4>("InvView");
		uniformInvProj = buffer.MakeHandle<mat4>("InvProj");
		uniformCameraPosition = buffer.MakeHandle<vec3>("CameraPosition");
	}
}
