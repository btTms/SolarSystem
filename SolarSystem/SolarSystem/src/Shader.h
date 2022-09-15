#pragma once

#include <GL/glew.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {

public:
	// the program ID:
	unsigned int ID;

	// constructor:
	Shader(const char* vertexPath, const char* fragmentPath);

	// use / activate the shader:
	void use();
	//utility unfirm functions:
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;

};
