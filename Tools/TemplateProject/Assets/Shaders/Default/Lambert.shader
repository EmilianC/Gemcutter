Attributes
{
	vec4 a_vert		: 0;
	vec2 a_uv		: 1;
	vec3 a_normal	: 2;
}

Samplers
{
	sampler2D sTex : 0;
}

Uniforms
{
	Light : 0
	{
		vec3 Color;
		vec3 Position;
		vec3 Direction;
		float AttenuationConstant;
		float AttenuationLinear;
		float AttenuationQuadratic;
		float Angle;
	}

	template static Ambient : 1
	{
		vec3 Color = (0.1, 0.1, 0.1);
	}
}

Vertex
{
	out vec2 texcoord;
	out vec3 norm;
	out vec3 pos;

	void main()
	{
		pos = (Jwl_ModelView * a_vert).xyz;
		gl_Position = Jwl_Proj * vec4(pos, 1.0);

		texcoord = a_uv;
		norm = mat3(Jwl_ModelView) * a_normal;
	}
}

Fragment
{
	in vec2 texcoord;
	in vec3 norm;
	in vec3 pos;

	out vec3 outColor;

	void main()
	{
		vec3 normal = normalize(norm);

		vec3 lighting = Ambient.Color;
		lighting += COMPUTE_POINT_LIGHT(Light, normal, pos);

		outColor = texture(sTex, texcoord).rgb * lighting;
	}
}