Attributes
{
	vec3 a_vert : 0;
}

Vertex
{
	out vec3 texcoord;

	void main()
	{
		texcoord = a_vert;
		// After the perspective divide, Z will be very close to 1.0
		gl_Position = (Gem_ViewProj * vec4(a_vert, 0.0)).xyww;
		gl_Position.w *= 1.0001;
	}
}

Samplers
{
	samplerCube sTex : 0;
}

Fragment
{
	in vec3 texcoord;
	out vec4 outColor;

	void main()
	{
		outColor = texture(sTex, texcoord);
	}
}
