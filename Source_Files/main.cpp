#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Mesh.hpp>
#include <Shader.hpp>

int main()
{
	std::cout << "Hello world" << std::endl;
	
	// Initialize GLFW
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	// Set OpenGL version to 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a window
	GLFWwindow* window = glfwCreateWindow(800, 600, "Doughnut", nullptr, nullptr);
	if (!window) {
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// Make the window's context current
	glfwMakeContextCurrent(window);

	// Load OpenGL function pointers using GLAD
	if (!gladLoadGL()) {
		std::cerr << "Failed to initialize GLAD" << std::endl;
		glfwDestroyWindow(window);
		glfwTerminate();
		return -1;
	}

	// Enable vsync
	glfwSwapInterval(1);

	// Enable depth testing
	glEnable(GL_DEPTH_TEST);

	// Vertex shader source code
	const char* vertexShaderSource = R"(
		#version 330 core

		layout(location = 0) in vec3 position;
		layout(location = 1) in vec3 normal;

		out VS_OUT {
			vec3 fragNormal;
			vec3 fragPos;
		} vs_out;

		uniform mat4 model;
		uniform mat4 view;
		uniform mat4 projection;
		uniform mat3 normalMatrix;

		void main()
		{
			gl_Position = projection * view * model * vec4(position, 1.0);
			vs_out.fragPos = vec3(model * vec4(position, 1.0));
			vs_out.fragNormal = normalize(normalMatrix * normal);
		}
	)";

	// Fragment shader source code
	const char* fragmentShaderSource = R"(
		#version 330 core

		in VS_OUT {
			vec3 fragNormal;
			vec3 fragPos;
		} fs_in;

		out vec4 FragColor;

		uniform vec3 lightPos;
		uniform vec3 viewPos;
		uniform vec3 objectColor;

		void main()
		{
			float ambientStrength = 0.3;
			vec3 ambient = ambientStrength * objectColor;

			vec3 norm = normalize(fs_in.fragNormal);
			vec3 lightDir = normalize(lightPos - fs_in.fragPos);
			float diff = max(dot(norm, lightDir), 0.0);
			vec3 diffuse = diff * objectColor;

			float specularStrength = 0.5;
			vec3 viewDir = normalize(viewPos - fs_in.fragPos);
			vec3 reflectDir = reflect(-lightDir, norm);
			float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
			vec3 specular = specularStrength * spec * vec3(1.0);

			vec3 result = ambient + diffuse + specular;
			FragColor = vec4(result, 1.0);
		}
	)";

	// Load shaders
	Shader shader(vertexShaderSource, fragmentShaderSource);

	// Create a simple triangle for testing
	std::vector<Vertex> triangleVertices = {
		// Position					Normal
		{{ 0.0f,  0.5f,  0.0f },	{ 0.0f,  0.0f,  1.0f }},	// Top
		{{-0.5f, -0.5f,  0.0f },	{ 0.0f,  0.0f,  1.0f }},	// Bottom-left
		{{ 0.5f, -0.5f,  0.0f },	{ 0.0f,  0.0f,  1.0f }}		// Bottom-right
	};

	std::vector<unsigned int> triangleIndices = {
		0, 1, 2	// Triangle
	};

	Mesh triangle;
	triangle.setMeshData(triangleVertices, triangleIndices);

	// Main rendering loop
	while (!glfwWindowShouldClose(window)) {
		// Clear the screen
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use shader program
		shader.Use();

		// Create matrices
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::lookAt(
			glm::vec3(0.0f, 0.0f, 2.0f),	// Camera position
			glm::vec3(0.0f, 0.0f, 0.0f),	// Look at
			glm::vec3(0.0f, 1.0f, 0.0f)		// Up vector
		);
		glm::mat4 projection = glm::perspective(
			glm::radians(45.0f),
			800.0f / 600.0f,
			0.1f,
			100.0f
		);

		// Normal matrix (transpose of inverse of model matrix)
		glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));

		// Set uniforms
		shader.SetMatrix4("model", model);
		shader.SetMatrix4("view", view);
		shader.SetMatrix4("projection", projection);
		shader.SetMatrix3("normalMatrix", normalMatrix);
		shader.SetVector3("objectColor", glm::vec3(0.3f, 0.6f, 1.0f));		// Light blue
		shader.SetVector3("lightPos", glm::vec3(2.0f, 2.0f, 2.0f));
		shader.SetVector3("viewPos", glm::vec3(0.0f, 0.0f, 2.0f));

		// Render triangle
		triangle.render();

		shader.UnUse();

		// Swap front and back buffers
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();
	}

	// Clean up
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}