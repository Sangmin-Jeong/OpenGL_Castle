#version 430 core

in vec3 color;
in vec2 texCoord;
in vec4 ambient;
out vec4 frag_color;

uniform sampler2D texture0;

void main()
{
	frag_color = texture(texture0, texCoord) * vec4(color, 1.0f) * ambient;
}