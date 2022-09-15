#version 330 core

out vec4 FragColor;

uniform vec3 color;

uniform sampler2D Texture;

in vec2 TexCoord;

void main() {

	FragColor = texture(Texture, TexCoord);
}