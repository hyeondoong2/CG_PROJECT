#version 330 core

layout (location = 0) in vec3 in_Position;  //--- ��ġ ����:attribute position 0
layout (location = 1) in vec2 vTexPos;  //--- �÷� ����:attribute position 1
layout (location = 2) in vec3 vNormal;


out vec3 out_Color;	// �÷��� fragment shader�� ���

out vec3 FragPos;  // ���: ���� ��ǥ���� ���� ��ġ
out vec3 Normal;   // ���: ���� ��ǥ���� ���� ����
out vec2 TexCoords; 

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;

uniform mat3 normalMatrix;
uniform vec3 incolor;

void main(void) 
{
	gl_Position = projection * view * transform * vec4 (in_Position, 1.0);
	FragPos = vec3(transform * vec4(in_Position, 1.0));
	Normal = vNormal;
	out_Color = incolor;
	TexCoords = vTexPos;
}