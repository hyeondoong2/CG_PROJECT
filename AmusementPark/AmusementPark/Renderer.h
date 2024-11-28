#pragma once
#include "OpenGL.h"
#include "ObjectMgr.h"
#include "Camera.h"
#include "Light.h"

class Renderer
{
	Light* m_light;

	GLuint VAO, VBO[2];

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

public:
	GLenum DrawMode = GL_FILL;

	Renderer(ObjectMgr* Mgr);
	~Renderer();
	void SceneRender();

	void SetCamera(Camera* cam);
	void SetLight(Light* lights);

private:
	void make_vertexShaders();
	void make_fragmentShaders();
	void make_shaderProgram();
	void Initialize();
};

