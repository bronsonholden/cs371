#version 330

uniform vec2 ScreenDimensions;

uniform sampler2D ColorMap;

void main()
{
	vec2 Texcoord = gl_FragCoord.xy / ScreenDimensions.xy;
	vec4 Color = texture(ColorMap, Texcoord);
	Color.x = clamp(1.0f - Color.x, 0, 1);
	Color.y = clamp(1.0f - Color.y, 0, 1);
	Color.z = clamp(1.0f - Color.z, 0, 1);
	gl_FragColor = Color;
}
