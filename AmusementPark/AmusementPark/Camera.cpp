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
    //look_location.y = location.y + sin(mouseY);
    //look_location.x = location.x + cos(mouseX) * cos(mouseY);
    //look_location.z = location.z + sin(mouseX) * cos(mouseY);

    if (perspect) {
        // 뷰 행렬 설정
        glm::mat4 view_matrix = glm::lookAt(location, look_location, glm::vec3(0.0, 1.0, 0.0));
        glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view_matrix));

        // 원근 투영 행렬 설정
        float fovy = glm::radians(45.0f);  // 45도 시야각
        float aspect = (float)1000 / (float)1000;  // 화면 비율 (동적 설정)
        float _near = 0.1f;                 // 가까운 클리핑 평면
        float _far = 1000.0f;               // 먼 클리핑 평면

        glm::mat4 projection = glm::perspective(fovy, aspect, _near, _far);
        glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // viewPos 전달 (카메라의 월드 좌표)
        glUniform3f(glGetUniformLocation(shader, "viewPos"), location.x, location.y, location.z);
    }
    else if (ortho) {
        glm::mat4 view_matrix = glm::lookAt(location, look_location, glm::vec3(0.0, 1.0, 0.0));
        glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view_matrix));

        // 직교 투영 설정 (동적 범위 설정)
        glm::mat4 projection = glm::ortho(0.0f, 2000.0f, 0.0f, 2000.0f, -1.0f, 1.0f);
        glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // viewPos 전달 (카메라의 월드 좌표)
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
    mouseY -= sensitivity * glm::radians(y); // 위아래 방향 반전

    // Y축 제한
    if (glm::degrees(mouseY) > 89.0f) mouseY = glm::radians(89.0f);
    if (glm::degrees(mouseY) < -89.0f) mouseY = glm::radians(-89.0f);
}


Camera::Camera()
{
}

Camera::~Camera()
{
}
