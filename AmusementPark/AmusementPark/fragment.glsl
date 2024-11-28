#version 330 core

in vec3 out_Color;  // ���� ���̴����� ���޵� ����
in vec3 FragPos;    // ���� ��ġ (���� ��ǥ��)
in vec3 Normal;     // ���� ���� ���� (���� ��ǥ��)
in vec2 TexCoords;

out vec4 FragColor; // ���� ����� ����

// ���� ���� uniform ����
uniform vec3 lightPos;      // ���� ��ġ
uniform vec3 viewPos;       // ī�޶� ��ġ
uniform vec3 lightColor;    // ���� ����
uniform vec3 objectColor;   // ��ü ����

uniform sampler2D u_BaseColor;
uniform sampler2D u_NormalMap;
uniform sampler2D u_Emissive;

void main(void) 
{
    //vec3 texColor = texture(texture1, TexCoords).rgb;

    // 1. Ambient (ȯ�汤)
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;

    // 2. Diffuse (���ݻ�)
    vec3 normalVector = normalize(Normal);       // ����ȭ�� ���� ����
    vec3 lightDir = normalize(lightPos - FragPos.xyz); // ���� ����
    float diffuseStrength = max(dot(normalVector, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * lightColor;


    // 3. Specular (�ݻ籤)
    float specularStrength = 0.5;                // �ݻ籤 ����
    int shininess = 190;                         // ��¦���� ����
    vec3 viewDir = normalize(viewPos - FragPos.xyz); // ��� ����
    vec3 reflectDir = reflect(-lightDir, normalVector); // �ݻ� ����
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

    vec3 specular = specularStrength * spec * out_Color;

    // ���� ���� ���� ���
    vec3 result = (ambient + diffuse + specular) * out_Color;
    FragColor = vec4(result, 1.0);
}
