#version 330

uniform mat4 ModelViewProjectionMatrix;

layout (location = 0) in vec3 Position;

void main()
{
	gl_Position = ModelViewProjectionMatrix * vec4(Position, 1);
}
