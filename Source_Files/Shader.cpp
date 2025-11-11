#include <Shader.hpp>

Shader::Shader(const std::string& vertexCode, const std::string& fragmentCode) {
	// Convert from std::string to const char*
	const char* vertexShaderCode = vertexCode.c_str();
	const char* fragmentShaderCode = fragmentCode.c_str();

	int success{};
	char infoLog[1024]{};

	// Vertex Shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	// Specify source code for shader before compiling it
	glShaderSource(vertexShader, 1, &vertexShaderCode, nullptr);
	glCompileShader(vertexShader);

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 1024, nullptr, infoLog);
		std::cerr << "Failed to compile vertex shader!" << std::endl;
		std::cerr << "InfoLog: " << infoLog << std::endl;
	}

	// Fragment Shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	// Specify source code for shader before compiling it
	glShaderSource(fragmentShader, 1, &fragmentShaderCode, nullptr);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 1024, nullptr, infoLog);
		std::cerr << "Failed to compile fragment shader!" << std::endl;
		std::cerr << "InfoLog: " << infoLog << std::endl;
	}


	// Take both shaders and combine them to form a Shader Program, assign to private member variable
	shaderProgramID = glCreateProgram();
	glAttachShader(shaderProgramID, vertexShader);
	glAttachShader(shaderProgramID, fragmentShader);
	glLinkProgram(shaderProgramID);

	glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgramID, 1024, nullptr, infoLog);
		std::cerr << "Failed to link shaders!" << std::endl;
		std::cerr << "InfoLog: " << infoLog << std::endl;
	}
	//else
	//{
	//	//Only generate binary if linking succeeded
	//	GLint length = 0;
	//	glGetProgramiv(shaderProgramID, GL_PROGRAM_BINARY_LENGTH, &length);

	//	if (length > 0) {
	//		std::vector<unsigned char> binary(length);
	//		GLenum format = 0;

	//		glGetProgramBinary(shaderProgramID, length, nullptr, &format, binary.data());

	//		// Save to disk (you can pick your cache folder)
	//		std::filesystem::create_directories("../Assets/Shaders/cache");
	//		std::ofstream out("../Assets/Shaders/cache/myshader.bin", std::ios::binary);
	//		out.write(reinterpret_cast<const char*>(binary.data()), length);
	//		out.close();

	//		// Save format separately
	//		std::ofstream meta("../Assets/Shaders/cache/myshader.meta");
	//		meta << format;
	//		meta.close();

	//		std::cout << "Shader binary saved to cache." << std::endl;
	//	}
	//}

	// We have linked shaders to the program, can delete shaders now
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::Use() {
	glUseProgram(shaderProgramID);
}
void Shader::UnUse()
{
	glUseProgram(0);
}
void Shader::SetBool(const std::string& uniformName, bool value) {
	glUniform1i(glGetUniformLocation(shaderProgramID, uniformName.c_str()), static_cast<int>(value));
}

void Shader::SetInt(const std::string& uniformName, int value) {
	glUniform1i(glGetUniformLocation(shaderProgramID, uniformName.c_str()), value);
}

void Shader::SetFloat(const std::string& uniformName, float value) {
	glUniform1f(glGetUniformLocation(shaderProgramID, uniformName.c_str()), value);
}

void Shader::SetVector3(const std::string& uniformName, glm::vec3 vector) {
	glUniform3f(glGetUniformLocation(shaderProgramID, uniformName.c_str()), vector.x, vector.y, vector.z);
}
void Shader::SetVector4(const std::string& uniformName, glm::vec4 vector)
{
	GLint location = glGetUniformLocation(shaderProgramID, uniformName.c_str());
	glUniform4fv(location, 1, glm::value_ptr(vector));
}

void Shader::SetMatrix3(const std::string& uniformName, glm::mat3 matrix) {
	glUniformMatrix3fv(glGetUniformLocation(shaderProgramID, uniformName.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::SetMatrix4(const std::string& uniformName, glm::mat4 matrix) {
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, uniformName.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}

Shader::~Shader() {
	glDeleteProgram(shaderProgramID);
}

Shader Shader::FromSource(const std::string& vertPath, const std::string& fragPath)
{
	// Helper lambda to read file content into a string
	auto ReadFile = [](const std::string& path) -> std::string {
		std::ifstream file(path);
		if (!file.is_open()) {
			std::cerr << "Failed to open shader file: " << path << std::endl;
			return "";
		}
		std::stringstream ss;
		ss << file.rdbuf();
		return ss.str();
		};

	std::string vertexCode = ReadFile(vertPath);
	std::string fragmentCode = ReadFile(fragPath);

	// Create Shader object using your existing constructor
	return Shader(vertexCode, fragmentCode);
}

GLuint Shader::getID() {
	return shaderProgramID;
}