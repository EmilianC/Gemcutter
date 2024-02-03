Attributes
{
	vec4 a_vert : 0;
	vec2 a_uv   : 1;
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
	out vec3 outColor;

	void main()
	{
		outColor = texture(sTex, texcoord).rgb;
	}
}
