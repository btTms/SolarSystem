#version 330 core

out vec4 FragColor;

in float ind;
in vec2 TexCoord;

uniform sampler2D planetTexture[9];

void main() {

	FragColor = texture(planetTexture[uint(ind)], TexCoord);
}