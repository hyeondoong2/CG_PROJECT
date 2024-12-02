#include "Camera.h"

void Camera::SetLocation(glm::vec3 _location)
{
    camMatrix = glm::mat4(1.0f);
    camMatrix = glm::translate(camMatrix, _location);
}

glm::mat4 Camera::GetLocation()
{
	return camMatrix;
}

void Camera::SetLookLocation(glm::vec3 _location)
{
    cam_Look_Matrix = glm::mat4(1.0f);
    cam_Look_Matrix = glm::translate(cam_Look_Matrix, _location);
}

glm::mat4 Camera::GetLookLocation()
{
	return cam_Look_Matrix;
}

void Camera::RotateCam(float angle)
{

}

void Camera::DoWorking(GLuint shader)
{
    //look_location.y = location.y + sin(mouseY);
    //look_location.x = location.x + cos(mouseX) * cos(mouseY);
    //look_location.z = location.z + sin(mouseX) * cos(mouseY);

    if (perspect) {
        // �� ��� ����
        glm::mat4 view_matrix = glm::lookAt(glm::vec3(camMatrix[3]), glm::vec3(cam_Look_Matrix[3]), glm::vec3(0.0, 1.0, 0.0));
        glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view_matrix));

        // ���� ���� ��� ����
        float fovy = glm::radians(50.0f);  // 45�� �þ߰�
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
    //glm::mat4 move = glm::mat4(1.0f);
    //move = glm::translate(move, _location);
    //camMatrix = move * camMatrix;
    camMatrix = glm::translate(camMatrix, _location);
}

void Camera::ChangeLookLocation(glm::vec3 _location)
{
    //glm::mat4 move = glm::mat4(1.0f);
    //move = glm::translate(move, _location);
    //cam_Look_Matrix = move * cam_Look_Matrix;
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
