// Copyright (c) 2017 Emilian Cioca
#pragma once
#include "Jewel3D/Entity/Entity.h"
#include "Jewel3D/Math/Matrix.h"
#include "Jewel3D/Resource/UniformBuffer.h"

namespace Jwl
{
	class vec3;
	class Viewport;

	class Camera : public Component<Camera>
	{
	public:
		Camera(Entity& owner);
		Camera(Entity& owner, f32 fovyDegrees, f32 aspectRatio, f32 zNear, f32 zFar);
		Camera(Entity& owner, f32 left, f32 right, f32 top, f32 bottom, f32 zNear, f32 zFar);
		Camera& operator=(const Camera&);

		//- Uploads the camera's values to OpenGL.
		void Bind() const;
		static void UnBind();

		//- Returns the inverse node pose, ready to use for rendering.
		mat4 GetViewMatrix() const;
		mat4 GetViewProjMatrix() const;
		mat4 GetProjMatrix() const;
		mat4 GetInverseProjMatrix() const;

		f32 GetNearPlaneWidth() const;
		f32 GetNearPlaneHeight() const;
		f32 GetFarPlaneWidth() const;
		f32 GetFarPlaneHeight() const;

		//- Computes the corners of the far plane in view-space.
		void GetFarPlaneCorners(vec3& bottomLeft, vec3& bottomRight, vec3& topRight, vec3& topLeft) const;
		//- Computes the corners of the near plane in view-space.
		void GetNearPlaneCorners(vec3& bottomLeft, vec3& bottomRight, vec3& topRight, vec3& topLeft) const;
		//- Returns the center of the viewing volume, compute as average of corners.
		vec3 GetCenterOfVolume() const;

		//- Sets the camera to Perspective mode with the current settings.
		void SetPerspective();
		//- Sets the camera to Perspective mode, overriding the settings.
		void SetPerspective(f32 fovyDegrees, f32 aspectRatio, f32 zNear, f32 zFar);

		//- Sets the camera to Orthographic mode with the current settings.
		void SetOrthograpic();
		//- Sets the camera to Orthographic mode, overriding the settings.
		void SetOrthograpic(f32 left, f32 right, f32 top, f32 bottom, f32 zNear, f32 zFar);
		//- Sets the camera to Orthographic mode, overriding the settings to match the viewport's dimensions.
		void SetOrthograpic(const Viewport& viewport, f32 zNear, f32 zFar);

		bool IsPerspective() const;
		bool IsOrthographic() const;

		void SetNearPlane(f32 zNear);
		void SetFarPlane(f32 zFar);
		void SetPlanes(f32 zNear, f32 zFar);

		/* For perspective projections */
		void SetFovy(f32 fovyDegrees);
		void SetAspectRatio(f32 aspectRatio);

		/* For orthographic projections */
		void SetLeftBound(f32 leftBound);
		void SetRightBound(f32 rightBound);
		void SetTopBound(f32 topBound);
		void SetBottomBound(f32 bottomBound);

		/* For perspective projections */
		f32 GetFovyDegrees() const;
		f32 GetAspectRatio() const;
		f32 GetNearPlane() const;
		f32 GetFarPlane() const;

		/* For orthographic projections */
		f32 GetLeftBound() const;
		f32 GetRightBound() const;
		f32 GetTopBound() const;
		f32 GetBottomBound() const;

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

		bool isPerspective	= true;
		f32 fovyDegrees	= 50.0f;
		f32 aspectRatio	= 1.0f;
		f32 zNear			= 1.0f;
		f32 zFar			= 1000.0f;
		f32 left			= -100.0f;
		f32 right			= 100.0f;
		f32 top			= 100.0f;
		f32 bottom		= -100.0f;
	};
}
