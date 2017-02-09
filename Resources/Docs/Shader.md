# Shader Outlines
Although Jewel3D uses GLSL as its core shading language, shaders in Jewel3D are written as a single, complete, program outline.
Shader Outlines allow you to easily describe your own custom data bindings without having to fumble with C++ or OpenGL.

```cpp
// Passthrough.shader
Attributes
{
	vec4 a_vert	: 0;
	vec2 a_uv   : 1;
}

Vertex
{
	out vec2 texcoord;

	void main()
	{
		gl_Position = Jwl_MVP * a_vert;
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
		outColor = texture(sTex, texcoord);
	}
}
```

# Shader Blocks
All key sections of the shader are described in their own blocks.

* ```Attributes```
	* Describes input mesh data and bindings
* ```Vertex```
	* A standard GLSL vertex shader
* ```Geometry```
	* A standard GLSL geometry shader
* ```Fragment```
	* A standard GLSL fragment shader
* ```Uniforms```
	* Describes uniform buffer data and bindings
* ```Samplers```
	* Describes textures samplers and bindings
	
If a ```Vertex``` block is not defined, a passthrough is automatically created containing the same ```Attributes``` and ```Vertex``` code from the example above.
This allows you to easily create post-process shaders. A ```Fragment``` block can also be omitted and will be substituted with the ```Samplers``` and ```Fragment``` code from the example above.
	
# Uniforms
A Uniform block can itself contain multiple Uniform Buffers. A Uniform Buffer is simply a structure of data bound to a specific ID.
The actual values of a Uniform Buffer are managed on the CPU with the ```Jwl::UniformBuffer``` resource.

A Uniform Buffer can be qualified as ```static```, ```template```, or both. If a Uniform Buffer is either ```static``` or ```template```, its variables can be assigned default values inline.

A ```static``` buffer is global to the shader. It is owned by the shader resource itself and is initialized the first time the shader is loaded.
A buffer created this way can be found in the shader's ```buffers``` data member after loading. The buffer will assume any default values specified inline.

When a Uniform Buffer is qualified as ```template``` it can be reflected to C++ code by calling ```Jwl::Shader::CreateBufferFromTemplate(unsigned)``` with its binding ID.
A uniform buffer created this way will assume any default values.

```Jwl::UniformBuffer```'s and ```Texture```'s can be attached to either the shader resource for global control of the shader's behaviour, or they can
be attached to the Material Component for per-object control. When rendering, Jewel3D will bind all Textures and Buffers from the currently rendering Shader and Material.
If both the shader and Material bind a buffer or texture to the same ID, the Material's per-object data is used.

# System Functions and Uniforms
A series of matrices are available for all GLSL shader blocks.
```cpp
mat4 Jwl_View;
mat4 Jwl_Proj;
mat4 Jwl_ViewProj;
mat4 Jwl_InvView;
mat4 Jwl_InvProj;
mat4 Jwl_MVP;
mat4 Jwl_ModelView;
mat4 Jwl_Model;
mat4 Jwl_InvModel;
```

A series of lighting functions are also available for all GLSL shader blocks.
Values are assumed to be in view space. The first parameter should be a Light's Uniform Buffer.
These functions will respect the light's attenuation values.
```cpp
vec3 COMPUTE_POINT_LIGHT(Light light, vec3 normal, vec3 pos);
vec3 COMPUTE_DIRECTIONAL_LIGHT(Light light, vec3 normal);
vec3 COMPUTE_SPOT_LIGHT(Light light, vec3 normal, vec3 pos);
```

# Lighting Example
```cpp
// Lambert.shader
Attributes
{
	vec4 a_vert		: 0;
	vec2 a_uv		: 1;
	vec3 a_normal	: 2;
}

Uniforms
{
	// Default light structure.
	// Populated automatically by the Light Component.
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

	// Static values owned globally by the shader.
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
		// Save the viewspace position for per-pixel lighting.
		pos = (Jwl_ModelView * a_vert).xyz;
		// Complete the transformation for the vertex.
		gl_Position = Jwl_Proj * vec4(pos, 1.0);

		texcoord = a_uv;
		// Rotate the normal into viewspace.
		norm = mat3(Jwl_ModelView) * a_normal;
	}
}

Samplers
{
	sampler2D sTex : 0;
}

Fragment
{
	in vec2 texcoord;
	in vec3 norm;
	in vec3 pos;

	out vec3 outColor;

	void main()
	{
		// Re-normalize after rasterization.
		vec3 normal = normalize(norm);

		// Built-in lighting functions respect all light parameters.
		vec3 lighting = Ambient.Color;
		lighting += COMPUTE_POINT_LIGHT(Light, normal, pos);

		// Combine lighting with the diffuse texture.
		outColor = texture(sTex, texcoord).rgb * lighting;
	}
}
```

# Variants
A single Jewel3D shader resource can actualy represent different compiled variants of the original shader source. Such shaders are often refered to as "Uber Shaders".
This allows you to switch the behaviour of your shaders dynamically without having to juggle many different shader instances.

Shader variants in Jewel3D are managed by the ```Jwl::ShaderVariantControl variantDefinitions``` member of the Material Component.

```cpp
auto& material = entity.Get<Material>();

// #define Use_Feature_X
material.variantDefinitions.Define("Use_Feature_X");

// #define BlurAmount 3
material.variantDefinitions.Define("BlurAmount", 3);

// Removes #define Use_Feature_X
material.variantDefinitions.Undefine("Use_Feature_X");

// BlendFunc::CutOut also notifies the shader with a system define.
// #define JWL_CUTOUT
material.SetBlendMode(BlendFunc::CutOut);
```

# Notes
* Uniform Buffer values are seamlessly used between all variants of a shader. 
* A particular shader variant is only compiled the first time that it is used.
