#version 330

uniform vec2 ScreenDimensions;

uniform sampler2D FinalMap;
uniform sampler2D ColorMap;
uniform sampler2D TexcoordMap;

void main()
{
	vec2 Texcoord = gl_FragCoord.xy / ScreenDimensions.xy;
	vec2 MappedTexcoord = texture(TexcoordMap, Texcoord).xy;
	vec3 MappedColor = texture(ColorMap, Texcoord).xyz;
	vec3 FinalColor = texture(FinalMap, MappedTexcoord).xyz;
	gl_FragColor = vec4(MappedColor * FinalColor, 1);
}
