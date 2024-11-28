#pragma once
#include <iostream>

#include "OpenGL.h"

class Light
{
	std::string type;
	glm::vec3 location;
	glm::vec3 rotation;
	glm::vec3 color;
	float distance;


public:
	glm::mat4 modelMatrix = glm::mat4(1.0f);

	glm::vec3 GetLocation() const;
	glm::vec3 GetLightColor() const;
	float GetLightDistance() const;

	void LightWorks(GLuint Shader);
	void ChangeColor(glm::vec3 _color);
	void Brightness(float bright);


	Light(glm::vec3 xyz);
	~Light();
};

