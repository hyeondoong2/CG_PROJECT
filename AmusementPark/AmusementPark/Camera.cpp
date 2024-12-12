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
    // Ä«¸Þ¶ó°¡ °øÀüÇÒ Áß½ÉÁ¡ (¿¹: ¿øÁ¡)
    glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);

    // Ä«¸Þ¶ó À§Ä¡¿Í Áß½ÉÁ¡ »çÀÌÀÇ ¿ÀÇÁ¼Â
    glm::vec3 offset = location - center;

    // °øÀü °¢µµ
    float orbitSpeed = glm::radians(angle); // r_speed¸¦ ¶óµð¾ÈÀ¸·Î º¯È¯

    // yÃàÀ» ±âÁØÀ¸·Î offset È¸Àü
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), orbitSpeed, glm::vec3(0.0, 1.0, 0.0));
    offset = glm::vec3(rotationMatrix * glm::vec4(offset, 1.0f));

    // »õ·Î¿î Ä«¸Þ¶ó À§Ä¡´Â Áß½ÉÁ¡ + È¸ÀüµÈ offset
    location = center + offset;

    // Ä«¸Þ¶ó°¡ Ç×»ó Áß½ÉÁ¡À» ¹Ù¶óº¸µµ·Ï ¼³Á¤
    look_location = center;

}

void Camera::DoWorking(GLuint shader)
{
    //look_location.y = location.y + sin(mouseY);
    //look_location.x = location.x + cos(mouseX) * cos(mouseY);
    //look_location.z = location.z + sin(mouseX) * cos(mouseY);

    if (perspect) {
        // ºä Çà·Ä ¼³Á¤
        glm::mat4 view_matrix = glm::lookAt(location, look_location, glm::vec3(0.0, 1.0, 0.0));
        glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view_matrix));

        // ¿ø±Ù Åõ¿µ Çà·Ä ¼³Á¤
        float fovy = glm::radians(45.0f);  // 45µµ ½Ã¾ß°¢
        float aspect = (float)1000 / (float)1000;  // È­¸é ºñÀ² (µ¿Àû ¼³Á¤)
        float _near = 0.1f;                 // °¡±î¿î Å¬¸®ÇÎ Æò¸é
        float _far = 1000.0f;               // ¸Õ Å¬¸®ÇÎ Æò¸é

        glm::mat4 projection = glm::perspective(fovy, aspect, _near, _far);
        glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // viewPos Àü´Þ (Ä«¸Þ¶óÀÇ ¿ùµå ÁÂÇ¥)
        glUniform3f(glGetUniformLocation(shader, "viewPos"), location.x, location.y, location.z);
    }
    else if (ortho) {
        glm::mat4 view_matrix = glm::lookAt(location, look_location, glm::vec3(0.0, 1.0, 0.0));
        glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view_matrix));

        // Á÷±³ Åõ¿µ ¼³Á¤ (µ¿Àû ¹üÀ§ ¼³Á¤)
        glm::mat4 projection = glm::ortho(0.0f, 2000.0f, 0.0f, 2000.0f, -1.0f, 1.0f);
        glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // viewPos Àü´Þ (Ä«¸Þ¶óÀÇ ¿ùµå ÁÂÇ¥)
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
