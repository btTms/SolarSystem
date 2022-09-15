#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <cmath>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Sphere{

public:

	Sphere(float radius, int sectorCount, int stackCount);

	// for debugging:
	void printInfo();

	// getters & setters:
	float getRadius() const { return radius; }
	int getSectorCount() const { return sectorCount; }
	int getStackCount() const { return stackCount; }

	void setProperties(float radius, int sectorCount, int stackCount);
	void setRadius(float radius);
	void setSectorCount(int sectorCount);
	void setStackCount(int stackCount);

	// general infos:
	unsigned int getVertexCount() const { return (unsigned int)vertices.size() / 3; }
	unsigned int getNormalCount() const { return (unsigned int)normals.size() / 3; }
	unsigned int getTexCoordCount() const { return (unsigned int)texCoords.size() / 2; }
	unsigned int getIndexCount() const { return (unsigned int)indices.size(); }
	unsigned int getLineIndexCount() const { return (unsigned int)lineIndices.size(); }
	unsigned int getTriangleCount() const { return getIndexCount() / 3; }

	// sizes for strides and memory
	unsigned int getVertexSize() const { return (unsigned int)vertices.size() * sizeof(float); }
	unsigned int getNormalSize() const { return (unsigned int)normals.size() * sizeof(float); }
	unsigned int getTexCoordSize() const { return (unsigned int)texCoords.size() * sizeof(float); }
	unsigned int getIndexSize() const { return (unsigned int)indices.size() * sizeof(unsigned int); }
	unsigned int getLineIndexSize() const { return (unsigned int)lineIndices.size() * sizeof(unsigned int); }


	// geters for the final verticies with all the 
	unsigned int getInterleavedVertexCount() const { return getVertexCount(); }    // # of vertices
	unsigned int getInterleavedVertexSize() const { return (unsigned int)interleavedVertices.size() * sizeof(float); }    // # of bytes
	int getInterleavedStride() const { return interleavedStride; }   // should be 32 bytes
	const float* getInterleavedVertices() const { return interleavedVertices.data(); }

	// getters for the drawcalls
	const float* getVertices() const { return vertices.data(); }
	const float* getNormals() const { return normals.data(); }
	const float* getTexCoords() const { return texCoords.data(); }
	const unsigned int* getIndices() const { return indices.data(); }
	const unsigned int* getLineIndices() const { return lineIndices.data(); }

	// textures:

	unsigned int loadTexture(const char* path, bool wrap);

private:
	float radius;
	int sectorCount; // nr of partitions in each layer (top and bottom ones are triangles)
	int stackCount; // nr of layers of a sphere

	std::vector<float> vertices;
	std::vector<unsigned int> indices;
	std::vector<unsigned int> lineIndices;
	std::vector<float> normals;
	std::vector<float> texCoords;

	// final vertices:
	std::vector<float> interleavedVertices;
	int interleavedStride;
	
	// builds:
	void buildVerticesSmooth();
	void buildInterleavedVertices();

	// clear
	void clearArrays();

	// computing normals:
	glm::vec3 computeFaceNormal(float x1, float y1, float z1,
								float x2, float y2, float z2,
								float x3, float y3, float z3);


};

