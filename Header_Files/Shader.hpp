#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h> // or your windowing library
#include <fstream>
#include <sstream>

//this is the class i would use to compile my shaders
class Shader
{
	//default constructor for the shader.
	Shader() : shaderProgramID(0) {}
	Shader(const std::string& vertexCode, const std::string& fragmentCode);
	Shader(const std::string& vertexCode, const std::string& fragmentCode, const std::string& binName);
	//for compiler
	static Shader FromSource(const std::string& vertPath, const std::string& fragPath);

	void Use();//bind shader.
	void UnUse(); //unbind shader.
	//for compilers 
	// Set uniform variables in the shader program (Overloads)
	void SetBool(const std::string& uniformName, bool value);
	void SetInt(const std::string& uniformName, int value);
	void SetFloat(const std::string& uniformName, float value);
	void SetVector3(const std::string& uniformName, glm::vec3 vector);
	void SetVector4(const std::string& uniformName, glm::vec4 vector);
	void SetMatrix3(const std::string& uniformName, glm::mat3 matrix);
	void SetMatrix4(const std::string& uniformName, glm::mat4 matrix);
	~Shader();

	GLuint getID();

private:
	GLuint shaderProgramID;

};