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

static float currentAngle = -1.0f;    // 현재 각도
static bool increasing = false;    // 각도 증가 방향
float minAngle = -1.0;             // 최소 각도 (시작 지점)
float maxAngle = 1.0f;             // 최대 각도 (끝 지점)
float angleStep = 0.01f;              // 한 프레임당 각도 변화량 (속도)

bool LightOn = true;

bool OpenDoor = false;
float doorAngle = 0.0f;

// 밤낮 플래그
bool DayMode = true;
bool NightMode = false;

bool RotateMouse = false;

// 시점 바꾸기 플래그
bool OneMode = false;
bool ThreeMode = true;

// 놀이기구 타기
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
	light = new Light({ 0.0, 100.0 , 20.0 });

	//camera->ortho = true;
	camera->perspect = true;

	// 카메라 위치 설정
	camera->SetLocation({ 0.0, -30.0, 120.0 });
	camera->SetLookLocation({ 0.0, 0.0, 0.0 });

	render->SetCamera(camera);
	render->SetLight(light);

	// 땅
	mgr->AddObject(base, glm::vec3({ 0.0, -40.0, -70.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 40.0, 1.0, 40.0 }), glm::vec3({ 0.6f, 0.933f, 0.565f }));

	// 관람차
	mgr->AddObject(wheel_body, glm::vec3({ 0.0, -40.0, -70.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 1.0, 1.0, 1.0 }), glm::vec3({ 1.0f, 0.713f, 0.756f }));
	mgr->AddObject(wheel_car, glm::vec3({ 0.0, -8.0, -70.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 0.8, 0.8, 0.8 }), glm::vec3({ 0.678f, 0.902f, 1.0f }));

	// 바이킹
	mgr->AddObject(viking_body, glm::vec3({ -50.0, -40.0, -30.0 }), glm::vec3({ 0.0, 90.0, 0.0 }),
		glm::vec3({ 4.0, 4.0, 4.0 }), glm::vec3({ 1.0f, 0.76f, 0.76f }));
	mgr->AddObject(viking_ship, glm::vec3({ -50.0, -40.0, -30.0 }), glm::vec3({ 0.0, 90.0, 0.0 }),
		glm::vec3({ 4.0, 4.0, 4.0 }), glm::vec3({ 1.0f, 1.0f, 0.8f }));


	// 회전목마
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
	// 롤코
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

	ship_pos = glm::vec3({ -50.0, -0.0, -10.0 }); // 바이킹 회전축

	// 나무
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

	// 의자
	mgr->AddObject(chair, glm::vec3({ -60.0, -40.0, -65.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 0.1, 0.1, 0.1 }), glm::vec3({ 0.65f, 0.4f, 0.2f }));

	mgr->AddObject(chair, glm::vec3({ 60.0, -40.0, -65.0 }), glm::vec3({ 0.0, 180.0, 0.0 }),
		glm::vec3({ 0.1, 0.1, 0.1 }), glm::vec3({ 0.65f, 0.4f, 0.2f }));


	// 울타리
	// // 첫 번째 울타리의 z 값을 -145로 수정하고, 빈 곳 없이 울타리 추가
	float x_start = -70.0f;
	float x_step = 7.0f; // x 값 간격

	// 색상 배열 정의
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

	// 왼쪽부터 오른쪽까지 울타리 추가
	for (int i = 0; i < 11; i++) {
		float x = x_start + i * x_step;
		mgr->AddObject(fence, glm::vec3({ x, -40.0f, -145.0f }), glm::vec3({ 0.0f, 270.0f, 0.0f }),
			glm::vec3({ 7.0f, 7.0f, 7.0f }), colors[i]);
	}

	// 오른쪽부터 왼쪽까지 울타리 추가 (대칭을 맞추기 위해 색상도 동일하게)
	for (int i = 0; i < 10; i++) {
		float x = -(x_start + i * x_step);
		mgr->AddObject(fence, glm::vec3({ x, -40.0f, -145.0f }), glm::vec3({ 0.0f, 270.0f, 0.0f }),
			glm::vec3({ 7.0f, 7.0f, 7.0f }), colors[i]);
	}

	// 왼쪽부터 오른쪽까지 울타리 추가
	for (int i = 0; i < 8; i++) {
		float x = x_start + i * x_step;
		mgr->AddObject(fence, glm::vec3({ x, -40.0f, -145.0f }), glm::vec3({ 0.0f, 270.0f, 0.0f }),
			glm::vec3({ 7.0f, 7.0f, 7.0f }), colors[i]);
		mgr->AddObject(fence, glm::vec3({ x, -40.0f, 5.0f }), glm::vec3({ 0.0f, 270.0f, 0.0f }),
			glm::vec3({ 7.0f, 7.0f, 7.0f }), colors[i]);
	}

	// 오른쪽부터 왼쪽까지 울타리 추가 (대칭을 맞추기 위해 색상도 동일하게)
	for (int i = 0; i < 8; i++) {
		float x = -(x_start + i * x_step);
		mgr->AddObject(fence, glm::vec3({ x, -40.0f, -145.0f }), glm::vec3({ 0.0f, 270.0f, 0.0f }),
			glm::vec3({ 7.0f, 7.0f, 7.0f }), colors[i]);
		mgr->AddObject(fence, glm::vec3({ x, -40.0f, 5.0f }), glm::vec3({ 0.0f, 270.0f, 0.0f }),
			glm::vec3({ 7.0f, 7.0f, 7.0f }), colors[i]);
	}





	// 구름
// 기존 구름 수정
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


	glutDisplayFunc(drawScene);		// 출력 콜백 함수
	glutMouseFunc(Mouse);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKeyboard);
	glutReshapeFunc(Reshape);
	glutPassiveMotionFunc(PassiveMotion);
	glutTimerFunc(render_freq, TimerFunction, 1);

	glutMainLoop();
}

//--- 출력 콜백함수

GLvoid drawScene()
{
	if (DayMode) glClearColor(0.678f, 0.847f, 0.902f, 1.0f);
	else if (NightMode) glClearColor(0.0, 0.0, 0.0, 1.0f);

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
	switch (key) {
		// 밤낮 바꾸기
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
	// 시점 변환
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
	// 놀이기구
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
	// 상하좌우 이동
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

			// 초기 상태에서 계산
			glm::mat4 orbit = glm::mat4(1.0f);
			orbit = glm::translate(orbit, glm::vec3(ship_pos));  // 바이킹의 기준 위치로 이동
			orbit = glm::rotate(orbit, glm::radians(currentAngle), glm::vec3(0.0, 0.0, 1.0)); // 회전
			orbit = glm::translate(orbit, glm::vec3(-ship_pos)); // 기준 위치를 원점으로 복귀

			v->modelMatrix = v->modelMatrix;  // 새로운 변환 행렬 적용
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