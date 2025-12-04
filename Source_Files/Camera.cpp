#include <Camera.hpp>

Camera::Camera()
	: position(0.0f, 0.0f, 3.0f)
	, target(0.0f, 0.0f, 0.0f)
	, up(0.0f, 1.0f, 0.0f)
	, fov(45.0f)
	, aspectRatio(800.0f / 600.0f)
	, nearPlane(0.1f)
	, farPlane(100.0f)
{
	std::cout << "Camera created" << std::endl;
	Update();
}

Camera::Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up)
	: position(position)
	, target(target)
	, up(up)
	, fov(45.0f)
	, aspectRatio(800.0f / 600.0f)
	, nearPlane(0.1f)
	, farPlane(100.0f)
{
	std::cout << "Camera created" << std::endl;
	Update();
}

Camera::~Camera()
{
	std::cout << "Camera destroyed" << std::endl;
}

void Camera::SetPosition(const glm::vec3& position)
{
	this->position = position;
}

void Camera::SetTarget(const glm::vec3& target)
{
	this->target = target;
}

void Camera::SetProjection(float fov, glm::vec2 windowSize, float nearPlane, float farPlane)
{
	this->fov = fov;
	this->aspectRatio = windowSize.x / windowSize.y;
	this->nearPlane = nearPlane;
	this->farPlane = farPlane;

	// Update projection matrix
	projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
}

glm::mat4 Camera::GetViewMatrix() const
{
	return viewMatrix;
}

glm::mat4 Camera::GetProjectionMatrix() const
{
	return projectionMatrix;
}

glm::vec3 Camera::GetPosition() const
{
	return position;
}

glm::vec3 Camera::GetDirection() const
{
	return direction;
}

void Camera::Update()
{
	// Calculate direction vector
	direction = glm::normalize(target - position);

	// Update view matrix
	viewMatrix = glm::lookAt(position, target, up);
}
