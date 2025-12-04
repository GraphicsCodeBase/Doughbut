#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <glad/glad.h>

//This is the Mesh class where i would hold my doughnut Mesh.

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
};

class Mesh
{
	public:
		Mesh();
		~Mesh();

		// Set mesh data (vertices and indices)
		void setMeshData(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

		// Generate a procedural torus (doughnut) mesh
		void generateTorus(float majorRadius, float minorRadius, unsigned int majorSegments, unsigned int minorSegments);

		// Render the mesh
		void render();

		// Get the number of indices
		unsigned int getIndexCount() const;

	private:
		// OpenGL buffer objects
		GLuint VAO;		// Vertex Array Object
		GLuint VBO;		// Vertex Buffer Object
		GLuint EBO;		// Element Buffer Object (Index Buffer)

		// Mesh data array of structs.
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

		// Helper method to setup the VAO and buffers
		void setupMesh();
};