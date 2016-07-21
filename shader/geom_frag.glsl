#version 330

in vec3 Position0;
in vec3 Normal0;
in vec3 Color0;
in vec2 Texcoord0;

/* fb attachments */
layout (location = 0) out vec3 PositionOut;
layout (location = 1) out vec3 ColorOut;
layout (location = 2) out vec3 NormalOut;
layout (location = 3) out vec2 TexcoordOut;
layout (location = 4) out vec3 VelocityOut;

void main()
{
	PositionOut = Position0;
	ColorOut = Color0;
	NormalOut = Normal0;
	TexcoordOut = Texcoord0;
	VelocityOut = vec3(0);
}
