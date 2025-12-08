#include <Mesh.hpp>
#include <cmath>
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

//What's happening:
//- Outer loop(i) : Steps around the main ring(0 to 360)
//- Inner loop(j) : Steps around the tube(0 to 360)
//- For each(i, j) pair, we calculate :
//-theta and phi angles in radians
//- Position using the parametric equations
//- Normal vector(perpendicular to surface)
void Mesh::generateTorus(float majorRadius, float minorRadius, unsigned int majorSegments, unsigned int minorSegments)
{
	//clear previous data
	vertices.clear();
	indices.clear();

	const float PI = 3.14159265359f;//for pi value.

	//generate vertices.
		 // Generate vertices
	for (unsigned int i = 0; i <= majorSegments; ++i)
	{
		// Angle around the main ring (theta)
		float theta = (float)i / (float)majorSegments * 2.0f * PI;
		float cosTheta = cos(theta);
		float sinTheta = sin(theta);

		for (unsigned int j = 0; j <= minorSegments; ++j)
		{
			// Angle around the tube (phi)
			float phi = (float)j / (float)minorSegments * 2.0f * PI;
			float cosPhi = cos(phi);
			float sinPhi = sin(phi);

			// Calculate position using torus parametric equations
			float x = (majorRadius + minorRadius * cosPhi) * cosTheta;
			float y = (majorRadius + minorRadius * cosPhi) * sinTheta;
			float z = minorRadius * sinPhi;

			// Calculate normal (points outward from tube surface)
			glm::vec3 normal = glm::normalize(glm::vec3(
				cosPhi * cosTheta,
				cosPhi * sinTheta,
				sinPhi
			));

			// Create vertex and add to array
			Vertex vertex;
			vertex.position = glm::vec3(x, y, z);
			vertex.normal = normal;
			vertices.push_back(vertex);
		}
	}
	// Generate indices - connect vertices into triangles
	for (unsigned int i = 0; i < majorSegments; ++i)
	{
		for (unsigned int j = 0; j < minorSegments; ++j)
		{
			// Calculate indices for the quad
			unsigned int first = i * (minorSegments + 1) + j;
			unsigned int second = first + minorSegments + 1;

			// First triangle of the quad
			indices.push_back(first);
			indices.push_back(second);
			indices.push_back(first + 1);

			// Second triangle of the quad
			indices.push_back(second);
			indices.push_back(second + 1);
			indices.push_back(first + 1);
		}
	}
	// Setup OpenGL buffers with the generated data
	setupMesh();
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