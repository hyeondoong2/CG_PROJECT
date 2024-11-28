#include "Renderer.h"
#include "fileReader.h"
#include <random>

#define M_PI 3.14159265358979323846


Renderer::Renderer(ObjectMgr* Mgr)
{
	Initialize();
	m_ObjectMgr = Mgr;
}

Renderer::~Renderer()
{
}

void Renderer::SceneRender()
{
	glUseProgram(shaderProgramID);

	camera->DoWorking(shaderProgramID);

	m_light->LightWorks(shaderProgramID);

	for (auto& v : m_ObjectMgr->GetAllObjs()) {

		glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "transform"), 1, GL_FALSE, glm::value_ptr(v->modelMatrix));

		glm::vec3 color = v->GetColor();
		glUniform3f(glGetUniformLocation(shaderProgramID, "incolor"), color.r, color.g, color.b);

		
		//// �ؽ�ó ���ε� �� ������ ����
		//  // BaseColor �ؽ�ó ó��
		//if (v->GetMaterial()->BaseColorID != 0) {
		//	glUniform1i(glGetUniformLocation(shaderProgramID, "u_BaseColor"), 0);
		//	glActiveTexture(GL_TEXTURE0);
		//	glBindTexture(GL_TEXTURE_2D, v->GetMaterial()->BaseColorID);  // �ؽ�ó ���ε�
		//}
		//else {
		//	std::cout << "empty" << '\n';
		//}

		//// NormalMap �ؽ�ó ó��
		//if (v->GetMaterial()->NormalMapID != 0) {
		//	glUniform1i(glGetUniformLocation(shaderProgramID, "u_NormalMap"), 1);
		//	glActiveTexture(GL_TEXTURE1);
		//	glBindTexture(GL_TEXTURE_2D, v->GetMaterial()->NormalMapID);  // ��� �� �ؽ�ó ���ε�
		//}

		//// Emissive �ؽ�ó ó��
		//if (v->GetMaterial()->EmissiveID != 0) {
		//	glUniform1i(glGetUniformLocation(shaderProgramID, "u_Emissive"), 2);
		//	glActiveTexture(GL_TEXTURE2);
		//	glBindTexture(GL_TEXTURE_2D, v->GetMaterial()->EmissiveID);  // �̹̽ú� �ؽ�ó ���ε�
		//}

		glBindVertexArray(v->GetMesh()->VAO);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawArrays(GL_TRIANGLES, 0, v->GetMesh()->polygon_count * 3);
		glBindVertexArray(0);
	}

}

void Renderer::SetCamera(Camera* cam)
{
	camera = cam;
}

void Renderer::SetLight(Light* lights)
{
	m_light = lights;
}


void Renderer::make_vertexShaders()
{
	vertexSource = filetobuf("vertex.glsl");	// ���ؽ� ���̴� �о����

	vertexShader = glCreateShader(GL_VERTEX_SHADER);	// ���ؽ� ���̴� ��ü �����

	glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);	// ���̴� �ڵ带 ���̴� ��ü�� �ֱ�
	glCompileShader(vertexShader);	// ���ؽ� ���̴� ������

	// �������� ����� ���� ���� ��� : ����üũ
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader ������ ����\n" << errorLog << std::endl;
		return;
	}
}


void Renderer::make_fragmentShaders()
{
	fragmentSource = filetobuf("fragment.glsl");	// �����׸�Ʈ ���̴� �о����

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);	// �����׸�Ʈ ���̴� ��ü �����

	glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);	// ���̴� �ڵ带 ���̴� ��ü�� �ֱ�
	glCompileShader(fragmentShader);	// �����׸�Ʈ ���̴� ������

	// �������� ����� ���� ���� ��� : ������ ���� üũ
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: fragment shader ������ ����\n" << errorLog << std::endl;
		return;
	}
}

void Renderer::make_shaderProgram()
{
	// 1. �Ϲ� ��ü�� ���̴� ���α׷�
	make_vertexShaders();       // �Ϲ� ��ü�� ���ؽ� ���̴� ����
	make_fragmentShaders();     // �����׸�Ʈ ���̴� ����

	shaderProgramID = glCreateProgram();  // �Ϲ� ��ü�� ���̴� ���α׷� ����

	glAttachShader(shaderProgramID, vertexShader);    // ��ü�� ���ؽ� ���̴� ���̱�
	glAttachShader(shaderProgramID, fragmentShader);  // �����׸�Ʈ ���̴� ���̱�

	glLinkProgram(shaderProgramID);  // ��ü�� ���̴� ���α׷� ��ũ

	// ���̴� ��ü ���� (���̴� ���α׷��� ��ũ�Ǿ����Ƿ� �ʿ� ����)
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glUseProgram(shaderProgramID);
}

void Renderer::Initialize()
{
	make_shaderProgram();
}
