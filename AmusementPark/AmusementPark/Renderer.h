#pragma once
#include "OpenGL.h"
#include "ObjectMgr.h"
#include "Importer.h"
#include "Camera.h"
#include "Light.h"

class Renderer
{
	Light* m_light;

	GLuint VAO, VBO[2];
	GLuint EviromentVAO, EviromentVBO;

	GLuint vaoX;
	GLuint vboX[2];

	GLuint vaoY;
	GLuint vboY[2];

	GLuint vaoZ;
	GLuint vboZ[2];

	GLchar* vertexSource, * fragmentSource;		// �ҽ��ڵ� ���� ����
	GLuint vertexShader, fragmentShader;	// ���̴� ��ü
	GLuint shaderProgramID, lineShaderProgramID;		// ���̴� ���α׷�

	GLuint vao;

	ObjectMgr* m_ObjectMgr;
	Camera* camera;
	Importer_obj* importer;

public:
	GLenum DrawMode = GL_FILL;

	Renderer(ObjectMgr* Mgr);
	~Renderer();
	void SceneRender();
	void Render_Enviroment(GLuint Shader);
	void Initialize_EviromentVAO();

	void SetCamera(Camera* cam);
	void SetLight(Light* lights);

private:
	void make_vertexShaders();
	void make_fragmentShaders();
	void make_shaderProgram();
	void Initialize();
};

