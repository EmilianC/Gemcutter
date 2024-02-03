Attributes
{
	vec4 a_vert  : 0;
	vec4 a_color : 1;
}

Vertex
{
	out vec4 color;

	void main()
	{
		color = a_color;
		gl_Position = Gem_MVP * a_vert;
	}
}

Fragment
{
	in vec4 color;
	out vec4 outColor;

	void main()
	{
		outColor = color;
	}
}
