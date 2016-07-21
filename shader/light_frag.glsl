#version 330

struct light
{
	vec3 pos;
	vec3 color;
	/* intensity parameters */
	float amb;
	float diff;
	/* attenuation parameters */
	float exp;
	float lin;
	float cst;
};

uniform mat4 PreviousMVP;
uniform vec2 ScreenDimensions;
uniform vec3 EyePosition;

uniform sampler2D PositionMap;
uniform sampler2D ColorMap;
uniform sampler2D NormalMap;
uniform sampler2D TexcoordMap;
uniform sampler2D VelocityMap;

uniform light Light;

vec3 light_color(vec3 dir, vec3 pos, vec3 nrm)
{
	vec3 color = Light.color * Light.amb;
	float dfac = dot(nrm, -dir);

	if (dfac > 0) {
		vec3 to_eye = normalize(EyePosition - pos);
		vec3 ref = normalize(reflect(dir, nrm));
		float sfac = dot(to_eye, ref);
		sfac = pow(sfac, 36);
		color += Light.color * Light.diff * dfac;
		if (sfac > 0)
			color += Light.color * sfac * 0.2f;
	}

	return color;
}

float light_attenuation(float dist)
{
	float atten = Light.cst;
	atten += Light.lin * dist;
	atten += Light.exp * dist * dist;
	return max(1, atten);
}

vec3 point_light_color(vec3 pos, vec3 nrm)
{
	vec3 dir = pos - Light.pos;
	float dist = length(dir);
	dir = normalize(dir);
	return light_color(dir, pos, nrm) / light_attenuation(dist);
}

void main()
{
	vec2 texcoord = gl_FragCoord.xy / ScreenDimensions.xy;
	vec3 pos = texture(PositionMap, texcoord).xyz;
	vec3 color = texture(ColorMap, texcoord).xyz;
	vec3 normal = texture(NormalMap, texcoord).xyz;
	normal = normalize(normal);
	gl_FragColor = vec4(color * point_light_color(pos, normal), 1);
}
