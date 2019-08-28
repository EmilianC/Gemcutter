// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Jewel3D/Entity/Entity.h"
#include "Jewel3D/Math/Matrix.h"
#include "Jewel3D/Math/Vector.h"
#include "Jewel3D/Resource/UniformBuffer.h"

namespace Jwl
{
	struct Viewport;

	class Camera : public Component<Camera>
	{
	public:
		Camera(Entity& owner);
		Camera(Entity& owner, float fovyDegrees, float aspectRatio, float zNear, float zFar);
		Camera(Entity& owner, float left, float right, float top, float bottom, float zNear, float zFar);
		Camera& operator=(const Camera&);

		// Uploads the camera's values to OpenGL.
		void Bind() const;
		static void UnBind();

		// Returns the inverse node pose, ready to use for rendering.
		mat4 GetViewMatrix() const;
		mat4 GetViewProjMatrix() const;
		mat4 GetProjMatrix() const;
		mat4 GetInverseProjMatrix() const;

		float GetNearPlaneWidth() const;
		float GetNearPlaneHeight() const;
		float GetFarPlaneWidth() const;
		float GetFarPlaneHeight() const;

		// Computes the corners of the far plane in view-space.
		void GetFarPlaneCorners(vec3& bottomLeft, vec3& bottomRight, vec3& topRight, vec3& topLeft) const;
		// Computes the corners of the near plane in view-space.
		void GetNearPlaneCorners(vec3& bottomLeft, vec3& bottomRight, vec3& topRight, vec3& topLeft) const;
		// Returns the center of the viewing volume, compute as average of corners.
		vec3 GetCenterOfVolume() const;

		// Sets the camera to Perspective mode with the current settings.
		void SetPerspective();
		// Sets the camera to Perspective mode, overriding the settings.
		void SetPerspective(float fovyDegrees, float aspectRatio, float zNear, float zFar);

		// Sets the camera to Orthographic mode with the current settings.
		void SetOrthographic();
		// Sets the camera to Orthographic mode, overriding the settings.
		void SetOrthographic(float left, float right, float top, float bottom, float zNear, float zFar);
		// Sets the camera to Orthographic mode, overriding the settings to match the viewport's dimensions.
		void SetOrthographic(const Viewport& viewport, float zNear, float zFar);

		bool IsPerspective() const;
		bool IsOrthographic() const;

		void SetNearPlane(float zNear);
		void SetFarPlane(float zFar);
		void SetPlanes(float zNear, float zFar);

		/* For perspective projections */
		void SetFovy(float fovyDegrees);
		void SetAspectRatio(float aspectRatio);

		/* For orthographic projections */
		void SetLeftBound(float leftBound);
		void SetRightBound(float rightBound);
		void SetTopBound(float topBound);
		void SetBottomBound(float bottomBound);
		void SetBounds(const Viewport& viewport);

		/* For perspective projections */
		float GetFovyDegrees() const;
		float GetAspectRatio() const;
		float GetNearPlane() const;
		float GetFarPlane() const;

		/* For orthographic projections */
		float GetLeftBound() const;
		float GetRightBound() const;
		float GetTopBound() const;
		float GetBottomBound() const;

	private:
		void CreateUniformBuffer();
		void CreateUniformHandles();

		mutable UniformBuffer buffer;
		mat4 projection;
		mat4 invProjection;

		mutable UniformHandle<mat4> uniformView;
		mutable UniformHandle<mat4> uniformProj;
		mutable UniformHandle<mat4> uniformViewProj;
		mutable UniformHandle<mat4> uniformInvView;
		mutable UniformHandle<mat4> uniformInvProj;
		mutable UniformHandle<vec3> uniformCameraPosition;

		bool isPerspective = true;
		float fovyDegrees = 50.0f;
		float aspectRatio = 1.0f;
		float zNear = 1.0f;
		float zFar = 1000.0f;
		float left = -100.0f;
		float right = 100.0f;
		float top = 100.0f;
		float bottom = -100.0f;
	};
}
