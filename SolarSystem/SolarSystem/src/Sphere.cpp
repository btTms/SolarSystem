#include "Sphere.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


const int MIN_SECTOR_COUNT = 3;
const int MIN_STACK_COUNT = 2;
const float MIN_SPHERE_RADIUS = 4;

Sphere::Sphere(float radius, int sectorCount, int stackCount, int increment, std::vector<unsigned int>& final_indices, std::vector<float>& final_vertex_buffer_data){
	setProperties(radius, sectorCount, stackCount, increment, final_indices, final_vertex_buffer_data);
	this->interleavedStride = 32;
}


void Sphere::printInfo(){
	std::cout << "===== Sphere =====\n"
		<< "        Radius: " << radius << "\n"
		<< "  Sector Count: " << sectorCount << "\n"
		<< "   Stack Count: " << stackCount << "\n"
		<< "Triangle Count: " << getTriangleCount() << "\n"
		<< "   Index Count: " << getIndexCount() << "\n"
		<< "  Vertex Count: " << getVertexCount() << "\n"
		<< "  Normal Count: " << getNormalCount() << "\n"
		<< "TexCoord Count: " << getTexCoordCount() << std::endl;
}

// setters:

void Sphere::setRadius(float radius){
	if (radius < MIN_SPHERE_RADIUS) {
		this->radius = MIN_SPHERE_RADIUS;
	}
	else {
		this->radius = radius;
	}
}

void Sphere::setSectorCount(int sectorCount){
	if (sectorCount < MIN_SECTOR_COUNT) {
		this->sectorCount = MIN_SECTOR_COUNT;
	}
	else {
		this->sectorCount = sectorCount;
	}
}

void Sphere::setStackCount(int stackCount){
	if (stackCount < MIN_STACK_COUNT) {
		this->stackCount = MIN_STACK_COUNT;
	}
	else {
		this->stackCount = stackCount;
	}
}


// builders:


void Sphere::buildInterleavedVertices(std::vector<float>& final_vertex_buffer_data){

	std::vector<float>().swap(interleavedVertices);

	std::size_t i, j;
	std::size_t count = vertices.size();

	for (i = 0, j = 0; i < count; i += 3, j += 2) {

		// vertex positions:
		this->interleavedVertices.push_back(vertices[i]);
		this->interleavedVertices.push_back(vertices[i + 1]);
		this->interleavedVertices.push_back(vertices[i + 2]);

		// normals:
		this->interleavedVertices.push_back(normals[i]);
		this->interleavedVertices.push_back(normals[i + 1]);
		this->interleavedVertices.push_back(normals[i + 2]);

		// texCoords:
		this->interleavedVertices.push_back(texCoords[j]);
		this->interleavedVertices.push_back(texCoords[j + 1]);
	}

	final_vertex_buffer_data.insert(final_vertex_buffer_data.end(), interleavedVertices.begin(), interleavedVertices.end());


}

void Sphere::buildVerticesSmooth(int increment, std::vector<unsigned int>& final_indices, std::vector<float>& final_vertex_buffer_data) {

	const float PI = acos(-1);

	// clear memory:
	clearArrays();

	float x, y, z, xz;									 // vertex position
	float nx, ny, nz, lenghtInv = 1.0f / (this->radius); // noraml
	float s, t;											 // texCoords

	float sectorStep = 2 * PI / sectorCount; // how much do we step in each layer arond the circle (x, y)
	float stackStep = PI / stackCount; // how much do we stepc in each layer in half a circle (z)
	float theta, phi;

	for (int i = 0; i <= stackCount; i++) {

		phi = PI / 2 - i * stackStep; // [pi / 2, - pi / 2]
		xz = radius * cosf(phi);      // a sugar vetulete az x z koordinata rendszerre
		y = -radius * sinf(phi);       // hol tartunk a felkorben 

		for (int j = 0; j <= sectorCount; j++) {

			theta = j * sectorStep;

			x = -xz * cosf(theta); // r * cos(phi) * cos(theta)
			z = xz * sinf(theta); // r * cos(phi) * sin(theta)

			this->vertices.push_back(x);
			this->vertices.push_back(y);
			this->vertices.push_back(z);

			// normalized normals:
			nx = x * lenghtInv;
			ny = y * lenghtInv;
			nz = z * lenghtInv;

			this->normals.push_back(nx);
			this->normals.push_back(ny);
			this->normals.push_back(nz);

			// vertex texture coordintes:
			s = (float)j / sectorCount;
			t = (float)i / stackCount;

			texCoords.push_back(s);
			texCoords.push_back(t);

		}

	}

	// indices:
	unsigned int k1, k2;

	for (int i = 0; i < stackCount; i++) {

		k1 = i * (sectorCount + 1);
		k2 = k1 + sectorCount + 1;

		for (int j = 0; j < sectorCount; j++, k1++, k2++) {

			if (i != 0) {
				this->indices.push_back(k1);
				this->indices.push_back(k2);
				this->indices.push_back(k1 + 1);

				final_indices.push_back(k1 + (increment * 703));
				final_indices.push_back(k2 + (increment * 703));
				final_indices.push_back(k1 + 1 + (increment * 703));
			}

			if (i != (stackCount - 1)) {
				this->indices.push_back(k1 + 1);
				this->indices.push_back(k2);
				this->indices.push_back(k2 + 1);

				final_indices.push_back(k1 + 1 + (increment * 703));
				final_indices.push_back(k2 + (increment * 703));
				final_indices.push_back(k2 + 1 + (increment * 703));
			}

		}
	}

	// generate interleaved vertex array;
	buildInterleavedVertices(final_vertex_buffer_data);

}


void Sphere::setProperties(float radius, int sectorCount, int stackCount, int incerement, std::vector<unsigned int>& final_indices, std::vector<float>& final_vertex_buffer_data) {
	this->radius = abs(radius);

	this->sectorCount = sectorCount;

	if (sectorCount < MIN_SECTOR_COUNT) {
		this->sectorCount = MIN_SECTOR_COUNT;
	}

	this->stackCount = stackCount;

	if (stackCount < MIN_STACK_COUNT) {
		this->stackCount = MIN_STACK_COUNT;
	}

	buildVerticesSmooth(incerement, final_indices, final_vertex_buffer_data);
}

void Sphere::clearArrays(){

	std::vector<float>().swap(vertices);
	std::vector<float>().swap(normals);
	std::vector<float>().swap(texCoords);
	std::vector<unsigned int>().swap(indices);
}

glm::vec3 Sphere::computeFaceNormal(float x1, float y1, float z1,  // v1
									float x2, float y2, float z2,  // v2
									float x3, float y3, float z3)  // v3
{
	const float EPSILON = 0.000001f;

	glm::vec3 normal(0.0f);     // default return value (0,0,0)
	float nx, ny, nz;

	// find 2 edge vectors: v1-v2, v1-v3
	float ex1 = x2 - x1;
	float ey1 = y2 - y1;
	float ez1 = z2 - z1;
	float ex2 = x3 - x1;
	float ey2 = y3 - y1;
	float ez2 = z3 - z1;

	// cross product: e1 x e2
	nx = ey1 * ez2 - ez1 * ey2;
	ny = ez1 * ex2 - ex1 * ez2;
	nz = ex1 * ey2 - ey1 * ex2;

	// normalize only if the length is > 0
	float length = sqrtf(nx * nx + ny * ny + nz * nz);
	if (length > EPSILON)
	{
		// normalize
		float lengthInv = 1.0f / length;
		normal.x = nx * lengthInv;
		normal.y = ny * lengthInv;
		normal.z = nz * lengthInv;
	}

	return normal;
}


// Load texture:
unsigned int Sphere::loadTexture(const char* path, bool wrap) {

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// set the options:
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);


	int width, height, nrOfChannels;
	stbi_set_flip_vertically_on_load(true);

	unsigned char* data = stbi_load(path, &width, &height, &nrOfChannels, 0);

	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "FAILED TO LOAD TEXTURE\n";
	}

	stbi_image_free(data);

	return texture;
	
}