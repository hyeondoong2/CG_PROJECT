#define _CRT_SECURE_NO_WARNINGS 
//#include "MakeShader.h"
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>
#include "Renderer.h"
#include "ObjectMgr.h"
#include "Importer.h"
#include "Camera.h"
#include <math.h>

std::random_device rd;
std::default_random_engine dre(rd());
std::uniform_real_distribution<float> loc{ -1.0,1.0 };


void Mouse(int button, int state, int x, int y);
GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid TimerFunction(int value);
void SpecialKeyboard(int key, int x, int y);

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 1000

#define render_freq 10
#define M_PI 3.14159265358979323846

Renderer* render;
ObjectMgr* mgr;
Importer_obj* importer;
Camera* camera;
Light* light;

bool DepthTest = true;
bool KeyB = false;
bool KeyM = false;
bool KeyF = false;
bool KeyE = false;
bool KeyT = false;
bool KeyZ = false;
bool KeyX = false;
bool KeyY = false;
bool KeyR = false;
bool KeyA = false;
bool KeyS = false;
bool isOne = false;

bool LightOn = true;

float convertX(float x) {
	return 2.0 * x / WINDOW_WIDTH - 1.0;
}

float convertY(float y) {
	return 1.0 - (2.0 * y) / WINDOW_HEIGHT;
}


void main(int argc, char** argv)
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv);

	//--- 윈도우 출력하고 콜백함수 설정
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Amusement Park");

	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	glewInit();

	// 세이더 읽어와서 세이더 프로그램 만들기
	importer = new Importer_obj;
	mgr = new ObjectMgr(importer);
	render = new Renderer(mgr);
	camera = new Camera;
	light = new Light({ 0.0, 0.0, 0.0 });
	//camera->ortho = true;
	camera->perspect = true;
	camera->SetLocation({ 0.0, 0.0, 100.0 });
	camera->SetLookLocation({ 0.0, 0.0, 0.0 });
	render->SetCamera(camera);
	render->SetLight(light);

	mgr->AddObject(wheel_body, glm::vec3({ 0.0, -30.0, 0.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 1.0, 1.0, 1.0 }), glm::vec3({ 1.0, 1.0, 1.0 }));

	mgr->AddObject(wheel_car, glm::vec3({ 0.0, 0.0, 0.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 1.0, 1.0, 1.0 }), glm::vec3({ 1.0, 1.0, 1.0 }));



	glutDisplayFunc(drawScene);		// 출력 콜백 함수
	glutMouseFunc(Mouse);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKeyboard);
	glutReshapeFunc(Reshape);
	glutTimerFunc(render_freq, TimerFunction, 1);

	glutMainLoop();
}

//--- 출력 콜백함수

GLvoid drawScene()
{
	glClearColor(0.678f, 0.847f, 0.902f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (DepthTest) { glEnable(GL_DEPTH_TEST); }
	render->SceneRender();

	glutSwapBuffers();	// 화면에 출력하기
}

//--- 다시 그리기 콜백함수
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

GLvoid Keyboard(unsigned char key, int x, int y)
{

	glutPostRedisplay();
}

void SpecialKeyboard(int key, int x, int y) {

	glutPostRedisplay();
}

void TimerFunction(int value)
{
	for (auto& v : mgr->GetAllObjs()) {
		if (v->GetType() == wheel_car) {
			glm::mat4 orbit = glm::mat4(1.0f);
			orbit = glm::translate(orbit, glm::vec3(0.0, 0.0, 0.0));
			orbit = glm::rotate(orbit, glm::radians(1.0f), glm::vec3(0.0, 0.0, 1.0));
			orbit = glm::translate(orbit, glm::vec3(0.0, 0.0, 0.0));
			v->modelMatrix = orbit * v->modelMatrix;
		}
	}

	glutPostRedisplay();
	glutTimerFunc(render_freq, TimerFunction, 1);
}

void Mouse(int button, int state, int x, int y)
{

	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN) {

		}
		else if (state == GLUT_UP) {

		}
	}
	else if (button == GLUT_RIGHT_BUTTON)
	{
		if (state == GLUT_DOWN) {
		}
	}

	glutPostRedisplay();
}