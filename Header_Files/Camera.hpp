#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

class Camera
{
public:
	Camera();
	Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f));
	~Camera();

	// Setters
	void SetPosition(const glm::vec3& position);
	void SetTarget(const glm::vec3& target);
	void SetProjection(float fov, glm::vec2 windowSize, float nearPlane, float farPlane);

	// Getters
	glm::mat4 GetViewMatrix() const;
	glm::mat4 GetProjectionMatrix() const;
	glm::vec3 GetPosition() const;
	glm::vec3 GetDirection() const;

	// Update camera matrices
	void Update();

private:
	// Camera vectors
	glm::vec3 position;
	glm::vec3 target;
	glm::vec3 up;
	glm::vec3 direction;

	// Matrices
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;

	// Projection parameters
	float fov;
	float aspectRatio;
	float nearPlane;
	float farPlane;
};
