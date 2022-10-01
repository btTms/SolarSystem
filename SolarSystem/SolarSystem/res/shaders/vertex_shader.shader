#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 4) in mat4 planetInstanceMatrix;

uniform mat4 projection;
uniform mat4 view;

out vec2 TexCoord;

flat out int samp_ind;

void main() {

	gl_Position = projection * view * planetInstanceMatrix * vec4(aPos, 1.0f);
	TexCoord = aTexCoords;

	samp_ind = gl_InstanceID;


}