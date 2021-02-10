# Shader Outlines
Although Gemcutter uses GLSL as its core shading language, shaders in Gemcutter are written as a single, complete, program outline.
Shader outlines allow you to easily describe your own custom data bindings without having to fumble with C++ or OpenGL.

```cpp
// Passthrough.shader
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
The actual values of a Uniform Buffer are managed by game code with the ```gem::UniformBuffer``` resource. Data will assume any default values specified inline for ```instance``` and ```static``` buffers.

A Uniform Buffer qualified as ```static``` will be shared by all instances of a shader. It is owned by the shader resource itself and is initialized the first time the shader is loaded.
A buffer created this way can be found in the shader's ```buffers``` data member after loading.

Buffers qualified as ```instance``` are instantiated per-entity and can be found in the Renderable Component's ```buffers``` data member.

```gem::UniformBuffer```'s can be attached to either the shader for global control of the shader's behaviour, or they can
be attached to the Renderable for per-object control. When rendering, Gemcutter will bind all Textures and Buffers from the currently rendering Shader, Material, and Renderable.
If both the shader and object bind a buffer or texture to the same ID, the object's data is used.

# System Uniforms, Functions, and Variables
A series of uniforms are available for all GLSL shader blocks.
```cpp
mat4 Gem_View;
mat4 Gem_Proj;
mat4 Gem_ViewProj;
mat4 Gem_InvView;
mat4 Gem_InvProj;
vec3 Gem_CameraPosition; // World space.

mat4 Gem_MVP;
mat4 Gem_ModelView;
mat4 Gem_Model;
mat4 Gem_InvModel;
mat3 Gem_NormalToWorld;
```

The following functions are available to use in all GLSL shader blocks.
```cpp
// Converts between color spaces.
float sRGB_to_linear(float x);
vec3 sRGB_to_linear(vec3 v);

float linear_to_sRGB(float x);
vec3 linear_to_sRGB(vec3 v);

// Constructs the Tangent/Bitangent/Normal matrix for use with normal-maps.
// You can pass the standard model attributes as arguments (a_normal, a_tangent.xyz, a_tangent.w).
mat3 make_TBN(vec3 normal, vec3 tangent, float handedness);

// Computes the surface contribution from any type of light and its parameters.
// The first parameter should be a Light's Uniform Buffer.
// 'normal' and 'pos' should be world-space vectors describing the surface.
vec3 compute_light(Light light, vec3 normal, vec3 pos);

bool is_point_light(Light light);
bool is_directional_light(Light light);
bool is_spot_light(Light light);
```

The standard math definitions from `gemcutter/Math/Math.h` are also defined in all GLSL shader blocks.
```cpp
#define M_PI     3.14159265358979323846
#define M_E      2.71828182845904523536
#define M_LOG2E  1.44269504088896340736
#define M_LOG10E 0.434294481903251827651
#define M_LN2    0.693147180559945309417
#define M_LN10   2.30258509299404568402
```

# Lighting Example
```cpp
// Lambert.shader
Attributes
{
	vec4 a_vert   : 0;
	vec2 a_uv     : 1;
	vec3 a_normal : 2;
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
		float AttenuationLinear;
		float AttenuationQuadratic;
		float Angle;
		uint Type;
	}

	// Static values owned globally by the shader.
	static Ambient : 1
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
		// Save the world-space position for per-pixel lighting.
		pos = (Gem_Model * a_vert).xyz;
		// Complete the transformation for the vertex.
		gl_Position = Gem_ViewProj * vec4(pos, 1.0);

		texcoord = a_uv;
		// Rotate the normal into world-space.
		norm = Gem_NormalToWorld * a_normal;
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

		// The built-in lighting function respects all light parameters.
		vec3 lighting = Ambient.Color;
		lighting += compute_light(Light, normal, pos);

		// Combine lighting with the diffuse texture.
		outColor = texture(sTex, texcoord).rgb * lighting;
	}
}
```

# Variants
A single Gemcutter shader resource can actually represent different compiled variants of the original shader source. Such shaders are often referred to as "Uber Shaders".
This allows you to switch the behaviour of your shaders dynamically without having to juggle many different shader instances.

Shader variants in Gemcutter are managed by the ```gem::ShaderVariantControl variants``` member of a Renderable Component.

```cpp
auto& renderable = entity.Get<Renderable>();

// #define Use_Feature_X
renderable.variants.Define("Use_Feature_X");

// #define BlurAmount 3
renderable.variants.Define("BlurAmount", 3);

// #define GEM_CUTOUT
// Enables alpha threshold discarding for some of the default shaders
renderable.variants.Define("GEM_CUTOUT");

// Removes #define Use_Feature_X
renderable.variants.Undefine("Use_Feature_X");
```

# sRGB Conversions
It is recommended to use sRGB textures and to composite your final scene into a RenderTarget with an sRGB color buffer.
This will preserve the color balance of your original textures and will improve the accuracy of lighting effects.

When reading from an sRGB texture in a shader the texture fetch will automatically convert the color to linear space.
When rendering to a RenderTarget with an sRGB color buffer, a linear->sRGB conversion will automatically be done for you.

However, when rendering directly to the backbuffer, a linear->sRGB conversion does NOT take place and textures will appear too dark.
In this case, you can call linear_to_sRGB() on your shader's output color.

# Notes
* Uniform Buffer values are seamlessly used between all variants of a shader.
* A particular shader variant is only compiled the first time that it is used.
