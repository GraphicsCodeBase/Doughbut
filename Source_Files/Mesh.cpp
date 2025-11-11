#include <Mesh.hpp>

Mesh::Mesh() : VAO(0), VBO(0), EBO(0)
{
}

Mesh::~Mesh()
{
	// Clean up OpenGL resources
	if (EBO != 0) {
		glDeleteBuffers(1, &EBO);
	}
	if (VBO != 0) {
		glDeleteBuffers(1, &VBO);
	}
	if (VAO != 0) {
		glDeleteVertexArrays(1, &VAO);
	}
}

void Mesh::setMeshData(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
{
	this->vertices = vertices;
	this->indices = indices;
	this->setupMesh();
}

void Mesh::setupMesh()
{
	// Generate VAO, VBO, and EBO if they don't exist
	if (VAO == 0) {
		glGenVertexArrays(1, &VAO);
	}
	if (VBO == 0) {
		glGenBuffers(1, &VBO);
	}
	if (EBO == 0) {
		glGenBuffers(1, &EBO);
	}

	// Bind VAO
	glBindVertexArray(VAO);

	// Fill VBO with vertex data
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	// Fill EBO with index data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// Set vertex attribute pointers
	// Position attribute (location 0)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	// Normal attribute (location 1)
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	// Unbind VAO
	glBindVertexArray(0);
}

void Mesh::render()
{
	// Bind VAO
	glBindVertexArray(VAO);

	// Draw the mesh using indices
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	// Unbind VAO
	glBindVertexArray(0);
}

unsigned int Mesh::getIndexCount() const
{
	return indices.size();
}