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
void PassiveMotion(int x, int y);
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

static float currentAngle = -1.0f;    // ���� ����
static bool increasing = false;    // ���� ���� ����
float minAngle = -1.0;             // �ּ� ���� (���� ����)
float maxAngle = 1.0f;             // �ִ� ���� (�� ����)
float angleStep = 0.01f;              // �� �����Ӵ� ���� ��ȭ�� (�ӵ�)

bool LightOn = true;

bool OpenDoor = false;
float doorAngle = 0.0f;

// �㳷 �÷���
bool DayMode = true;
bool NightMode = false;

bool RotateMouse = false;

// ���� �ٲٱ� �÷���
bool OneMode = false;
bool ThreeMode = true;

// ���̱ⱸ Ÿ��
bool MerryGoRound = false;
bool RollerCoaster = false;
bool Viking = false;

static int lastX = 0, lastY = 0;

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
	light = new Light({ 0.0, 100.0 , 20.0 });

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
		glm::vec3({ 4.0, 4.0, 4.0 }), glm::vec3({ 1.0f, 0.76f, 0.76f }));
	mgr->AddObject(viking_ship, glm::vec3({ -50.0, -40.0, -30.0 }), glm::vec3({ 0.0, 90.0, 0.0 }),
		glm::vec3({ 4.0, 4.0, 4.0 }), glm::vec3({ 1.0f, 1.0f, 0.8f }));


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
	mgr->AddObject(roller_coaster_head, glm::vec3({ -5.0, -36.0, -114.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 0.0005,  0.0005, 0.0005 }), glm::vec3({ 0.678f, 0.902f, 1.0f }));
	mgr->AddObject(roller_coaster_body, glm::vec3({ 0.0, -36.0, -114.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 0.0005,  0.0005, 0.0005 }), glm::vec3({ 1.0f, 0.95f, 0.8f }));
	mgr->AddObject(roller_coaster_body, glm::vec3({ 5.0, -36.0, -114.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 0.0005,  0.0005, 0.0005 }), glm::vec3({ 1.0f, 0.713f, 0.756f }));
	mgr->AddObject(roller_coaster_body, glm::vec3({ 10.0, -36.0, -114.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 0.0005,  0.0005, 0.0005 }), glm::vec3({ 0.88f, 0.74f, 0.91f }));

	ship_pos = glm::vec3({ -50.0, -0.0, -10.0 }); // ����ŷ ȸ����

	// ����
	mgr->AddObject(tree_leaf, glm::vec3({ -60.0, -40.0, -90.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 4.0, 4.0, 4.0 }), glm::vec3({ 0.3f, 0.7f, 0.3f }));
	mgr->AddObject(tree_wood, glm::vec3({ -60.0, -40.0, -90.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 4.0, 4.0, 4.0 }), glm::vec3({ 0.65f, 0.4f, 0.2f }));

	mgr->AddObject(tree_leaf, glm::vec3({ -50.0, -40.0, -100.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 3.0, 3.0, 3.0 }), glm::vec3({ 0.3f, 0.7f, 0.3f }));
	mgr->AddObject(tree_wood, glm::vec3({ -50.0, -40.0, -100.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 3.0, 3.0, 3.0 }), glm::vec3({ 0.65f, 0.4f, 0.2f }));

	mgr->AddObject(tree_leaf, glm::vec3({ 60.0, -40.0, -90.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 4.0, 4.0, 4.0 }), glm::vec3({ 0.3f, 0.7f, 0.3f }));
	mgr->AddObject(tree_wood, glm::vec3({ 60.0, -40.0, -90.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 4.0, 4.0, 4.0 }), glm::vec3({ 0.65f, 0.4f, 0.2f }));

	mgr->AddObject(tree_leaf, glm::vec3({ 50.0, -40.0, -100.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 3.0, 3.0, 3.0 }), glm::vec3({ 0.3f, 0.7f, 0.3f }));
	mgr->AddObject(tree_wood, glm::vec3({ 50.0, -40.0, -100.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 3.0, 3.0, 3.0 }), glm::vec3({ 0.65f, 0.4f, 0.2f }));

	// ����
	mgr->AddObject(chair, glm::vec3({ -60.0, -40.0, -65.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 0.1, 0.1, 0.1 }), glm::vec3({ 0.65f, 0.4f, 0.2f }));

	mgr->AddObject(chair, glm::vec3({ 60.0, -40.0, -65.0 }), glm::vec3({ 0.0, 180.0, 0.0 }),
		glm::vec3({ 0.1, 0.1, 0.1 }), glm::vec3({ 0.65f, 0.4f, 0.2f }));


	// ��Ÿ��
	// // ù ��° ��Ÿ���� z ���� -145�� �����ϰ�, �� �� ���� ��Ÿ�� �߰�
	float x_start = -70.0f;
	float x_step = 7.0f; // x �� ����

	// ���� �迭 ����
	glm::vec3 colors[] = {
		glm::vec3(0.87f, 0.83f, 0.95f),
		glm::vec3(0.678f, 0.902f, 1.0f),
		glm::vec3(1.0f, 1.0f, 0.8f),
		glm::vec3(1.0f, 0.713f, 0.756f),
		glm::vec3(0.88f, 0.74f, 0.91f),
		glm::vec3(0.87f, 0.83f, 0.95f),
		glm::vec3(0.678f, 0.902f, 1.0f),
		glm::vec3(1.0f, 1.0f, 0.8f),
		glm::vec3(0.87f, 0.83f, 0.95f),
		glm::vec3(0.678f, 0.902f, 1.0f),
		glm::vec3(1.0f, 1.0f, 0.8f),
	};

	// ���ʺ��� �����ʱ��� ��Ÿ�� �߰�
	for (int i = 0; i < 11; i++) {
		float x = x_start + i * x_step;
		mgr->AddObject(fence, glm::vec3({ x, -40.0f, -145.0f }), glm::vec3({ 0.0f, 270.0f, 0.0f }),
			glm::vec3({ 7.0f, 7.0f, 7.0f }), colors[i]);
	}

	// �����ʺ��� ���ʱ��� ��Ÿ�� �߰� (��Ī�� ���߱� ���� ���� �����ϰ�)
	for (int i = 0; i < 10; i++) {
		float x = -(x_start + i * x_step);
		mgr->AddObject(fence, glm::vec3({ x, -40.0f, -145.0f }), glm::vec3({ 0.0f, 270.0f, 0.0f }),
			glm::vec3({ 7.0f, 7.0f, 7.0f }), colors[i]);
	}

	// ���ʺ��� �����ʱ��� ��Ÿ�� �߰�
	for (int i = 0; i < 8; i++) {
		float x = x_start + i * x_step;
		mgr->AddObject(fence, glm::vec3({ x, -40.0f, -145.0f }), glm::vec3({ 0.0f, 270.0f, 0.0f }),
			glm::vec3({ 7.0f, 7.0f, 7.0f }), colors[i]);
		mgr->AddObject(fence, glm::vec3({ x, -40.0f, 5.0f }), glm::vec3({ 0.0f, 270.0f, 0.0f }),
			glm::vec3({ 7.0f, 7.0f, 7.0f }), colors[i]);
	}

	// �����ʺ��� ���ʱ��� ��Ÿ�� �߰� (��Ī�� ���߱� ���� ���� �����ϰ�)
	for (int i = 0; i < 8; i++) {
		float x = -(x_start + i * x_step);
		mgr->AddObject(fence, glm::vec3({ x, -40.0f, -145.0f }), glm::vec3({ 0.0f, 270.0f, 0.0f }),
			glm::vec3({ 7.0f, 7.0f, 7.0f }), colors[i]);
		mgr->AddObject(fence, glm::vec3({ x, -40.0f, 5.0f }), glm::vec3({ 0.0f, 270.0f, 0.0f }),
			glm::vec3({ 7.0f, 7.0f, 7.0f }), colors[i]);
	}





	// ����
// ���� ���� ����
	mgr->AddObject(cloud, glm::vec3({ -40.0, 50.0, -130.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 2.0, 2.0, 2.0 }), glm::vec3({ 1.0f, 1.0, 1.0 }));
	mgr->AddObject(cloud, glm::vec3({ -35.0, 55.0, -110.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 1.9, 1.9, 1.9 }), glm::vec3({ 1.0f, 1.0, 1.0 }));
	mgr->AddObject(cloud, glm::vec3({ -25.0, 60.0, -100.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 2.1, 2.1, 2.1 }), glm::vec3({ 1.0f, 1.0, 1.0 }));
	mgr->AddObject(cloud, glm::vec3({ -15.0, 55.0, -90.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 1.7, 1.7, 1.7 }), glm::vec3({ 1.0f, 1.0, 1.0 }));
	mgr->AddObject(cloud, glm::vec3({ -5.0, 60.0, -80.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 2.0, 2.0, 2.0 }), glm::vec3({ 1.0f, 1.0, 1.0 }));
	mgr->AddObject(cloud, glm::vec3({ 5.0, 55.0, -70.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 1.9, 1.9, 1.9 }), glm::vec3({ 1.0f, 1.0, 1.0 }));
	mgr->AddObject(cloud, glm::vec3({ 15.0, 50.0, -60.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 1.8, 1.8, 1.8 }), glm::vec3({ 1.0f, 1.0, 1.0 }));
	mgr->AddObject(cloud, glm::vec3({ 25.0, 55.0, -50.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 2.0, 2.0, 2.0 }), glm::vec3({ 1.0f, 1.0, 1.0 }));
	mgr->AddObject(cloud, glm::vec3({ 35.0, 50.0, -40.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 1.7, 1.7, 1.7 }), glm::vec3({ 1.0f, 1.0, 1.0 }));
	mgr->AddObject(cloud, glm::vec3({ 45.0, 55.0, -30.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 1.9, 1.9, 1.9 }), glm::vec3({ 1.0f, 1.0, 1.0 }));
	mgr->AddObject(cloud, glm::vec3({ -40.0, 60.0, -20.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 2.1, 2.1, 2.1 }), glm::vec3({ 1.0f, 1.0, 1.0 }));
	mgr->AddObject(cloud, glm::vec3({ -30.0, 50.0, -10.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 1.8, 1.8, 1.8 }), glm::vec3({ 1.0f, 1.0, 1.0 }));
	mgr->AddObject(cloud, glm::vec3({ -20.0, 60.0, 0.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 2.0, 2.0, 2.0 }), glm::vec3({ 1.0f, 1.0, 1.0 }));
	mgr->AddObject(cloud, glm::vec3({ -10.0, 55.0, 10.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 1.9, 1.9, 1.9 }), glm::vec3({ 1.0f, 1.0, 1.0 }));
	mgr->AddObject(cloud, glm::vec3({ 0.0, 50.0, 20.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 2.0, 2.0, 2.0 }), glm::vec3({ 1.0f, 1.0, 1.0 }));
	mgr->AddObject(cloud, glm::vec3({ 10.0, 55.0, 30.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 1.8, 1.8, 1.8 }), glm::vec3({ 1.0f, 1.0, 1.0 }));
	mgr->AddObject(cloud, glm::vec3({ 20.0, 50.0, 40.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 2.1, 2.1, 2.1 }), glm::vec3({ 1.0f, 1.0, 1.0 }));
	mgr->AddObject(cloud, glm::vec3({ 30.0, 55.0, 50.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 1.9, 1.9, 1.9 }), glm::vec3({ 1.0f, 1.0, 1.0 }));
	mgr->AddObject(cloud, glm::vec3({ 40.0, 50.0, 60.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 2.0, 2.0, 2.0 }), glm::vec3({ 1.0f, 1.0, 1.0 }));
	mgr->AddObject(cloud, glm::vec3({ -45.0, 55.0, 70.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 1.8, 1.8, 1.8 }), glm::vec3({ 1.0f, 1.0, 1.0 }));
	mgr->AddObject(cloud, glm::vec3({ -35.0, 50.0, 80.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 1.9, 1.9, 1.9 }), glm::vec3({ 1.0f, 1.0, 1.0 }));


	glutDisplayFunc(drawScene);		// ��� �ݹ� �Լ�
	glutMouseFunc(Mouse);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKeyboard);
	glutReshapeFunc(Reshape);
	glutPassiveMotionFunc(PassiveMotion);
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
		camera->SetLocation({ 0.0, -30.0, 120.0 });
		camera->SetLookLocation({ 0.0, 0.0, 0.0 });
	}
	break;
	case '3':
	{
		ThreeMode = true;
		OneMode = false;
		camera->SetLocation({ 0.0, 150.0, 150.0 });
		camera->SetLookLocation({ 0.0, 0.0, 0.0 });
	}
	break;
	// ���̱ⱸ
	case '7': {
		MerryGoRound = !MerryGoRound;
		if (MerryGoRound) {
			camera->SetLocation({ 50.0, -25.0, -42.0 });
			camera->SetLookLocation({ 0.0, 0.0, 0.0 });
			camera->angle = 100.0f;
		}
		else {
			camera->SetLocation({ 50.0, -30.0, 20.0 });
			camera->SetLookLocation({ 50.0, 0.0, -100.0 });
			camera->angle = 45.0f;
		}
	}
			break;
	case '8': {

	}
			break;
	case '9': {


	}
			break;
	// �����¿� �̵�
	if (OneMode) {
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
	case 'o':
	case 'O': {
		OpenDoor = !OpenDoor;
	}
	}
	}
	glutPostRedisplay();
}

void SpecialKeyboard(int key, int x, int y) {

	glutPostRedisplay();
}

void TimerFunction(int value)
{
	if (MerryGoRound) {
		camera->RotateCam(1.0, glm::vec3{ 50.0, -40.0, -30.0 });
	}

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

			// �ʱ� ���¿��� ���
			glm::mat4 orbit = glm::mat4(1.0f);
			orbit = glm::translate(orbit, glm::vec3(ship_pos));  // ����ŷ�� ���� ��ġ�� �̵�
			orbit = glm::rotate(orbit, glm::radians(currentAngle), glm::vec3(0.0, 0.0, 1.0)); // ȸ��
			orbit = glm::translate(orbit, glm::vec3(-ship_pos)); // ���� ��ġ�� �������� ����

			v->modelMatrix = v->modelMatrix;  // ���ο� ��ȯ ��� ����
		}

		if (v->GetType() == cloud) {
			glm::mat4 orbit = glm::mat4(1.0f);
			//orbit = glm::translate(orbit, glm::vec3(0.0, 0.0, 60.0));
			orbit = glm::rotate(orbit, glm::radians(v->speed), glm::vec3(0.0, 1.0, 0.0));
			//orbit = glm::translate(orbit, -glm::vec3(0.0, 0.0, 60.0));
			v->modelMatrix = orbit * v->modelMatrix;
		}

		if (OpenDoor) {
			doorAngle += 0.1;
			if (doorAngle < 300) {
				if (v->GetType() == door1) {
					glm::mat4 orbit = glm::mat4(1.0f);
					orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
					orbit = glm::rotate(orbit, glm::radians(1.0f), glm::vec3(0.0, 1.0, 0.0));
					orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
					v->modelMatrix = orbit * v->modelMatrix;
				}
				else if (v->GetType() == door2) {
					glm::mat4 orbit = glm::mat4(1.0f);
					orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
					orbit = glm::rotate(orbit, glm::radians(-1.0f), glm::vec3(0.0, 1.0, 0.0));
					orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
					v->modelMatrix = orbit * v->modelMatrix;
				}
			}
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
			RotateMouse = true;
		}

		else if (state == GLUT_UP) {
			RotateMouse = false;
		}
	}
	else if (button == GLUT_RIGHT_BUTTON)
	{
		if (state == GLUT_DOWN) {
		}
	}
	glutPostRedisplay();
}

void PassiveMotion(int x, int y) {

}