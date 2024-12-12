#include "Camera.h"

void Camera::SetLocation(glm::vec3 _location)
{
	location = _location;
}

glm::vec3 Camera::GetLocation()
{
	return location;
}

void Camera::SetLookLocation(glm::vec3 _location)
{
	look_location = _location;
}

glm::vec3 Camera::GetLookLocation()
{
	return look_location;
}

void Camera::RotateCam(char axis, float angle)
{
	// 원점(0, 0, 0)을 기준으로 회전 (중심: 원점)
	glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);

	// 카메라의 현재 위치에서 원점까지의 벡터(위치와 원점의 차이)
	glm::vec3 offset = location - center;

	// 회전 각도 (각도 -> 라디안으로 변환)
	float orbitSpeed = glm::radians(angle); // 회전 속도를 각도로 설정

	glm::mat4 rotationMatrix;
	// y축을 기준으로 회전 행렬을 만든다
	switch (axis) {
	case'y':
		 rotationMatrix = glm::rotate(glm::mat4(1.0f), orbitSpeed, glm::vec3(0.0, 1.0, 0.0));
		break;
	case'x':
		 rotationMatrix = glm::rotate(glm::mat4(1.0f), orbitSpeed, glm::vec3(1.0, 0.0, 0.0));
		break;
	}

	// 위치 벡터(offset)를 회전 행렬을 사용하여 회전시킨다
	offset = glm::vec3(rotationMatrix * glm::vec4(offset, 1.0f));

	// 회전 후 새로운 위치 = 원점 + 회전된 벡터
	location = center + offset;

	// 카메라는 여전히 원점을 바라보게 설정
	look_location = center;
}


void Camera::DoWorking(GLuint shader)
{
	// 카메라의 위치(location)와 바라보는 위치(look_location)에 따라, 카메라의 시점(view matrix)을 설정합니다.

	// 원근법을 사용하는 경우
	if (perspect) {
		// glm::lookAt 함수로 카메라의 뷰 행렬 생성
		glm::mat4 view_matrix = glm::lookAt(location, look_location, glm::vec3(0.0, 1.0, 0.0));
		glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view_matrix));

		// 원근 투영 행렬 설정 (fovy = 45도, 화면 비율, 근거리 및 원거리)
		float fovy = glm::radians(45.0f);  // 시야각(fovy) 45도
		float aspect = (float)1000 / (float)1000;  // 화면 비율: 1000 x 1000 화면 크기
		float _near = 0.1f;  // 근거리: 0.1
		float _far = 1000.0f;  // 원거리: 1000

		glm::mat4 projection = glm::perspective(fovy, aspect, _near, _far);
		glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		// 카메라의 위치(location)을 viewPos로 셰이더에 전달 (카메라의 위치는 뷰 행렬과 함께 사용)
		glUniform3f(glGetUniformLocation(shader, "viewPos"), location.x, location.y, location.z);
	}
	// 직교법을 사용하는 경우
	else if (ortho) {
		// 직교 투영을 위한 뷰 행렬 생성
		glm::mat4 view_matrix = glm::lookAt(location, look_location, glm::vec3(0.0, 1.0, 0.0));
		glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view_matrix));

		// 직교 투영 행렬 설정 (투영 영역 설정: 0 ~ 2000 x 0 ~ 2000 범위, 깊이 -1.0 ~ 1.0)
		glm::mat4 projection = glm::ortho(0.0f, 2000.0f, 0.0f, 2000.0f, -1.0f, 1.0f);
		glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		// 카메라 위치를 viewPos로 셰이더에 전달 (카메라의 위치 정보)
		glUniform3f(glGetUniformLocation(shader, "viewPos"), location.x, location.y, location.z);
	}
}


void Camera::ChangeLocation(glm::vec3 _location)
{
	location.x += _location.x;
	location.y += _location.y;
	location.z += _location.z;
}

void Camera::ChangeLookLocation(glm::vec3 _location)
{
	look_location.x += _location.x;
	look_location.y += _location.y;
	look_location.z += _location.z;
}

void Camera::SetRotation(float x, float y)
{
	mouseX += sensitivity * glm::radians(x);
	mouseY -= sensitivity * glm::radians(y); // À§¾Æ·¡ ¹æÇâ ¹ÝÀü

	// YÃà Á¦ÇÑ
	if (glm::degrees(mouseY) > 89.0f) mouseY = glm::radians(89.0f);
	if (glm::degrees(mouseY) < -89.0f) mouseY = glm::radians(-89.0f);
}


Camera::Camera()
{
}

Camera::~Camera()
{
}
