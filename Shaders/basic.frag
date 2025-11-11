#version 330 core

// Input from vertex shader
in VS_OUT {
	vec3 fragNormal;
	vec3 fragPos;
} fs_in;

// Output
out vec4 FragColor;

// Uniforms
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 objectColor;

void main()
{
	// Ambient
	float ambientStrength = 0.3;
	vec3 ambient = ambientStrength * objectColor;

	// Diffuse
	vec3 norm = normalize(fs_in.fragNormal);
	vec3 lightDir = normalize(lightPos - fs_in.fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * objectColor;

	// Specular
	float specularStrength = 0.5;
	vec3 viewDir = normalize(viewPos - fs_in.fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
	vec3 specular = specularStrength * spec * vec3(1.0);

	// Combine
	vec3 result = ambient + diffuse + specular;
	FragColor = vec4(result, 1.0);
}
