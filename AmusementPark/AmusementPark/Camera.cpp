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
	// ī�޶� ������ �߽��� (��: ����)
	glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);

	// ī�޶� ��ġ�� �߽��� ������ ������
	glm::vec3 offset = location - center;

	// ���� ����
	float orbitSpeed = glm::radians(angle); // r_speed�� �������� ��ȯ

	// y���� �������� offset ȸ��
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), orbitSpeed, glm::vec3(0.0, 1.0, 0.0));
	offset = glm::vec3(rotationMatrix * glm::vec4(offset, 1.0f));

	// ���ο� ī�޶� ��ġ�� �߽��� + ȸ���� offset
	location = center + offset;

	// ī�޶� �׻� �߽����� �ٶ󺸵��� ����
	look_location = center;

}

void Camera::DoWorking(GLuint shader)
{
	if (perspect) {
		// �� ��� ����
		glm::mat4 view_matrix = glm::lookAt(location, look_location, glm::vec3(0.0, 1.0, 0.0));
		glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view_matrix));

		// ���� ���� ��� ����
		float fovy = glm::radians(100.0f); // 45�� �þ߰�
		float aspect = 800.0f / 800.0f;   // ȭ�� ����
		float _near = 0.1f;                // ����� Ŭ���� ���
		float _far = 100.0f;               // �� Ŭ���� ���

		glm::mat4 projection = glm::perspective(fovy, aspect, _near, _far);
		glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		// viewPos ���� (ī�޶��� ���� ��ǥ)
		unsigned int viewPosLocation = glGetUniformLocation(shader, "viewPos");
		glUniform3f(viewPosLocation, look_location.x, location.y, location.z);
	}

	else if (ortho) {
		glm::mat4 view_matrix = glm::lookAt(location, look_location, glm::vec3(0.0, 1.0, 0.0));

		glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view_matrix));

		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -200.0f, 200.0f); //--- ���� ������ [-100.0, 100.0] �������� ����
		unsigned int projectionLocation = glGetUniformLocation(shader, "projection"); //--- ���� ��ȯ �� ����
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

		// viewPos ���� (ī�޶��� ���� ��ǥ)
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
