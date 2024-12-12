#version 330 core

in vec3 out_Color;   // ���� ���̴����� ���޵� ����
in vec3 FragPos;     // ���� ��ġ (���� ��ǥ��)
in vec3 Normal;      // ���� ���� ���� (���� ��ǥ��)

out vec4 FragColor;  // ���� ����� ����

// ���� ���� uniform ����
uniform vec3 lightPos;       // ���� ��ġ
uniform vec3 viewPos;        // ī�޶� ��ġ
uniform vec3 lightColor;     // ���� ����
uniform float Alpha;         // ���İ� (����)

void main(void) 
{
    // 1. Ambient (ȯ�汤)
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;

    // 2. Diffuse (���ݻ�)
    vec3 normalVector = normalize(Normal);       
    vec3 lightDir = normalize(lightPos - FragPos);
    float diffuseStrength = max(dot(normalVector, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * lightColor;

    // 3. Specular (�ݻ籤)
    float specularStrength = 0.5;                
    int shininess = 150;                         
    vec3 viewDir = normalize(viewPos - FragPos); 
    vec3 reflectDir = reflect(-lightDir, normalVector);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

    vec3 specular = specularStrength * spec * lightColor;

    // ���� ���� ���� ���
    vec3 result = (ambient + diffuse + specular) * out_Color;

    // ���� ���� ����
    float gamma = 2.2;
    vec3 gammaCorrected = pow(result, vec3(1.0 / gamma));

    // ���İ��� ������ ���� ����
    FragColor = vec4(gammaCorrected, Alpha);
}
