#version 430 core

in vec3 color;
in vec2 texCoord;
in vec3 normal;
out vec4 frag_color;

uniform sampler2D texture0;

uniform vec3 ambientColor;
uniform float ambientStrength;

uniform vec3 dirColor;
uniform float dirStrength;
uniform vec3 lightDirection;

void main()
{
	// Calculate lighting.
	vec4 ambient = vec4(ambientColor, 1.0f) * ambientStrength;
	
	float dirFactor = max( dot( normalize(normal), normalize(lightDirection) ), 0.0f);
	vec4 directional = vec4(dirColor, 1.0f) * dirStrength * dirFactor;
	
	frag_color = texture(texture0, texCoord) * vec4(color, 1.0f) * (ambient + directional);
}