#include "Light.h"

Light::Light(glm::vec3 xyz)
{
	type = "none";
	location = xyz;
	rotation = glm::vec3{ 0 };
	color = glm::vec3{ 1.0, 1.0, 1.0 };
	distance = 200.f;

	modelMatrix = glm::translate(modelMatrix, location);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1.0, 0.0, 0.0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0.0, 1.0, 0.0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0.0, 0.0, 1.0));
}

Light::~Light()
{
}

glm::vec3 Light::GetLocation() const
{
	return location;
}

glm::vec3 Light::GetLightColor() const
{
	return color;
}

float Light::GetLightDistance() const
{
	return distance;
}

void Light::LightWorks(GLuint Shader)
{
	unsigned int lightPosLocation = glGetUniformLocation(Shader, "lightPos");
	//glm::vec3 light_location = GetLocation();
	glm::vec3 light_location = modelMatrix[3];
	glUniform3f(lightPosLocation, light_location.x, light_location.y, light_location.z);

	unsigned int lightColorLocation = glGetUniformLocation(Shader, "lightColor");
	glm::vec3 light_color = GetLightColor();
	glUniform3f(lightColorLocation, light_color.r, light_color.g, light_color.b);

	unsigned int lightDistanceLocation = glGetUniformLocation(Shader, "lightDistance");
	glUniform1f(lightDistanceLocation, GetLightDistance());
}

void Light::ChangeColor(glm::vec3 _color)
{
	color = _color;
}

void Light::Brightness(float bright)
{
	color.r += bright;
	color.g += bright;
	color.b += bright;
}
