#define _CRT_SECURE_NO_WARNINGS 
//#include "MakeShader.h"
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm/glm.hpp>
#include <gl/glm/glm/ext.hpp>
#include <gl/glm/glm/gtc/matrix_transform.hpp>

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

static float currentAngle = -1.0f;   // ���� ����
static bool increasing = false;      // ���� ���� ����
float minAngle = -1.0;            // �ּ� ���� (���� ����)
float maxAngle = 1.0f;             // �ִ� ���� (�� ����)
float angleStep = 0.01f;             // �� �����Ӵ� ���� ��ȭ�� (�ӵ�)

bool LightOn = true;

// �㳷 �÷���
bool DayMode = true;
bool NightMode = false;

// ���� �ٲٱ� �÷���
bool OneMode = false;
bool ThreeMode = true;

float convertX(float x) {
	return 2.0 * x / WINDOW_WIDTH - 1.0;
}

float convertY(float y) {
	return 1.0 - (2.0 * y) / WINDOW_HEIGHT;
}


void main(int argc, char** argv)
{
	//--- ������ �����ϱ�
	glutInit(&argc, argv);

	//--- ������ ����ϰ� �ݹ��Լ� ����
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Amusement Park");

	//--- GLEW �ʱ�ȭ�ϱ�
	glewExperimental = GL_TRUE;
	glewInit();

	// ���̴� �о�ͼ� ���̴� ���α׷� �����
	importer = new Importer_obj;
	mgr = new ObjectMgr(importer);
	render = new Renderer(mgr);
	camera = new Camera;
	light = new Light({ 100.0, 100.0, 100.0 });

	//camera->ortho = true;
	camera->perspect = true;

	// ī�޶� ��ġ ����
	camera->SetLocation({ 0.0, -30.0, 120.0 });
	camera->SetLookLocation({ 0.0, 0.0, 0.0 });

	render->SetCamera(camera);
	render->SetLight(light);

	// ��
	mgr->AddObject(base, glm::vec3({ 0.0, -40.0, -70.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 40.0, 1.0, 40.0 }), glm::vec3({ 0.6f, 0.933f, 0.565f }));

	// ������
	mgr->AddObject(wheel_body, glm::vec3({ 0.0, -40.0, -70.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 1.0, 1.0, 1.0 }), glm::vec3({ 1.0f, 0.713f, 0.756f }));
	mgr->AddObject(wheel_car, glm::vec3({ 0.0, -8.0, -70.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 0.8, 0.8, 0.8 }), glm::vec3({ 0.678f, 0.902f, 1.0f }));

	// ����ŷ
	mgr->AddObject(viking_body, glm::vec3({ -50.0, -40.0, -30.0 }), glm::vec3({ 0.0, 90.0, 0.0 }),
		glm::vec3({ 3.0, 3.0, 3.0 }), glm::vec3({ 1.0f, 0.76f, 0.76f }));
	mgr->AddObject(viking_ship, glm::vec3({ -50.0, -40.0, -30.0 }), glm::vec3({ 0.0, 90.0, 0.0 }),
		glm::vec3({ 3.0, 3.0, 3.0 }), glm::vec3({ 1.0f, 0.95f, 0.8f }));


	// ȸ����
	mgr->AddObject(merry_go_round_body, glm::vec3({ 50.0, -40.0, -30.0 }), glm::vec3({ 0.0, 90.0, 0.0 }),
		glm::vec3({ 1.0, 1.0, 1.0 }), glm::vec3({ 0.87f, 0.83f, 0.95f }));
	mgr->AddObject(merry_go_round_horse, glm::vec3({ 50.0, -40.0, -30.0 }), glm::vec3({ 0.0, 90.0, 0.0 }),
		glm::vec3({ 1.2, 1.2, 1.2 }), glm::vec3({ 0.678f, 0.902f, 1.0f }));
	mgr->AddObject(merry_go_round_horse, glm::vec3({ 50.0, -40.0, -30.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 1.2, 1.2, 1.2 }), glm::vec3({ 1.0f, 1.0f, 0.8f }));
	mgr->AddObject(merry_go_round_horse, glm::vec3({ 50.0, -40.0, -30.0 }), glm::vec3({ 0.0, 180.0, 0.0 }),
		glm::vec3({ 1.2, 1.2, 1.2 }), glm::vec3({ 1.0f, 0.713f, 0.756f }));
	mgr->AddObject(merry_go_round_horse, glm::vec3({ 50.0, -40.0, -30.0 }), glm::vec3({ 0.0, 270.0, 0.0 }),
		glm::vec3({ 1.2, 1.2, 1.2 }), glm::vec3({ 0.88f, 0.74f, 0.91f }));
	
	// ����
	mgr->AddObject(roller_coaster_rail, glm::vec3({ 0.0, -40.0, -120.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 0.0005, 0.0005, 0.0005 }), glm::vec3({ 1.0f, 1.0f, 1.0f }));
	mgr->AddObject(roller_coaster_head, glm::vec3({ -5.0, -36.0, -115.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 0.0005,  0.0005, 0.0005 }), glm::vec3({ 0.678f, 0.902f, 1.0f }));
	mgr->AddObject(roller_coaster_body, glm::vec3({ 0.0, -36.0, -115.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 0.0005,  0.0005, 0.0005 }), glm::vec3({ 1.0f, 0.95f, 0.8f }));
	mgr->AddObject(roller_coaster_body, glm::vec3({ 5.0, -36.0, -115.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 0.0005,  0.0005, 0.0005 }), glm::vec3({ 1.0f, 0.713f, 0.756f }));
	mgr->AddObject(roller_coaster_body, glm::vec3({ 10.0, -36.0, -115.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 0.0005,  0.0005, 0.0005 }), glm::vec3({ 0.88f, 0.74f, 0.91f }));

	ship_pos = glm::vec3({ -50.0, -10.0, -10.0 }); // ����ŷ ȸ����

	//Object* newObj = new Object(type, loc, rot, _size, _color, ObjectNum, m_importer);
	mgr->AddObject(tree, glm::vec3({30.0,-40.0,-65.0}), glm::vec3({0.0,0.0,0.0}),
		glm::vec3({0.8,0.8,0.8}), glm::vec3({ 0.265f, 0.55f, 0.265f }));



	glutDisplayFunc(drawScene);		// ��� �ݹ� �Լ�
	glutMouseFunc(Mouse);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKeyboard);
	glutReshapeFunc(Reshape);
	glutTimerFunc(render_freq, TimerFunction, 1);

	glutMainLoop();
}

//--- ��� �ݹ��Լ�

GLvoid drawScene()
{
	if (DayMode) glClearColor(0.678f, 0.847f, 0.902f, 1.0f);
	else if (NightMode) glClearColor(0.0, 0.0, 0.0, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);  // ���� �׽�Ʈ Ȱ��ȭ
	glClear(GL_DEPTH_BUFFER_BIT);  // ���� ���� �ʱ�ȭ
	render->SceneRender();

	glutSwapBuffers();	// ȭ�鿡 ����ϱ�
}

//--- �ٽ� �׸��� �ݹ��Լ�
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
		// �㳷 �ٲٱ�
	case 'M':
	case 'm':
	{
		DayMode = true;
		NightMode = false;
	}
	break;
	case 'N':
	case 'n':
	{
		NightMode = true;
		DayMode = false;
	}
	break;
	// ���� ��ȯ
	case '1':
	{
		OneMode = true;
		ThreeMode = false;
	}
	break;
	case '3':
	{
		ThreeMode = true;
		OneMode = false;
	}
	break;
	// �����¿� �̵�
	case 'w':
	case 'W':
	{
		camera->ChangeLocation({0.0, 0.0, -1.0});
		camera->ChangeLookLocation({0.0, 0.0, -1.0});
	}
	break;
	case 's':
	case 'S':
	{
		camera->ChangeLocation({ 0.0, 0.0,1.0 });
		camera->ChangeLookLocation({ 0.0, 0.0,1.0 });
	}
	break;
	case 'a':
	case 'A':
	{
		camera->ChangeLocation({ -1.0, 0.0, 0.0 });
		camera->ChangeLookLocation({ -1.0, 0.0, 0.0 });
	}
	break;
	case 'd':
	case 'D':
	{
		camera->ChangeLocation({ 1.0, 0.0, 0.0 });
		camera->ChangeLookLocation({ 1.0, 0.0, 0.0 });
	}
	break;
	}
	glutPostRedisplay();
}

void SpecialKeyboard(int key, int x, int y) {

	glutPostRedisplay();
}

void TimerFunction(int value)
{
	// ȸ�� ���� ������Ʈ

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
			// ȸ�� ���� ������Ʈ
			if (increasing) {
				currentAngle += angleStep;
				if (currentAngle >= maxAngle) {
					increasing = false; // �ִ� ������ �����ϸ� ���� ��ȯ
				}
			}
			else {
				currentAngle -= angleStep;
				if (currentAngle <= minAngle) {
					increasing = true;  // �ּ� ������ �����ϸ� ���� ��ȯ
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
	//if (button == 3) camera->BindWithMouseWheel(-1.f);
	//else if (button == 4) camera->BindWithMouseWheel(1.f);

	

	glutPostRedisplay();
}