#version 330 core

// Input vertex attributes
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

// Output to fragment shader
out VS_OUT {
	vec3 fragNormal;
	vec3 fragPos;
} vs_out;

// Uniforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

void main()
{
	// Transform vertex position
	gl_Position = projection * view * model * vec4(position, 1.0);

	// Pass fragment position in world space
	vs_out.fragPos = vec3(model * vec4(position, 1.0));

	// Transform and normalize the normal
	vs_out.fragNormal = normalize(normalMatrix * normal);
}
