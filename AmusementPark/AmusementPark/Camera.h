#pragma once
#include "OpenGL.h"

class Camera
{
private:
	float mouseX = 0.0f;
	float mouseY = 0.0f;
	float sensitivity = 0.01f; // 마우스 감도

	float camera_arm_length{ 20.f };

public:

	glm::mat4 camMatrix = glm::mat4(1.0f);
	glm::mat4 cam_Look_Matrix = glm::mat4(1.0f);

	glm::mat4 Initial_camMatrix = glm::mat4(1.0f);
	glm::mat4 Initial_cam_Look_Matrix = glm::mat4(1.0f);

	glm::vec3 location;
	glm::vec3 rotation;
	glm::vec3 look_location;

	void SetLocation(glm::vec3 _location);
	glm::mat4 GetLocation();

	void SetLookLocation(glm::vec3 _location);
	glm::mat4 GetLookLocation();

	void RotateCamY(float angle, glm::vec3 pos);
	void RotateCamZ(float angle, glm::vec3 pos);
	void DoWorking(GLuint shader);

	void ChangeLocation(glm::vec3 _location);
	void ChangeLookLocation(glm::vec3 _location);

	void SetRotation(float x, float y);

	bool ortho = false;
	bool perspect = true;

	float angle = 70.0f;


	Camera();
	~Camera();
};

