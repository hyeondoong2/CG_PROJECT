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
GLvoid SpecialKeyboard(int key, int x, int y);

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

bool roller_crush_check(glm::mat4 prev, glm::mat4 curr);

void printMatrix(const glm::mat4& matrix, const std::string& name) {
	std::cout << "Matrix: " << name << std::endl;
	for (int i = 0; i < 4; ++i) {
		std::cout << "| ";
		for (int j = 0; j < 4; ++j) {
			std::cout << matrix[i][j] << " ";
		}
		std::cout << "|" << std::endl;
	}
	std::cout << std::endl;
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

	//// ������
	//mgr->AddObject(wheel_body, glm::vec3({ 0.0, -40.0, -70.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
	//	glm::vec3({ 1.0, 1.0, 1.0 }), glm::vec3({ 1.0f, 0.713f, 0.756f }));
	//mgr->AddObject(wheel_car, glm::vec3({ 0.0, -8.0, -70.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
	//	glm::vec3({ 0.8, 0.8, 0.8 }), glm::vec3({ 0.678f, 0.902f, 1.0f }));

	//// ����ŷ
	//mgr->AddObject(viking_body, glm::vec3({ -50.0, -40.0, -30.0 }), glm::vec3({ 0.0, 90.0, 0.0 }),
	//	glm::vec3({ 3.0, 3.0, 3.0 }), glm::vec3({ 1.0f, 0.76f, 0.76f }));
	//mgr->AddObject(viking_ship, glm::vec3({ -50.0, -40.0, -30.0 }), glm::vec3({ 0.0, 90.0, 0.0 }),
	//	glm::vec3({ 3.0, 3.0, 3.0 }), glm::vec3({ 1.0f, 0.95f, 0.8f }));


	//// ȸ����
	//mgr->AddObject(merry_go_round_body, glm::vec3({ 50.0, -40.0, -30.0 }), glm::vec3({ 0.0, 90.0, 0.0 }),
	//	glm::vec3({ 1.0, 1.0, 1.0 }), glm::vec3({ 0.87f, 0.83f, 0.95f }));
	//mgr->AddObject(merry_go_round_horse, glm::vec3({ 50.0, -40.0, -30.0 }), glm::vec3({ 0.0, 90.0, 0.0 }),
	//	glm::vec3({ 1.2, 1.2, 1.2 }), glm::vec3({ 0.678f, 0.902f, 1.0f }));
	//mgr->AddObject(merry_go_round_horse, glm::vec3({ 50.0, -40.0, -30.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
	//	glm::vec3({ 1.2, 1.2, 1.2 }), glm::vec3({ 1.0f, 1.0f, 0.8f }));
	//mgr->AddObject(merry_go_round_horse, glm::vec3({ 50.0, -40.0, -30.0 }), glm::vec3({ 0.0, 180.0, 0.0 }),
	//	glm::vec3({ 1.2, 1.2, 1.2 }), glm::vec3({ 1.0f, 0.713f, 0.756f }));
	//mgr->AddObject(merry_go_round_horse, glm::vec3({ 50.0, -40.0, -30.0 }), glm::vec3({ 0.0, 270.0, 0.0 }),
	//	glm::vec3({ 1.2, 1.2, 1.2 }), glm::vec3({ 0.88f, 0.74f, 0.91f }));
	//
	// ����
	mgr->AddObject(roller_coaster_rail, glm::vec3({ 0.0, -40.0, -120.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 0.0005, 0.0005, 0.0005 }), glm::vec3({ 1.0f, 1.0f, 1.0f }));
	mgr->AddObject(roller_coaster_head, glm::vec3({ -5.0, -36.0, -114.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 0.0005,  0.0005, 0.0005 }), glm::vec3({ 0.678f, 0.902f, 1.0f }));
	mgr->AddObject(roller_coaster_body1, glm::vec3({ 0.5, -36.0, -114.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 0.0005,  0.0005, 0.0005 }), glm::vec3({ 1.0f, 0.95f, 0.8f }));
	mgr->AddObject(roller_coaster_body2, glm::vec3({ 6.0, -36.0, -114.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 0.0005,  0.0005, 0.0005 }), glm::vec3({ 1.0f, 0.713f, 0.756f }));
	mgr->AddObject(roller_coaster_body3, glm::vec3({ 11.5, -36.0, -114.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 0.0005,  0.0005, 0.0005 }), glm::vec3({ 0.88f, 0.74f, 0.91f }));

	//ship_pos = glm::vec3({ -50.0, -10.0, -10.0 }); // ����ŷ ȸ����

	//Object* newObj = new Object(type, loc, rot, _size, _color, ObjectNum, m_importer);
	//mgr->AddObject(tree, glm::vec3({30.0,-40.0,-65.0}), glm::vec3({0.0,0.0,0.0}),
	//	glm::vec3({0.8,0.8,0.8}), glm::vec3({ 0.265f, 0.55f, 0.265f }));

	////��
	//mgr->AddObject(door, glm::vec3({ 33.0,-23.0,0.0 }), glm::vec3({ 0.0,70.0,0.0 }),
	//	glm::vec3({ 15.0,15.0,15.0 }), glm::vec3({1.0f,1.0f,1.0f }));
	//mgr->AddObject(door, glm::vec3({ -33.0,-23.0,0.0 }), glm::vec3({ 0.0,110.0,0.0 }),
	//	glm::vec3({ 15.0,15.0,15.0 }), glm::vec3({ 1.0f,1.0f,1.0f }));


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
		camera->ChangeLocation({ 0.0, 0.0, -1.0 });
		camera->ChangeLookLocation({ 0.0, 0.0, -1.0 });

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

GLvoid SpecialKeyboard(int key, int x, int y)
{
	char axis{};
	switch (key) {
	case GLUT_KEY_LEFT: // ���� ȭ��ǥ Ű
		axis = 'y';
		camera->RotateCam(axis, 1.0f);

		break;
	case GLUT_KEY_RIGHT: // ������ ȭ��ǥ Ű
		axis = 'y';
		camera->RotateCam(axis, -1.0f);
		break;
	case GLUT_KEY_DOWN:
		axis = 'x';
		camera->RotateCam(axis, -1.0f);
		break;
	case GLUT_KEY_UP:
		axis = 'x';
		camera->RotateCam(axis, 1.0f);
		break;
	}

	glutPostRedisplay();
}

glm::mat4  prevhead = glm::mat4(1.0f); // ���� ���� �����


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

		else if (v->GetType() == roller_coaster_head) {
			glm::mat4 orbit = glm::mat4(1.0f);
			if (v->GetMatrix()[3].x <= -37.0 && v->GetMatrix()[3].x > -40.0 && v->GetMatrix()[3].z >= -122) {
				orbit = glm::translate(orbit, glm::vec3(-0.05, 0.0, 0.0));
			}
			else if (v->GetMatrix()[3].x > -52.0 && v->GetMatrix()[3].x <= -40.0 && v->GetMatrix()[3].y <= 22.0 && v->GetMatrix()[3].z >= -122) {
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.15f), glm::vec3(0.0, 0.0, -1.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(-0.05, 0.02, 0.0));
			}
			else if (v->GetMatrix()[3].x > -61.0 && v->GetMatrix()[3].x <= -52.0 && v->GetMatrix()[3].z >= -122) {
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.1f), glm::vec3(0.0, 0.0, -1.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(-0.05, 0.05, 0.0));
			}
			else if (v->GetMatrix()[3].x > -64.0 && v->GetMatrix()[3].x <= -61.0 && v->GetMatrix()[3].z >= -122) {
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.15f), glm::vec3(0.0, 0.0, -1.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(-0.02, 0.05, 0.0));
			}
			else if (v->GetMatrix()[3].x > -65.4 && v->GetMatrix()[3].x <= -64.0 && v->GetMatrix()[3].z >= -122) {
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.14f), glm::vec3(0.0, 0.0, -1.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(-0.018, 0.07, 0.0));
			}
			else if (v->GetMatrix()[3].x <= -65.4 && v->GetMatrix()[3].y <= 18.0 && v->GetMatrix()[3].z >= -122) {   //���� ����
				orbit = glm::translate(orbit, glm::vec3(0.0, 0.1, 0.0));
			}
			else if (v->GetMatrix()[3].x <= 60.0 && v->GetMatrix()[3].x > -37.0 && v->GetMatrix()[3].z >= -122) {
				orbit = glm::translate(orbit, glm::vec3(-0.1, 0.0, 0.0));
			}
			else if (v->GetMatrix()[3].x <= 60.0 && v->GetMatrix()[3].y <= 23.0 && v->GetMatrix()[3].z >= -122) {
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.5f), glm::vec3(-1.0, 0.0, 0.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(0.0, 0.07, -0.03));
			}
			else if (v->GetMatrix()[3].x <= 60.0 && v->GetMatrix()[3].y <= 25.0 && v->GetMatrix()[3].z >= -123.0) {
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.8f), glm::vec3(-1.0, 0.0, 0.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(0.0, 0.01, -0.05));
			}
			else if (v->GetMatrix()[3].x <= 60.0 && v->GetMatrix()[3].y > 21.0 && v->GetMatrix()[3].z <= -123.0) {
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.4f), glm::vec3(-1.0, 0.0, 0.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(0.0, -0.04, -0.025));
			}
			//�ѷ��ڽ��� ���� 
			else if (v->GetMatrix()[3].x <= 60.0 && v->GetMatrix()[3].y >= 12.0) {
				orbit = glm::translate(orbit, glm::vec3(0.0, -0.06, 0.0));
			}
			else if (v->GetMatrix()[3].x <= 60.0 && v->GetMatrix()[3].y >= 8.0) {
				orbit = glm::translate(orbit, glm::vec3(0.0, -0.3, 0.0));
			}
			else if (v->GetMatrix()[3].x <= 60.0 && v->GetMatrix()[3].y >= -8.0) {
				orbit = glm::translate(orbit, glm::vec3(0.0, -0.7, 0.0));
			}
			else if (v->GetMatrix()[3].x >= -66.0 && v->GetMatrix()[3].x <= -64.8 && v->GetMatrix()[3].z <= -120.0) {	//
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.33f), glm::vec3(0.0, 0.0, 1.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(0.023, -0.23, 0.0));
			}
			else if (v->GetMatrix()[3].x >= -65.0 && v->GetMatrix()[3].x <= -63.0 && v->GetMatrix()[3].z <= -120.0) {	//
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.833f), glm::vec3(0.0, 0.0, 1.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(0.08, -0.25, 0.0));
			}
			else if (v->GetMatrix()[3].x >= -63.0 && v->GetMatrix()[3].x <= -57.0 && v->GetMatrix()[3].z <= -120.0) {	//
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.83f), glm::vec3(0.0, 0.0, 1.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(0.2, -0.28, 0.0));
			}
			else if (v->GetMatrix()[3].x >= -57.0 && v->GetMatrix()[3].x <= -50.0 && v->GetMatrix()[3].z <= -120.0) {	//
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.9f), glm::vec3(0.0, 0.0, 1.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(0.3, -0.22, 0.0));
			}
			else if (v->GetMatrix()[3].x >= -50.0 && v->GetMatrix()[3].x <= -44.0 && v->GetMatrix()[3].z <= -120.0) {	//
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.2f), glm::vec3(0.0, 0.0, 1.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(0.4, -0.15, 0.0));
			}
			else if (v->GetMatrix()[3].x >= -44.0 && v->GetMatrix()[3].x <= -36.0 && v->GetMatrix()[3].z <= -120.0) {	//
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.8f), glm::vec3(0.0, 0.0, 1.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(0.4, -0.05, 0.0));
			}
			//���� �̵� �ӵ�
			else if (v->GetMatrix()[3].x >= -38.0 && v->GetMatrix()[3].x <= 10.0 && v->GetMatrix()[3].z <= -120.0) {	//
				orbit = glm::translate(orbit, glm::vec3(0.6, 0.0, 0.0));
			}
			else if (v->GetMatrix()[3].x >= 10.0 && v->GetMatrix()[3].x <= 35.0 && v->GetMatrix()[3].z <= -120.0) {	//
				orbit = glm::translate(orbit, glm::vec3(0.4, 0.0, 0.0));
			}
			else if (v->GetMatrix()[3].x >= 35.0 && v->GetMatrix()[3].x <= 68.0 && v->GetMatrix()[3].z <= -120.0) {	//
				orbit = glm::translate(orbit, glm::vec3(0.2, 0.0, 0.0));
			}
			else if (v->GetMatrix()[3].x <= 73.0 && v->GetMatrix()[3].z <= -120.0) {
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(-0.5f), glm::vec3(0.0, 1.0, 0.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(0.04, 0.0, 0.03));
			}
			else if (v->GetMatrix()[3].z <= -114.0 && v->GetMatrix()[3].z >= -123.0) {
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(-0.45f), glm::vec3(0.0, 1.0, 0.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(-0.012, 0.0, 0.03));
			}
			else if (v->GetMatrix()[3].x >= 70.5 && v->GetMatrix()[3].z >= -114.5) {
				glm::vec3 loc = glm::vec3(70.0, -36.0, -114.0);  // ��ġ
				glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0);        // ȸ�� (����)
				glm::vec3 _size = glm::vec3(0.0005, 0.0005, 0.0005); // ũ��

				// �ʱ�ȭ
				v->modelMatrix = glm::mat4(1.0f);

				// ũ�� ����
				glm::mat4 scale = glm::scale(glm::mat4(1.0f), _size);

				// ȸ�� ���� (XYZ ������ ȸ��)
				glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), rot.x, glm::vec3(1.0, 0.0, 0.0)); // X�� ȸ��
				rotation = glm::rotate(rotation, rot.y, glm::vec3(0.0, 1.0, 0.0));                 // Y�� ȸ��
				rotation = glm::rotate(rotation, rot.z, glm::vec3(0.0, 0.0, 1.0));                 // Z�� ȸ��

				// ��ġ �̵�
				glm::mat4 translation = glm::translate(glm::mat4(1.0f), loc);

				// ��ȯ ����: Translation * Rotation * Scale
				v->modelMatrix = translation * rotation * scale;

				// ����� ���
				//printMatrix(v->modelMatrix, "v->modelMatrix");

			}
			else if (v->GetMatrix()[3].x >= 65.0) {
				orbit = glm::translate(orbit, glm::vec3(-0.09, 0.0, 0.0));
			}
			else if (v->GetMatrix()[3].x >= 20.0) {
				orbit = glm::translate(orbit, glm::vec3(-0.3, 0.0, 0.0));
			}
			v->modelMatrix = orbit * v->modelMatrix;

		}
		else if (v->GetType() == roller_coaster_body1) {
			glm::mat4 orbit = glm::mat4(1.0f);
			if (v->GetMatrix()[3].x <= -30.0 && v->GetMatrix()[3].x > -39.0 && v->GetMatrix()[3].z >= -122) {
				orbit = glm::translate(orbit, glm::vec3(-0.05, 0.0, 0.0));
			}
			else if (v->GetMatrix()[3].x > -43.0 && v->GetMatrix()[3].x <= -39.0 && v->GetMatrix()[3].y <= 22.0 && v->GetMatrix()[3].z >= -122) {
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.33f), glm::vec3(0.0, 0.0, -1.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(-0.083, 0.025, 0.0));

			}
			else if (v->GetMatrix()[3].x > -48.0 && v->GetMatrix()[3].x <= -39.0 && v->GetMatrix()[3].y <= 22.0 && v->GetMatrix()[3].z >= -122) {
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.13f), glm::vec3(0.0, 0.0, -1.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(-0.05, 0.013, 0.0));

			}
			else if (v->GetMatrix()[3].x > -55.0 && v->GetMatrix()[3].x <= -48.0 && v->GetMatrix()[3].z >= -122) {
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.15f), glm::vec3(0.0, 0.0, -1.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(-0.06, 0.04, 0.0));
			}
			else if (v->GetMatrix()[3].x > -61.0 && v->GetMatrix()[3].x <= -55.0 && v->GetMatrix()[3].z >= -122) {
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.09f), glm::vec3(0.0, 0.0, -1.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(-0.043, 0.043, 0.0));
			}
			else if (v->GetMatrix()[3].x > -63.5 && v->GetMatrix()[3].x <= -61.0 && v->GetMatrix()[3].z >= -122) {
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.14f), glm::vec3(0.0, 0.0, -1.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(-0.024, 0.052, 0.0));
			}
			else if (v->GetMatrix()[3].x > -65.4 && v->GetMatrix()[3].x <= -63.5 && v->GetMatrix()[3].z >= -122) {
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.185f), glm::vec3(0.0, 0.0, -1.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(-0.025, 0.08, 0.0));
			}
			else if (v->GetMatrix()[3].x <= -65.4 && v->GetMatrix()[3].y <= 14.0 && v->GetMatrix()[3].z >= -122) {   //���� ����
				orbit = glm::translate(orbit, glm::vec3(0.0, 0.1, 0.0));
			}
			else if (v->GetMatrix()[3].x <= -65.4 && v->GetMatrix()[3].y <= 18.0 && v->GetMatrix()[3].z >= -122) {   //���� ����
				orbit = glm::translate(orbit, glm::vec3(0.0, 0.05, 0.0));
			}
			else if (v->GetMatrix()[3].x <= 60.0 && v->GetMatrix()[3].x > -30.0 && v->GetMatrix()[3].z >= -122) {	//�������� �̵�
				orbit = glm::translate(orbit, glm::vec3(-0.1, 0.0, 0.0));
			}
			else if (v->GetMatrix()[3].x <= 60.0 && v->GetMatrix()[3].y <= 23.0 && v->GetMatrix()[3].z >= -122) {
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.5f), glm::vec3(-1.0, 0.0, 0.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(0.0, 0.07, -0.03));
			}
			else if (v->GetMatrix()[3].x <= 60.0 && v->GetMatrix()[3].y <= 25.0 && v->GetMatrix()[3].z >= -123.0) {
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.8f), glm::vec3(-1.0, 0.0, 0.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(0.0, 0.01, -0.05));
			}
			else if (v->GetMatrix()[3].x <= 60.0 && v->GetMatrix()[3].y > 21.0 && v->GetMatrix()[3].z <= -123.0) {
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.4f), glm::vec3(-1.0, 0.0, 0.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(0.0, -0.04, -0.025));
				//std::cout << "x      " << v->GetMatrix()[3].x << std::endl;
				//std::cout << "y      " << v->GetMatrix()[3].y << std::endl;
				//std::cout << "z      " << v->GetMatrix()[3].z << std::endl;
			}
			////�ѷ��ڽ��� ���� a
			else if (v->GetMatrix()[3].x <= 60.0 && v->GetMatrix()[3].y >= 17.6) {
				orbit = glm::translate(orbit, glm::vec3(0.0, -0.08, 0.0));
				std::cout << "x      " << v->GetMatrix()[3].x << std::endl;
				std::cout << "y      " << v->GetMatrix()[3].y << std::endl;
				std::cout << "z      " << v->GetMatrix()[3].z << std::endl;
			}
			else if (v->GetMatrix()[3].x <= 60.0 && v->GetMatrix()[3].y >= 13.6) {
				orbit = glm::translate(orbit, glm::vec3(0.0, -0.2, 0.0));
			}
			else if (v->GetMatrix()[3].x <= 60.0 && v->GetMatrix()[3].y >= -3.0) {
				orbit = glm::translate(orbit, glm::vec3(0.0, -0.7, 0.0));
			}
			else if (v->GetMatrix()[3].x <= 60.0 && v->GetMatrix()[3].y >= -8.5) {
				orbit = glm::translate(orbit, glm::vec3(0.0, -0.4, 0.0));
			}
			else if (v->GetMatrix()[3].x >= -66.0 && v->GetMatrix()[3].x <= -64.8 && v->GetMatrix()[3].z <= -120.0) {	//
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.33f), glm::vec3(0.0, 0.0, 1.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(0.023, -0.23, 0.0));
			}
			else if (v->GetMatrix()[3].x >= -65.0 && v->GetMatrix()[3].x <= -63.0 && v->GetMatrix()[3].z <= -120.0) {	//
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.833f), glm::vec3(0.0, 0.0, 1.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(0.08, -0.25, 0.0));
			}
			else if (v->GetMatrix()[3].x >= -63.0 && v->GetMatrix()[3].x <= -57.0 && v->GetMatrix()[3].z <= -120.0) {	//
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.83f), glm::vec3(0.0, 0.0, 1.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(0.2, -0.28, 0.0));
			}
			else if (v->GetMatrix()[3].x >= -57.0 && v->GetMatrix()[3].x <= -50.0 && v->GetMatrix()[3].z <= -120.0) {	//
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.9f), glm::vec3(0.0, 0.0, 1.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(0.3, -0.22, 0.0));
			}
			else if (v->GetMatrix()[3].x >= -50.0 && v->GetMatrix()[3].x <= -44.0 && v->GetMatrix()[3].z <= -120.0) {	//
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.2f), glm::vec3(0.0, 0.0, 1.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(0.4, -0.15, 0.0));
			}
			else if (v->GetMatrix()[3].x >= -44.0 && v->GetMatrix()[3].x <= -36.0 && v->GetMatrix()[3].z <= -120.0) {	//
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.8f), glm::vec3(0.0, 0.0, 1.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(0.4, -0.05, 0.0));
			}
			//���� �̵� �ӵ�
			else if (v->GetMatrix()[3].x >= -38.0 && v->GetMatrix()[3].x <= 10.0 && v->GetMatrix()[3].z <= -120.0) {	//
				orbit = glm::translate(orbit, glm::vec3(0.6, 0.0, 0.0));
			}
			else if (v->GetMatrix()[3].x >= 10.0 && v->GetMatrix()[3].x <= 30.0 && v->GetMatrix()[3].z <= -120.0) {	//
				orbit = glm::translate(orbit, glm::vec3(0.4, 0.0, 0.0));
			}
			else if (v->GetMatrix()[3].x >= 30.0 && v->GetMatrix()[3].x <= 63.0 && v->GetMatrix()[3].z <= -120.0) {	//
				orbit = glm::translate(orbit, glm::vec3(0.2, 0.0, 0.0));
			}
			else if (v->GetMatrix()[3].x >= 63.0 && v->GetMatrix()[3].x <= 68.0 && v->GetMatrix()[3].z <= -120.0) {	//
				orbit = glm::translate(orbit, glm::vec3(0.05, 0.0, 0.0));
			}
			else if (v->GetMatrix()[3].x <= 73.0 && v->GetMatrix()[3].z <= -120.0) {
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(-0.7f), glm::vec3(0.0, 1.0, 0.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(0.045, 0.0, 0.02));
			}
			else if (v->GetMatrix()[3].z < -114.5 && v->GetMatrix()[3].z >= -123.0) {
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(-0.38f), glm::vec3(0.0, 1.0, 0.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(-0.014, 0.0, 0.038));
				printMatrix(v->modelMatrix, "v->modelMatrix");
			}
			else if (v->GetMatrix()[3].x >= 70.5 && v->GetMatrix()[3].z >= -114.5) {
				glm::vec3 loc = glm::vec3(70.0, -36.0, -114.0);  // ��ġ
				glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0);        // ȸ�� (����)
				glm::vec3 _size = glm::vec3(0.0005, 0.0005, 0.0005); // ũ��

				// �ʱ�ȭ
				v->modelMatrix = glm::mat4(1.0f);

				// ũ�� ����
				glm::mat4 scale = glm::scale(glm::mat4(1.0f), _size);

				// ȸ�� ���� (XYZ ������ ȸ��)
				glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), rot.x, glm::vec3(1.0, 0.0, 0.0)); // X�� ȸ��
				rotation = glm::rotate(rotation, rot.y, glm::vec3(0.0, 1.0, 0.0));                 // Y�� ȸ��
				rotation = glm::rotate(rotation, rot.z, glm::vec3(0.0, 0.0, 1.0));                 // Z�� ȸ��
				//glm::mat4 rotation = glm::mat4(1.0f); // ȸ�� ����
				// ��ġ �̵�
				glm::mat4 translation = glm::translate(glm::mat4(1.0f), loc);

				// ��ȯ ����: Translation * Rotation * Scale
				v->modelMatrix = translation * rotation * scale;

				// ����� ���
				printMatrix(v->modelMatrix, "v->modelMatrix");

			}
			else if (v->GetMatrix()[3].x >= 65.0) {
				orbit = glm::translate(orbit, glm::vec3(-0.1, 0.0, 0.0));
				//printMatrix(v->modelMatrix, "v->modelMatrix");
			}
			else if (v->GetMatrix()[3].x >= 20.0) {
				orbit = glm::translate(orbit, glm::vec3(-0.3, 0.0, 0.0));
				//printMatrix(v->modelMatrix, "v->modelMatrix");
			}
			v->modelMatrix = orbit * v->modelMatrix;
			prevhead = v->modelMatrix;

		}
		else if (v->GetType() == roller_coaster_body2) {
			glm::mat4 orbit = glm::mat4(1.0f);
			if (prevhead[3].x > -45.0 && prevhead[3].z >= -114.0) {	// �����̵�
				glm::mat4 orbit = glm::mat4(1.0f);
				glm::vec3 loc = glm::vec3(prevhead[3].x + 5.8, -36.0, -114.0);  // ��ġ
				glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0);        // ȸ�� (����)
				glm::vec3 _size = glm::vec3(0.0005, 0.0005, 0.0005); // ũ��

				// �ʱ�ȭ
				v->modelMatrix = glm::mat4(1.0f);

				// ũ�� ����
				glm::mat4 scale = glm::scale(glm::mat4(1.0f), _size);

				// ȸ�� ���� (XYZ ������ ȸ��)
				glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), rot.x, glm::vec3(1.0, 0.0, 0.0)); // X�� ȸ��
				rotation = glm::rotate(rotation, rot.y, glm::vec3(0.0, 1.0, 0.0));                 // Y�� ȸ��
				rotation = glm::rotate(rotation, rot.z, glm::vec3(0.0, 0.0, 1.0));                 // Z�� ȸ��

				// ��ġ �̵�
				glm::mat4 translation = glm::translate(glm::mat4(1.0f), loc);

				// ��ȯ ����: Translation * Rotation * Scale
				v->modelMatrix = translation * rotation * scale;
			}
			else if (v->GetMatrix()[3].x > -43.0 && v->GetMatrix()[3].x <= -39.0 && v->GetMatrix()[3].y <= 22.0 && v->GetMatrix()[3].z >= -122) {
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.33f), glm::vec3(0.0, 0.0, -1.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(-0.083, 0.025, 0.0));

			}
			else if (v->GetMatrix()[3].x > -48.0 && v->GetMatrix()[3].x <= -39.0 && v->GetMatrix()[3].y <= 22.0 && v->GetMatrix()[3].z >= -122) {
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.13f), glm::vec3(0.0, 0.0, -1.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(-0.05, 0.013, 0.0));

			}
			else if (v->GetMatrix()[3].x > -55.0 && v->GetMatrix()[3].x <= -48.0 && v->GetMatrix()[3].z >= -122) {
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.15f), glm::vec3(0.0, 0.0, -1.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(-0.06, 0.04, 0.0));
			}
			else if (v->GetMatrix()[3].x > -61.0 && v->GetMatrix()[3].x <= -55.0 && v->GetMatrix()[3].z >= -122) {
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.09f), glm::vec3(0.0, 0.0, -1.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(-0.043, 0.043, 0.0));
			}
			else if (v->GetMatrix()[3].x > -63.5 && v->GetMatrix()[3].x <= -61.0 && v->GetMatrix()[3].z >= -122) {
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.15f), glm::vec3(0.0, 0.0, -1.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(-0.0245, 0.06, 0.0));
			}
			else if (v->GetMatrix()[3].x > -65.4 && v->GetMatrix()[3].x <= -63.5 && v->GetMatrix()[3].z >= -122) {
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.185f), glm::vec3(0.0, 0.0, -1.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(-0.0253, 0.1, 0.0));
			}
			else if (prevhead[3].x <= 64.5 && prevhead[3].z == -114.0) {	//�ö󰡴� �κ�
				glm::mat4 orbit = glm::mat4(1.0f);
				glm::vec3 loc = glm::vec3(prevhead[3].x, prevhead[3].y - 5.8, -114.0);  // ��ġ
				glm::vec3 rot = glm::vec3(0.0, 0.0, -1.5708);        // ȸ�� (����)
				glm::vec3 _size = glm::vec3(0.0005, 0.0005, 0.0005); // ũ��

				// �ʱ�ȭ
				v->modelMatrix = glm::mat4(1.0f);

				// ũ�� ����
				glm::mat4 scale = glm::scale(glm::mat4(1.0f), _size);

				// ȸ�� ���� (XYZ ������ ȸ��)
				glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), rot.x, glm::vec3(1.0, 0.0, 0.0)); // X�� ȸ��
				rotation = glm::rotate(rotation, rot.y, glm::vec3(0.0, 1.0, 0.0));                 // Y�� ȸ��
				rotation = glm::rotate(rotation, rot.z, glm::vec3(0.0, 0.0, 1.0));                 // Z�� ȸ��

				// ��ġ �̵�
				glm::mat4 translation = glm::translate(glm::mat4(1.0f), loc);

				// ��ȯ ����: Translation * Rotation * Scale
				v->modelMatrix = translation * rotation * scale;
			}
			else if (v->GetMatrix()[3].x <= -65.4 && v->GetMatrix()[3].y <= 18.0 && v->GetMatrix()[3].z >= -122) {   //���� ����
				orbit = glm::translate(orbit, glm::vec3(0.0, 0.1, 0.0));
			}
			else if (v->GetMatrix()[3].x <= 60.0 && v->GetMatrix()[3].y <= 23.0 && v->GetMatrix()[3].z >= -122) {
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.5f), glm::vec3(-1.0, 0.0, 0.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(0.0, 0.07, -0.03));
			}
			else if (v->GetMatrix()[3].x <= 60.0 && v->GetMatrix()[3].y <= 25.0 && v->GetMatrix()[3].z >= -123.0) {
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.8f), glm::vec3(-1.0, 0.0, 0.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(0.0, 0.01, -0.05));
			}
			else if (v->GetMatrix()[3].x <= 60.0 && v->GetMatrix()[3].y > 21.0 && v->GetMatrix()[3].z <= -123.0) {
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.4f), glm::vec3(-1.0, 0.0, 0.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(0.0, -0.04, -0.025));
			}
			else if (v->GetMatrix()[3].x <= 60.0 && v->GetMatrix()[3].y >= -8.5 && prevhead[3].z <= -124.0) {	//�������� �κ�
				glm::mat4 orbit = glm::mat4(1.0f);
				glm::vec3 loc = glm::vec3(prevhead[3].x, prevhead[3].y + 5.8, prevhead[3].z);  // ��ġ
				glm::vec3 rot = glm::vec3(0.0, 3.1416, 1.5708);        // ȸ�� (����)
				glm::vec3 _size = glm::vec3(0.0005, 0.0005, 0.0005); // ũ��

				// �ʱ�ȭ
				v->modelMatrix = glm::mat4(1.0f);

				// ũ�� ����
				glm::mat4 scale = glm::scale(glm::mat4(1.0f), _size);

				// ȸ�� ���� (XYZ ������ ȸ��)
				glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), rot.x, glm::vec3(1.0, 0.0, 0.0)); // X�� ȸ��
				rotation = glm::rotate(rotation, rot.y, glm::vec3(0.0, 1.0, 0.0));                 // Y�� ȸ��
				rotation = glm::rotate(rotation, rot.z, glm::vec3(0.0, 0.0, 1.0));                 // Z�� ȸ��

				// ��ġ �̵�
				glm::mat4 translation = glm::translate(glm::mat4(1.0f), loc);

				// ��ȯ ����: Translation * Rotation * Scale
				v->modelMatrix = translation * rotation * scale;
			}
			else if (v->GetMatrix()[3].x <= 60.0 && v->GetMatrix()[3].y >= -14.0) {
				orbit = glm::translate(orbit, glm::vec3(0.0, -0.4, 0.0));
			}
			else if (v->GetMatrix()[3].x >= -66.0 && v->GetMatrix()[3].x <= -64.8 && v->GetMatrix()[3].z <= -120.0) {	//
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.33f), glm::vec3(0.0, 0.0, 1.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(0.023, -0.23, 0.0));
			}
			else if (v->GetMatrix()[3].x >= -65.0 && v->GetMatrix()[3].x <= -63.0 && v->GetMatrix()[3].z <= -120.0) {	//
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.833f), glm::vec3(0.0, 0.0, 1.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(0.08, -0.25, 0.0));
			}
			else if (v->GetMatrix()[3].x >= -63.0 && v->GetMatrix()[3].x <= -57.0 && v->GetMatrix()[3].z <= -120.0) {	//
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.83f), glm::vec3(0.0, 0.0, 1.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(0.2, -0.28, 0.0));
			}
			else if (v->GetMatrix()[3].x >= -57.0 && v->GetMatrix()[3].x <= -50.0 && v->GetMatrix()[3].z <= -120.0) {	//
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.9f), glm::vec3(0.0, 0.0, 1.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(0.3, -0.22, 0.0));
			}
			else if (v->GetMatrix()[3].x >= -50.0 && v->GetMatrix()[3].x <= -44.0 && v->GetMatrix()[3].z <= -120.0) {	//
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.2f), glm::vec3(0.0, 0.0, 1.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(0.4, -0.15, 0.0));
			}
			else if (v->GetMatrix()[3].x >= -44.0 && v->GetMatrix()[3].x <= -36.0 && v->GetMatrix()[3].z <= -120.0) {	//
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.8f), glm::vec3(0.0, 0.0, 1.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(0.4, -0.05, 0.0));
			}
			else if (prevhead[3].x<=68.0 && prevhead[3].z <= -124.0) {	//������ �� ����
				glm::mat4 orbit = glm::mat4(1.0f);
				glm::vec3 loc = glm::vec3(prevhead[3].x-5.8, prevhead[3].y , prevhead[3].z);  // ��ġ
				glm::vec3 rot = glm::vec3(0.0, 3.1416, 0.0);        // ȸ�� (����)
				glm::vec3 _size = glm::vec3(0.0005, 0.0005, 0.0005); // ũ��

				// �ʱ�ȭ
				v->modelMatrix = glm::mat4(1.0f);

				// ũ�� ����
				glm::mat4 scale = glm::scale(glm::mat4(1.0f), _size);

				// ȸ�� ���� (XYZ ������ ȸ��)
				glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), rot.x, glm::vec3(1.0, 0.0, 0.0)); // X�� ȸ��
				rotation = glm::rotate(rotation, rot.y, glm::vec3(0.0, 1.0, 0.0));                 // Y�� ȸ��
				rotation = glm::rotate(rotation, rot.z, glm::vec3(0.0, 0.0, 1.0));                 // Z�� ȸ��

				// ��ġ �̵�
				glm::mat4 translation = glm::translate(glm::mat4(1.0f), loc);

				// ��ȯ ����: Translation * Rotation * Scale
				v->modelMatrix = translation * rotation * scale;
				}
			v->modelMatrix = orbit * v->modelMatrix;
			printMatrix(v->modelMatrix, "v->modelMatrix");
		}
	}


	glutPostRedisplay();
	glutTimerFunc(render_freq, TimerFunction, 1);
}

bool roller_crush_check(glm::mat4 prev, glm::mat4 curr) {
	//printMatrix(prev, "prevhead");
	//printMatrix(curr, "v->modelMatrix");
	//if (prev[3].z >= -123 && curr[3].z >= -123) {	//�浹�ϸ� false
	//	if (prev[3].x >= -65 && curr[3].x >= -65 && prev[3].x + 2.5 <= curr[3].x - 2.5) {
	//		return true;
	//		if (prev[3].y - 0.15 >= curr[3].y + 0.2 && prev[3].x + 2.5 >= curr[3].x - 2.5) {
	//			return false;
	//		}
	//	}
	//}
	//else if (prev[3].z <= -120 && curr[3].z <= -120) {
	//	if (prev[3].y + 0.2 >= curr[3].y - 0.15 && prev[3].x - 2.5 <= curr[3].x + 2.5) {
	//		return false;
	//	}
	//}
	return true;
}

void Mouse(int button, int state, int x, int y)
{
	//if (button == 3) camera->BindWithMouseWheel(-1.f);
	//else if (button == 4) camera->BindWithMouseWheel(1.f);



	glutPostRedisplay();
}

