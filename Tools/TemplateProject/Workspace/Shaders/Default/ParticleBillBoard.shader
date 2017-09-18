Attributes
{
	vec4 a_vert		 : 0;
	vec2 a_size		 : 1;
	vec3 a_color	 : 2;
	float a_alpha	 : 3;
	float a_rotation : 4;
	float a_ageRatio : 5;
}

Uniforms
{
	Properties : 14
	{
		vec2 StartSize;
		vec2 EndSize;
		vec3 StartColor;
		vec3 EndColor;
		float StartAlpha;
		float EndAlpha;
	}
}

Vertex
{
	#if defined(JWL_PARTICLE_SIZE)
		out vec2 size;
	#endif
	#if defined(JWL_PARTICLE_COLOR)
		out vec3 color;
	#endif
	#if defined(JWL_PARTICLE_ALPHA)
		out float alpha;
	#endif
	#if defined(JWL_PARTICLE_ROTATION)
		out float rotation;
	#endif
	#if defined(JWL_PARTICLE_AGERATIO)
		out float ageRatio;
	#endif

	void main()
	{
		#if defined(JWL_PARTICLE_LOCAL_SPACE)
			gl_Position = Jwl_ModelView * a_vert;
		#else
			gl_Position = Jwl_View * a_vert;
		#endif

		#if defined(JWL_PARTICLE_SIZE)
			size = a_size;
		#endif
		#if defined(JWL_PARTICLE_COLOR)
			color = a_color;
		#endif
		#if defined(JWL_PARTICLE_ALPHA)
			alpha = a_alpha;
		#endif
		#if defined(JWL_PARTICLE_ROTATION)
			rotation = a_rotation;
		#endif
		#if defined(JWL_PARTICLE_AGERATIO)
			ageRatio = a_ageRatio;
		#endif
	}
}

Geometry
{
	// Input type.
	layout(points) in;

	// Output type.
	layout(triangle_strip, max_vertices = 4) out;

	#if defined(JWL_PARTICLE_SIZE)
		in vec2 size[];
	#endif
	#if defined(JWL_PARTICLE_COLOR)
		in vec3 color[];
	#endif
	#if defined(JWL_PARTICLE_ALPHA)
		in float alpha[];
	#endif
	#if defined(JWL_PARTICLE_ROTATION)
		in float rotation[];
	#endif
	#if defined(JWL_PARTICLE_AGERATIO)
		in float ageRatio[];
	#endif

	out vec2 texcoord;
	out vec4 frag_color;

	vec2 OFFSETS[4] = vec2[](
		vec2(-0.5, -0.5),
		vec2(0.5, -0.5),
		vec2(-0.5, 0.5),
		vec2(0.5, 0.5));

	vec2 TEXCOORDS[4] = vec2[](
		vec2(0.0, 0.0),
		vec2(1.0, 0.0),
		vec2(0.0, 1.0),
		vec2(1.0, 1.0));

	void main()
	{
		vec3 position = gl_in[0].gl_Position.xyz;
		vec4 outPosition = vec4(0.0, 0.0, position.z, 1.0);

		/* Resolve Particle Parameters */
		vec2 Size;
		#if defined(JWL_PARTICLE_SIZE)
			Size = size[0];
		#else
			Size = mix(Properties.StartSize, Properties.EndSize, ageRatio[0]);
		#endif

		vec4 Color;
		#if defined(JWL_PARTICLE_COLOR)
			Color.rgb = color[0];
		#else
			Color.rgb = mix(Properties.StartColor, Properties.EndColor, ageRatio[0]);
		#endif

		#if defined(JWL_PARTICLE_ALPHA)
			Color.a = alpha[0];
		#else
			Color.a = mix(Properties.StartAlpha, Properties.EndAlpha, ageRatio[0]);
		#endif

		// Create rotation matrix.
		#if defined(JWL_PARTICLE_ROTATION)
			float cosR = cos(rotation[0]);
			float sinR = sin(rotation[0]);
			mat2 rot = mat2(cosR, -sinR, sinR, cosR);
		#endif

		/* Emit Vertices */
		for (int i = 0; i < 4; ++i)
		{
			vec2 offset = OFFSETS[i] * Size;
			#if defined(JWL_PARTICLE_ROTATION)
				offset = rot * offset;
			#endif
			outPosition.xy = position.xy + offset;
			gl_Position = Jwl_Proj * outPosition;

			frag_color = Color;
			texcoord = TEXCOORDS[i];
			EmitVertex();
		}

		EndPrimitive();
	}
}

Samplers
{
	sampler2D sTex : 0;
}

Fragment
{
	in vec2 texcoord;
	in vec4 frag_color;

	out vec4 outColor;

	void main()
	{
		outColor = texture(sTex, texcoord).rgba;

		#if defined(JWL_CUTOUT)
			if (outColor.a < 1.0)
			{
				discard;
			}
		#endif

		outColor.rgb *= frag_color.rgb;
		outColor *= frag_color.a;
	}
}