#version 330 core

layout (location = 0) in vec3 in_Position;  //--- 위치 변수:attribute position 0
layout (location = 1) in vec2 vTexPos;  //--- 컬러 변수:attribute position 1
layout (location = 2) in vec3 vNormal;


out vec3 out_Color;	// 컬러를 fragment shader로 출력

out vec3 FragPos;  // 출력: 월드 좌표계의 정점 위치
out vec3 Normal;   // 출력: 월드 좌표계의 법선 벡터
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