Attributes
{
	vec4 a_vert : 0;
}

Uniforms
{
	instance Material : 0
	{
		vec4 Color = (0.0, 0.0, 0.0, 1.0);
	}
}

Vertex
{
	void main()
	{
		gl_Position = Jwl_MVP * a_vert;
	}
}

Fragment
{
	out vec4 outColor;

	void main()
	{
		outColor = Material.Color;
	}
}
