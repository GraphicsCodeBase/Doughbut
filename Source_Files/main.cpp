#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Mesh.hpp>
#include <Shader.hpp>
#include <Camera.hpp>

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

	// Load shaders from files
	Shader shader = Shader::FromSource("../../../Shaders/main.vert", "../../../Shaders/main.frag");

	Mesh doughnut;
	doughnut.generateTorus(
		1.0f,   // majorRadius - size of the hole
		0.4f,   // minorRadius - thickness of the tube
		50,     // majorSegments - smoothness around the ring
		30      // minorSegments - smoothness around the tube
	);

	// Camera setup
	Camera cam(glm::vec3(0.0f, 0.0f, 4.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	glm::vec3 camera_position = glm::vec3(0.0f, 0.0f, 4.0f);
	glm::vec3 camera_dir = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec2 cursor_pos = glm::vec2(0.0f);
	glm::vec2 window_size = glm::vec2(800.0f, 600.0f);

	// Delta time
	float lastFrame = 0.0f;
	float dt = 0.0f;

	// Rotation
	float rotationAngleX = 0.0f;
	float rotationAngleY = 0.0f;
	float rotationAngleZ = 0.0f;
	float rotationSpeedX = 50.0f;  // Degrees per second around X-axis
	float rotationSpeedY = 30.0f;  // Degrees per second around Y-axis
	float rotationSpeedZ = 70.0f;  // Degrees per second around Z-axis

	// Main rendering loop
	while (!glfwWindowShouldClose(window)) {
		// Calculate delta time
		float currentFrame = (float)glfwGetTime();
		dt = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Clear the screen
		glClearColor(1.0f, 0.8f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Camera controls
		{
			// Cursor
			double cursor_x = 0.0;
			double cursor_y = 0.0;
			glfwGetCursorPos(window, &cursor_x, &cursor_y);

			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2)) {
				float speed = 4.0f;
				auto side = glm::normalize(glm::cross(camera_dir, glm::vec3(0, 1, 0)));
				auto up = glm::normalize(glm::cross(camera_dir, side));

				if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) {
					speed *= 2.0f;
				}

				// Move
				if (glfwGetKey(window, GLFW_KEY_W)) {
					camera_position += glm::normalize(camera_dir) * dt * speed;
				}
				if (glfwGetKey(window, GLFW_KEY_S)) {
					camera_position -= glm::normalize(camera_dir) * dt * speed;
				}
				if (glfwGetKey(window, GLFW_KEY_A)) {
					camera_position -= glm::normalize(side) * dt * speed;
				}
				if (glfwGetKey(window, GLFW_KEY_D)) {
					camera_position += glm::normalize(side) * dt * speed;
				}
				if (glfwGetKey(window, GLFW_KEY_SPACE)) {
					camera_position -= up * dt * speed;
				}
				if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)) {
					camera_position += up * dt * speed;
				}

				// View rotation
				glm::vec2 cursor_delta = glm::vec2((float)cursor_x - cursor_pos.x, (float)cursor_y - cursor_pos.y);
				camera_dir = glm::vec3(glm::vec4(camera_dir, 0) * glm::rotate(glm::mat4(1.0f), glm::radians(15.0f) * dt * cursor_delta.y, side));
				camera_dir = glm::vec3(glm::vec4(camera_dir, 0) * glm::rotate(glm::mat4(1.0f), glm::radians(15.0f) * dt * cursor_delta.x, glm::vec3(0, 1, 0)));
			}
			cursor_pos = glm::vec2((float)cursor_x, (float)cursor_y);

			// Update camera
			float near = 0.01f;
			float far = 1000.0f;
			cam.SetProjection(70.0f, window_size, near, far);
			cam.SetPosition(camera_position);
			cam.SetTarget(camera_position + camera_dir);
			cam.Update();
		}

		// Update rotations
		rotationAngleX += rotationSpeedX * dt;
		rotationAngleY += rotationSpeedY * dt;
		rotationAngleZ += rotationSpeedZ * dt;

		// Keep angles in range [0, 360)
		if (rotationAngleX >= 360.0f) rotationAngleX -= 360.0f;
		if (rotationAngleY >= 360.0f) rotationAngleY -= 360.0f;
		if (rotationAngleZ >= 360.0f) rotationAngleZ -= 360.0f;

		// Use shader program
		shader.Use();

		// Create matrices with multiple rotations (all three axes)
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(rotationAngleX), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate around X-axis (pitch)
		model = glm::rotate(model, glm::radians(rotationAngleY), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around Y-axis (yaw)
		model = glm::rotate(model, glm::radians(rotationAngleZ), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate around Z-axis (roll)
		glm::mat4 view = cam.GetViewMatrix();
		glm::mat4 projection = cam.GetProjectionMatrix();

		// Normal matrix (transpose of inverse of model matrix)
		glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));

		// Set uniforms
		shader.SetMatrix4("model", model);
		shader.SetMatrix4("view", view);
		shader.SetMatrix4("projection", projection);
		shader.SetMatrix3("normalMatrix", normalMatrix);
		shader.SetVector3("objectColor", glm::vec3(0.3f, 0.6f, 1.0f));		// Light blue
		shader.SetVector3("lightPos", glm::vec3(2.0f, 2.0f, 2.0f));
		shader.SetVector3("viewPos", cam.GetPosition());

		// Render doughnut
		doughnut.render();

		shader.UnUse();//unbind shader.

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