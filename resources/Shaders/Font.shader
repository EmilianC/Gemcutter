Attributes
{
	vec4 a_vert : 0;
	vec2 a_uv   : 1;
}

Uniforms
{
	instance Material : 0
	{
		vec3 Color = (1.0, 1.0, 1.0);
	}
}

Vertex
{
	out vec2 texcoord;

	void main()
	{
		gl_Position = Gem_MVP * a_vert;
		texcoord = a_uv;
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
		outColor = vec4(Material.Color, texture(sTex, texcoord).r);

		#if defined(GEM_CUTOUT)
			if (outColor.a < 1.0)
			{
				discard;
			}
		#endif
	}
}