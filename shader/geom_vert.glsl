#version 330

uniform mat4 ModelMatrix;
uniform mat4 ViewProjectionMatrix;
uniform mat4 PreviousMVP;

/* vertex attrib arrays */
layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec3 Color;
layout (location = 3) in vec2 Texcoord;

/* varyings */
out vec3 Position0;
out vec3 Normal0;
out vec3 Color0;
out vec2 Texcoord0;

void main()
{
	Position0 = (ModelMatrix * vec4(Position, 1)).xyz;
	Normal0 = (ModelMatrix * vec4(Normal, 1)).xyz;
	Color0 = Color;
	Texcoord0 = Texcoord;
	gl_Position = ViewProjectionMatrix * vec4(Position0, 1);
}
