#include "Camera.h"

void Camera::SetLocation(glm::vec3 _location)
{
    camMatrix = glm::mat4(1.0f);
    camMatrix = glm::translate(camMatrix, _location);

    Initial_camMatrix = glm::mat4(1.0f);
    Initial_camMatrix = glm::translate(Initial_camMatrix, _location);
}

glm::mat4 Camera::GetLocation()
{
	return camMatrix;
}

void Camera::SetLookLocation(glm::vec3 _location)
{
    cam_Look_Matrix = glm::mat4(1.0f);
    cam_Look_Matrix = glm::translate(cam_Look_Matrix, _location);

    Initial_cam_Look_Matrix = glm::mat4(1.0f);
    Initial_cam_Look_Matrix = glm::translate(Initial_cam_Look_Matrix, _location);
}

glm::mat4 Camera::GetLookLocation()
{
	return cam_Look_Matrix;
}

void Camera::RotateCamY(float angle, glm::vec3 pos)
{
    // ���� ī�޶� ��ġ�� �������� ��ȯ ����
    glm::mat4 orbit = glm::mat4(1.0f);

    // ȸ�� �߽����� �̵�
    orbit = glm::translate(orbit, pos);

    // Y�� �������� ȸ��
    orbit = glm::rotate(orbit, glm::radians(angle), glm::vec3(0.0, 1.0, 0.0));

    // ȸ�� �߽ɿ��� �ٽ� ���� ��ġ�� �̵�
    orbit = glm::translate(orbit, -pos);

    // ī�޶� ��ȯ ��Ŀ� ����

    camMatrix = orbit * camMatrix;
    cam_Look_Matrix = orbit * cam_Look_Matrix;
}

void Camera::RotateCamZ(float angle, glm::vec3 pos)
{
    // ���� ī�޶� ��ġ�� �������� ��ȯ ����
    glm::mat4 orbit = glm::mat4(1.0f);

    // ȸ�� �߽����� �̵�
    orbit = glm::translate(orbit, pos);

    // Y�� �������� ȸ��
    orbit = glm::rotate(orbit, glm::radians(angle), glm::vec3(0.0, 0.0, 1.0));

    // ȸ�� �߽ɿ��� �ٽ� ���� ��ġ�� �̵�
    orbit = glm::translate(orbit, -pos);

    // ī�޶� ��ȯ ��Ŀ� ����

    camMatrix = orbit * camMatrix;
    cam_Look_Matrix = orbit * cam_Look_Matrix;
}

void Camera::DoWorking(GLuint shader)
{
    if (perspect) {
        // �� ��� ����
        glm::mat4 view_matrix = glm::lookAt(glm::vec3(camMatrix[3]), glm::vec3(cam_Look_Matrix[3]), glm::vec3(0.0, 1.0, 0.0));
        glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view_matrix));

        // ���� ���� ��� ����
        float fovy = glm::radians(angle);  // 45�� �þ߰�
        float aspect = (float)1000 / (float)1000;  // ȭ�� ���� (���� ����)
        float _near = 0.1f;                 // ����� Ŭ���� ���
        float _far = 1000.0f;               // �� Ŭ���� ���

        glm::mat4 projection = glm::perspective(fovy, aspect, _near, _far);
        glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    }
    else if (ortho) {
        glm::mat4 view_matrix = glm::lookAt(glm::vec3(camMatrix[3]), glm::vec3(cam_Look_Matrix[3]), glm::vec3(0.0, 1.0, 0.0));
        glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view_matrix));

        // ���� ���� ���� (���� ���� ����)
        glm::mat4 projection = glm::ortho(0.0f, 2000.0f, 0.0f, 2000.0f, -1.0f, 1.0f);
        glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    }

    // ī�޶��� ���� ��ǥ ���� (�� ����� ������ ��)
    glm::vec3 cam_position = glm::vec3(camMatrix[3]);
    glUniform3f(glGetUniformLocation(shader, "viewPos"), cam_position.x, cam_position.y, cam_position.z);
}

void Camera::ChangeLocation(glm::vec3 _location)
{
    camMatrix = glm::translate(camMatrix, _location);
}

void Camera::ChangeLookLocation(glm::vec3 _location)
{
    cam_Look_Matrix = glm::translate(cam_Look_Matrix, _location);
}

void Camera::SetRotation(float x, float y)
{
    mouseX += sensitivity * glm::radians(x);
    mouseY -= sensitivity * glm::radians(y); // ���Ʒ� ���� ����

    // Y�� ����
    if (glm::degrees(mouseY) > 89.0f) mouseY = glm::radians(89.0f);
    if (glm::degrees(mouseY) < -89.0f) mouseY = glm::radians(-89.0f);
}


Camera::Camera()
{
}

Camera::~Camera()
{
}
