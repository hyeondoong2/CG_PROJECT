#pragma once
#include "OpenGL.h"

class Camera
{
private:
	float mouseX = 0.0f;
	float mouseY = 0.0f;
	float sensitivity = 0.01f; // ¸¶¿ì½º °¨µµ

	float camera_arm_length{ 20.f };

public:

	glm::mat4 camMatrix = glm::mat4(1.0f);
	glm::vec3 location;
	glm::vec3 rotation;
	glm::vec3 look_location;

	void SetLocation(glm::vec3 _location);
	glm::vec3 GetLocation();

	void SetLookLocation(glm::vec3 _location);
	glm::vec3 GetLookLocation();

	void RotateCam(char axis, float angle);
	void DoWorking(GLuint shader);

	void ChangeLocation(glm::vec3 _location);
	void ChangeLookLocation(glm::vec3 _location);

	void SetRotation(float x, float y);

	bool ortho = false;
	bool perspect = true;



	Camera();
	~Camera();
};

