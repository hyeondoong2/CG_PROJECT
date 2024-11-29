#pragma once
#include "OpenGL.h"

class Camera
{
private:
public:

	glm::mat4 camMatrix = glm::mat4(1.0f);
	glm::vec3 location;
	glm::vec3 look_location;

	void SetLocation(glm::vec3 _location);
	glm::vec3 GetLocation();

	void SetLookLocation(glm::vec3 _location);
	glm::vec3 GetLookLocation();

	void RotateCam(float angle);
	void DoWorking(GLuint shader);

	bool ortho = false;
	bool perspect = true;



	Camera();
	~Camera();
};

