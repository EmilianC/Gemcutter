Attributes
{
	vec4 a_vert : 0;
	vec2 a_uv   : 1;
}

Uniforms
{
	instance Material : 0
	{
		float AlphaCutOff = 0.5;
	}
}

Vertex
{
	out vec2 texcoord;

	void main()
	{
		texcoord = a_uv;

		#if defined(JWL_SPRITE_BILLBOARD)
			// Get the position of the sprite.
			vec4 vertex = Jwl_Model[3].xyzw;

			// Calculate vertex offset from the center.
			vec2 offset = texcoord;
			#if defined(JWL_SPRITE_CENTERED_X)
			offset.x -= 0.5;
			#endif
			#if defined(JWL_SPRITE_CENTERED_Y)
			offset.y -= 0.5;
			#endif

			// Extract the scale from the basis vectors.
			vec2 scale = vec2(length(Jwl_Model[0].xyz), length(Jwl_Model[1].xyz));

			// Offset in view space to create perfect billboard.
			vertex = Jwl_View * vertex;
			vertex.xy += offset * scale;

			// Project to screen.
			gl_Position = Jwl_Proj * vertex;
		#else
			vec4 vertex = a_vert;

			// Offset in local-space.
			#if defined(JWL_SPRITE_CENTERED_X)
			vertex.x -= 0.5;
			#endif
			#if defined(JWL_SPRITE_CENTERED_Y)
			vertex.y -= 0.5;
			#endif

			// Transform and project to screen.
			gl_Position = Jwl_MVP * vertex;
		#endif
	}
}

Samplers
{
	sampler2D sTex : 0;
}

Fragment
{
	in vec2 texcoord;

	out vec4 outColor;

	void main()
	{
		outColor = texture(sTex, texcoord).rgba;

		#if defined(JWL_CUTOUT)
			if (outColor.a <= Material.AlphaCutOff)
			{
				discard;
			}
		#endif
	}
}