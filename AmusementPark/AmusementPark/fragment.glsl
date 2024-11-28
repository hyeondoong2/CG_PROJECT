#version 330 core

in vec3 out_Color;  // 정점 셰이더에서 전달된 색상
in vec3 FragPos;    // 정점 위치 (월드 좌표계)
in vec3 Normal;     // 정점 법선 벡터 (월드 좌표계)
in vec2 TexCoords;

out vec4 FragColor; // 최종 출력할 색상

// 조명 관련 uniform 변수
uniform vec3 lightPos;      // 조명 위치
uniform vec3 viewPos;       // 카메라 위치
uniform vec3 lightColor;    // 조명 색상
uniform vec3 objectColor;   // 객체 색상

uniform sampler2D u_BaseColor;
uniform sampler2D u_NormalMap;
uniform sampler2D u_Emissive;

void main(void) 
{
    //vec3 texColor = texture(texture1, TexCoords).rgb;

    // 1. Ambient (환경광)
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;

    // 2. Diffuse (난반사)
    vec3 normalVector = normalize(Normal);       // 정규화된 법선 벡터
    vec3 lightDir = normalize(lightPos - FragPos.xyz); // 광원 방향
    float diffuseStrength = max(dot(normalVector, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * lightColor;


    // 3. Specular (반사광)
    float specularStrength = 0.5;                // 반사광 강도
    int shininess = 190;                         // 반짝임의 정도
    vec3 viewDir = normalize(viewPos - FragPos.xyz); // 뷰어 방향
    vec3 reflectDir = reflect(-lightDir, normalVector); // 반사 벡터
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

    vec3 specular = specularStrength * spec * out_Color;

    // 최종 조명 색상 계산
    vec3 result = (ambient + diffuse + specular) * out_Color;
    FragColor = vec4(result, 1.0);
}
