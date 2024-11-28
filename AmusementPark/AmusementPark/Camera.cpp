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

void Camera::RotateCam(float angle)
{
	// 카메라가 공전할 중심점 (예: 원점)
	glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);

	// 카메라 위치와 중심점 사이의 오프셋
	glm::vec3 offset = location - center;

	// 공전 각도
	float orbitSpeed = glm::radians(angle); // r_speed를 라디안으로 변환

	// y축을 기준으로 offset 회전
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), orbitSpeed, glm::vec3(0.0, 1.0, 0.0));
	offset = glm::vec3(rotationMatrix * glm::vec4(offset, 1.0f));

	// 새로운 카메라 위치는 중심점 + 회전된 offset
	location = center + offset;

	// 카메라가 항상 중심점을 바라보도록 설정
	look_location = center;

}

void Camera::DoWorking(GLuint shader)
{
	if (perspect) {
		// 뷰 행렬 설정
		glm::mat4 view_matrix = glm::lookAt(location, look_location, glm::vec3(0.0, 1.0, 0.0));
		glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view_matrix));

		// 원근 투영 행렬 설정
		float fovy = glm::radians(100.0f); // 45도 시야각
		float aspect = 800.0f / 800.0f;   // 화면 비율
		float _near = 0.1f;                // 가까운 클리핑 평면
		float _far = 100.0f;               // 먼 클리핑 평면

		glm::mat4 projection = glm::perspective(fovy, aspect, _near, _far);
		glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		// viewPos 전달 (카메라의 월드 좌표)
		unsigned int viewPosLocation = glGetUniformLocation(shader, "viewPos");
		glUniform3f(viewPosLocation, look_location.x, location.y, location.z);
	}

	else if (ortho) {
		glm::mat4 view_matrix = glm::lookAt(location, look_location, glm::vec3(0.0, 1.0, 0.0));

		glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view_matrix));

		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -200.0f, 200.0f); //--- 투영 공간을 [-100.0, 100.0] 공간으로 설정
		unsigned int projectionLocation = glGetUniformLocation(shader, "projection"); //--- 투영 변환 값 설정
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

		// viewPos 전달 (카메라의 월드 좌표)
		unsigned int viewPosLocation = glGetUniformLocation(shader, "viewPos");
		glUniform3f(viewPosLocation, location.x, location.y, location.z);
	}
}
Camera::Camera()
{
}

Camera::~Camera()
{
}
