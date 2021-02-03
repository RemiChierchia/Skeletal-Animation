#shader vertex
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormals;
layout(location = 2) in vec2 aTexCoords;

out vec3 Normals;
out vec2 TexCoords;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	Normals = mat3(transpose(inverse(model))) * aNormals;
	TexCoords = aTexCoords;
	FragPos = vec3(model * vec4(aPos, 1.0));

	gl_Position = projection * view * vec4(FragPos, 1.0);
}


#shader fragment
#version 330 core

out vec4 FragColor;

in vec3 Normals;
in vec2 TexCoords;
in vec3 FragPos;

uniform sampler2D texture1;
uniform vec3 lightColor;
uniform vec3 lightPos;


void main()
{
	vec3 norm = normalize(Normals);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	vec4 result = vec4(diffuse, 1.0f) * texture(texture1, TexCoords);

	FragColor = result;
}