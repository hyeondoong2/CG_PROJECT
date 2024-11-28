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

glm::vec3 ship_pos{};

Renderer* render;
ObjectMgr* mgr;
Importer_obj* importer;
Camera* camera;
Light* light;

static float currentAngle = -1.0f;   // 현재 각도
static bool increasing = true;      // 각도 증가 방향
float minAngle = -1.0f;            // 최소 각도 (시작 지점)
float maxAngle = 1.0f;             // 최대 각도 (끝 지점)
float angleStep = 0.01f;             // 한 프레임당 각도 변화량 (속도)

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
	light = new Light({ 200.0, 200.0, 200.0 });
	//camera->ortho = true;
	camera->perspect = true;
	camera->SetLocation({ 0.0, 120.0, 150.0 });
	camera->SetLookLocation({ 0.0, 0.0, 0.0 });
	render->SetCamera(camera);
	render->SetLight(light);

	mgr->AddObject(base, glm::vec3({ 0.0, -40.0, -70.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 30.0, 1.0, 30.0 }), glm::vec3({ 0.4, 1.0, 0.0 }));

	mgr->AddObject(wheel_body, glm::vec3({ 0.0, -40.0, -120.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 1.0, 1.0, 1.0 }), glm::vec3({ 1.0, 1.0, 1.0 }));

	mgr->AddObject(wheel_car, glm::vec3({ 0.0, -10.0, -120.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 0.8, 0.8, 0.8 }), glm::vec3({ 1.0, 1.0, 1.0 }));

	mgr->AddObject(viking_body, glm::vec3({ -30.0, -30.0, -30.0 }), glm::vec3({ 0.0, 90.0, 0.0 }),
		glm::vec3({ 3.0, 3.0, 3.0 }), glm::vec3({ 1.0, 1.0, 1.0 }));

	mgr->AddObject(viking_ship, glm::vec3({ -30.0, -30.0, -30.0 }), glm::vec3({ 0.0, 90.0, 0.0 }),
		glm::vec3({ 3.0, 3.0, 3.0 }), glm::vec3({ 1.0, 1.0, 1.0 }));

	mgr->AddObject(merry_go_round_body, glm::vec3({ 30.0, -30.0, -30.0 }), glm::vec3({ 0.0, 90.0, 0.0 }),
		glm::vec3({ 1.0, 1.0, 1.0 }), glm::vec3((1.0, 1.0, 1.0)));

	mgr->AddObject(merry_go_round_horse, glm::vec3({ 30.0, -30.0, -30.0 }), glm::vec3({ 0.0, 90.0, 0.0 }),
		glm::vec3({ 1.0, 1.0, 1.0 }), glm::vec3({ 1.0, 1.0, 1.0 }));

	ship_pos = glm::vec3({ -30.0, 0.0, -10.0 });

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
	glEnable(GL_DEPTH_TEST);  // 깊이 테스트 활성화
	glClear(GL_DEPTH_BUFFER_BIT);  // 깊이 버퍼 초기화
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
	// 회전 각도 업데이트

	for (auto& v : mgr->GetAllObjs()) {
		if (v->GetType() == wheel_car) {
			glm::mat4 orbit = glm::mat4(1.0f);
			orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
			orbit = glm::rotate(orbit, glm::radians(1.0f), glm::vec3(0.0, 0.0, 1.0));
			orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
			v->modelMatrix = orbit * v->modelMatrix;
		}

		else if (v->GetType() == merry_go_round_horse) {
			glm::mat4 orbit = glm::mat4(1.0f);
			orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
			orbit = glm::rotate(orbit, glm::radians(1.0f), glm::vec3(0.0, 1.0, 0.0));
			orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
			v->modelMatrix = orbit * v->modelMatrix;
		}

		if (v->GetType() == viking_ship) {
			// 회전 각도 업데이트
			if (increasing) {
				currentAngle += angleStep;
				if (currentAngle >= maxAngle) {
					increasing = false; // 최대 각도에 도달하면 방향 전환
				}
			}
			else {
				currentAngle -= angleStep;
				if (currentAngle <= minAngle) {
					increasing = true;  // 최소 각도에 도달하면 방향 전환
				}
			}

			glm::mat4 orbit = glm::mat4(1.0f);
			orbit = glm::translate(orbit, glm::vec3(ship_pos));
			orbit = glm::rotate(orbit, glm::radians(currentAngle), glm::vec3(0.0, 0.0, 1.0));
			orbit = glm::translate(orbit, glm::vec3(-ship_pos));
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