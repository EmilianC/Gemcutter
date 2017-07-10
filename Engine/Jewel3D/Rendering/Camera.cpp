// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "Camera.h"
#include "Jewel3D/Application/Logging.h"
#include "Jewel3D/Math/Math.h"
#include "Jewel3D/Math/Transform.h"
#include "Jewel3D/Math/Vector.h"
#include "Jewel3D/Rendering/Rendering.h"
#include "Jewel3D/Rendering/Viewport.h"

namespace Jwl
{
	Camera::Camera(Entity& _owner)
		: Component(_owner)
	{
		CreateUniformBuffer();
		CreateUniformHandles();
		SetPerspective(fovyDegrees, aspectRatio, zNear, zFar);
	}

	Camera::Camera(Entity& _owner, f32 _fovyDegrees, f32 _aspectRatio, f32 _zNear, f32 _zFar)
		: Component(_owner)
	{
		CreateUniformBuffer();
		CreateUniformHandles();
		SetPerspective(_fovyDegrees, _aspectRatio, _zNear, _zFar);
	}

	Camera::Camera(Entity& _owner, f32 _left, f32 _right, f32 _top, f32 _bottom, f32 _zNear, f32 _zFar)
		: Component(_owner)
	{
		CreateUniformBuffer();
		CreateUniformHandles();
		SetOrthograpic(_left, _right, _top, _bottom, _zNear, _zFar);
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

		buffer.Bind(static_cast<u32>(UniformBufferSlot::Camera));
	}

	void Camera::UnBind()
	{
		UniformBuffer::UnBind(static_cast<u32>(UniformBufferSlot::Camera));
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

	f32 Camera::GetNearPlaneWidth() const
	{
		// Get horizontal FOV.
		f32 tanFovy_over2 = tan(ToRadian(fovyDegrees) / 2.0f);
		f32 fovXRadians = 2.0f * atan(tanFovy_over2 * aspectRatio);

		f32 nearPlaneWidth = 2.0f * (zNear * tan(fovXRadians / 2.0f));

		return nearPlaneWidth;
	}

	f32 Camera::GetNearPlaneHeight() const
	{
		// Get horizontal FOV.
		f32 tanFovy_over2 = tan(ToRadian(fovyDegrees) / 2.0f);

		f32 nearPlaneHeight = 2.0f * (zNear * tanFovy_over2);

		return nearPlaneHeight;
	}

	f32 Camera::GetFarPlaneWidth() const
	{
		// Get horizontal FOV.
		f32 tanFovy_over2 = tan(ToRadian(fovyDegrees) / 2.0f);
		f32 fovXRadians = 2.0f * atan(tanFovy_over2 * aspectRatio);

		f32 farPlaneWidth = 2.0f * (zFar * tan(fovXRadians / 2.0f));

		return farPlaneWidth;
	}

	f32 Camera::GetFarPlaneHeight() const
	{
		// Get horizontal FOV.
		f32 tanFovy_over2 = tan(ToRadian(fovyDegrees) / 2.0f);

		f32 farPlaneHeight = 2.0f * (zFar * tanFovy_over2);

		return farPlaneHeight;
	}

	void Camera::GetFarPlaneCorners(vec3& bottomLeft, vec3& bottomRight, vec3& topRight, vec3& topLeft) const
	{
		// Get horizontal FOV.
		f32 tanFovy_over2 = tan(ToRadian(fovyDegrees) / 2.0f);
		f32 fovXRadians = 2.0f * atan(tanFovy_over2 * aspectRatio);

		f32 farPlaneHalfWidth = zFar * tan(fovXRadians / 2.0f);
		f32 farPlaneHalfHeight = zFar * tanFovy_over2;

		bottomLeft = vec3(-farPlaneHalfWidth, -farPlaneHalfHeight, -zFar);
		bottomRight = vec3(farPlaneHalfWidth, -farPlaneHalfHeight, -zFar);
		topRight = vec3(farPlaneHalfWidth, farPlaneHalfHeight, -zFar);
		topLeft = vec3(-farPlaneHalfWidth, farPlaneHalfHeight, -zFar);
	}

	void Camera::GetNearPlaneCorners(vec3& bottomLeft, vec3& bottomRight, vec3& topRight, vec3& topLeft) const
	{
		// Get horizontal FOV.
		f32 tanFovy_over2 = tan(ToRadian(fovyDegrees) / 2.0f);
		f32 fovXRadians = 2.0f * atan(tanFovy_over2 * aspectRatio);

		f32 nearPlaneHalfWidth = zNear * tan(fovXRadians / 2.0f);
		f32 nearPlaneHalfHeight = zNear * tanFovy_over2;

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

	void Camera::SetPerspective(f32 _fovyDegrees, f32 _aspectRatio, f32 _zNear, f32 _zFar)
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

	void Camera::SetOrthograpic()
	{
		isPerspective = false;

		projection = mat4::OrthographicProjection(left, right, top, bottom, zNear, zFar);
		invProjection = mat4::InverseOrthographicProjection(left, right, top, bottom, zNear, zFar);

		uniformProj.Set(projection);
		uniformInvProj.Set(invProjection);
	}

	void Camera::SetOrthograpic(f32 _left, f32 _right, f32 _top, f32 _bottom, f32 _zNear, f32 _zFar)
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

	void Camera::SetOrthograpic(const Viewport& viewport, f32 _zNear, f32 _zFar)
	{
		isPerspective = false;
		left = static_cast<f32>(viewport.x);
		right = static_cast<f32>(viewport.x + viewport.width);
		top = static_cast<f32>(viewport.y + viewport.height);
		bottom = static_cast<f32>(viewport.y);
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

	void Camera::SetFovy(f32 _fovyDegrees)
	{
		ASSERT(isPerspective, "Camera must be perspective to call this function.");

		fovyDegrees = _fovyDegrees;
		projection = mat4::PerspectiveProjection(fovyDegrees, aspectRatio, zNear, zFar);
		invProjection = mat4::InversePerspectiveProjection(fovyDegrees, aspectRatio, zNear, zFar);

		uniformProj.Set(projection);
		uniformInvProj.Set(invProjection);
	}

	void Camera::SetAspectRatio(f32 _aspectRatio)
	{
		ASSERT(isPerspective, "Camera must be perspective to call this function.");

		aspectRatio = _aspectRatio;
		projection = mat4::PerspectiveProjection(fovyDegrees, aspectRatio, zNear, zFar);
		invProjection = mat4::InversePerspectiveProjection(fovyDegrees, aspectRatio, zNear, zFar);

		uniformProj.Set(projection);
		uniformInvProj.Set(invProjection);
	}

	void Camera::SetNearPlane(f32 _zNear)
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

	void Camera::SetFarPlane(f32 _zFar)
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

	void Camera::SetPlanes(f32 _zNear, f32 _zFar)
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

	void Camera::SetLeftBound(f32 leftBound)
	{
		ASSERT(!isPerspective, "Camera must be orthographic to call this function.");

		left = leftBound;
		projection = mat4::OrthographicProjection(left, right, top, bottom, zNear, zFar);
		invProjection = mat4::InverseOrthographicProjection(left, right, top, bottom, zNear, zFar);

		uniformProj.Set(projection);
		uniformInvProj.Set(invProjection);
	}

	void Camera::SetRightBound(f32 rightBound)
	{
		ASSERT(!isPerspective, "Camera must be orthographic to call this function.");

		right = rightBound;
		projection = mat4::OrthographicProjection(left, right, top, bottom, zNear, zFar);
		invProjection = mat4::InverseOrthographicProjection(left, right, top, bottom, zNear, zFar);

		uniformProj.Set(projection);
		uniformInvProj.Set(invProjection);
	}

	void Camera::SetTopBound(f32 topBound)
	{
		ASSERT(!isPerspective, "Camera must be orthographic to call this function.");

		top = topBound;
		projection = mat4::OrthographicProjection(left, right, top, bottom, zNear, zFar);
		invProjection = mat4::InverseOrthographicProjection(left, right, top, bottom, zNear, zFar);

		uniformProj.Set(projection);
		uniformInvProj.Set(invProjection);
	}

	void Camera::SetBottomBound(f32 bottomBound)
	{
		ASSERT(!isPerspective, "Camera must be orthographic to call this function.");

		bottom = bottomBound;
		projection = mat4::OrthographicProjection(left, right, top, bottom, zNear, zFar);
		invProjection = mat4::InverseOrthographicProjection(left, right, top, bottom, zNear, zFar);

		uniformProj.Set(projection);
		uniformInvProj.Set(invProjection);
	}

	f32 Camera::GetFovyDegrees() const
	{
		return fovyDegrees;
	}

	f32 Camera::GetAspectRatio() const
	{
		return aspectRatio;
	}

	f32 Camera::GetNearPlane() const
	{
		return zNear;
	}

	f32 Camera::GetFarPlane() const
	{
		return zFar;
	}

	f32 Camera::GetLeftBound() const
	{
		return left;
	}

	f32 Camera::GetRightBound() const
	{
		return right;
	}

	f32 Camera::GetTopBound() const
	{
		return top;
	}

	f32 Camera::GetBottomBound() const
	{
		return bottom;
	}

	void Camera::CreateUniformBuffer()
	{
		buffer.AddUniform("View", sizeof(mat4));
		buffer.AddUniform("Proj", sizeof(mat4));
		buffer.AddUniform("ViewProj", sizeof(mat4));
		buffer.AddUniform("InvView", sizeof(mat4));
		buffer.AddUniform("InvProj", sizeof(mat4));
		buffer.InitBuffer();
	}

	void Camera::CreateUniformHandles()
	{
		uniformView = buffer.MakeHandle<mat4>("View");
		uniformProj = buffer.MakeHandle<mat4>("Proj");
		uniformViewProj = buffer.MakeHandle<mat4>("ViewProj");
		uniformInvView = buffer.MakeHandle<mat4>("InvView");
		uniformInvProj = buffer.MakeHandle<mat4>("InvProj");
	}
}
