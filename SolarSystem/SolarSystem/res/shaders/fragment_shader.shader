#version 330 core
#extension GL_EXT_texture_array : enable

out vec4 FragColor;

in vec2 TexCoord;

flat in int samp_ind;

uniform sampler2DArray planetTexture;

void main() {

	int ind = samp_ind;

	if (ind >= 3) {

		if (ind == 3) {
			FragColor = mix(texture(planetTexture, vec3(TexCoord.xy, ind + 1)), texture(planetTexture, vec3(TexCoord.xy, ind + 2)), 0.5);
		}
		else {
			FragColor = texture(planetTexture, vec3(TexCoord.xy, ind + 2));
		}

	}
	else{

		if (ind == 2) {
			FragColor = mix(texture(planetTexture, vec3(TexCoord.xy, ind)), texture(planetTexture, vec3(TexCoord.xy, ind + 1)), 0.3);
		}
		else {
			FragColor = texture(planetTexture, vec3(TexCoord.xy, ind));
		}

	}

	
}