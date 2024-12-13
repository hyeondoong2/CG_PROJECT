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
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(shaderProgramID, "textureSampler"), 0);

	// ī�޶� �� ���� ����
	camera->DoWorking(shaderProgramID);
	m_light->LightWorks(shaderProgramID);

	for (auto& v : m_ObjectMgr->GetAllObjs()) {
		if (v->GetType() == cloud) continue; // ������ �ǳʶ�
		// �� ��ȯ ��� ����
		glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "transform"), 1, GL_FALSE, glm::value_ptr(v->modelMatrix));

		// ���� ������ ����
		glm::vec3 color = v->GetColor();
		glUniform3f(glGetUniformLocation(shaderProgramID, "incolor"), color.r, color.g, color.b);
			glBindTexture(GL_TEXTURE_2D, v->text);

		if (v->GetType() == kitty) {
			glUniform1i(glGetUniformLocation(shaderProgramID, "useTexture"), GL_TRUE);
			glBindVertexArray(v->GetMesh()->VAO);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDrawArrays(GL_TRIANGLES, 0, v->GetMesh()->polygon_count * 3);
		}
		else {
			// VAO ���ε�
			//glBindTexture(GL_TEXTURE_2D, v->text);
			glUniform1i(glGetUniformLocation(shaderProgramID, "useTexture"), GL_FALSE);
			glBindVertexArray(v->GetMesh()->VAO);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDrawArrays(GL_TRIANGLES, 0, v->GetMesh()->polygon_count * 3);
		}

		glBindVertexArray(0); // VAO ���ε� ����
	}

	// 2. ���� ��ü(����) ������
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE); // ���� ���� ���� ��Ȱ��ȭ

	for (auto& v : m_ObjectMgr->GetAllObjs()) {
		if (v->GetType() != cloud) continue; // ������ ó��

		glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "transform"), 1, GL_FALSE, glm::value_ptr(v->modelMatrix));

		glm::vec3 color = v->GetColor();
		// ������ ���İ� ����
		glUniform1i(glGetUniformLocation(shaderProgramID, "useTexture"), GL_FALSE);
		glUniform3f(glGetUniformLocation(shaderProgramID, "incolor"), color.r, color.g, color.b);
		glUniform1f(glGetUniformLocation(shaderProgramID, "Alpha"), 0.4f); // ���� ����

		glBindVertexArray(v->GetMesh()->VAO);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // �簢�� �Ž��� ��赵 ����� ó���ǵ��� ����
		// �簢���� �� ���� �ﰢ������ ������ (����: �� �ﰢ���� �׸��ٰ� ����)
		glDrawArrays(GL_TRIANGLES, 0, v->GetMesh()->polygon_count * 3);
		glBindVertexArray(0);;
	}

	glDepthMask(GL_TRUE); // ���� ���� ���� �ٽ� Ȱ��ȭ
	glDisable(GL_BLEND); // ���� ��Ȱ��ȭ
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
