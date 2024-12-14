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
#include<Windows.h>
#include<mmsystem.h>
#include <math.h>

#pragma comment(lib,"winmm.lib")
#define M_Run "sounds/amusementPark.wav"

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
void MoveRollerCoaster();

#define WINDOW_WIDTH 900
#define WINDOW_HEIGHT 900

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
float minAngle = -90.0;             // 최소 각도 (시작 지점)
float maxAngle = 90.0f;             // 최대 각도 (끝 지점)
float angleStep = 1.0f;              // 한 프레임당 각도 변화량 (속도)

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

bool isSlow = false;
glm::mat4 prev_m{};
glm::mat4 prev_b_m{};

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

	OneMode = false;
	ThreeMode = true;

	// 카메라 위치 설정
	camera->SetLocation({ 0.0, 100.0, 70.0 });
	camera->SetLookLocation({ 0.0, 0.0, -30.0 });

	render->SetCamera(camera);
	render->SetLight(light);

	mgr->AddObject(mymelody, glm::vec3({ -30.0, -40.0, -135.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 10.0, 10.0, 10.0 }), glm::vec3({ 1.0f, 1.0f, 1.0f }));
	mgr->AddObject(kitty, glm::vec3({ 30.0, -40.0, -135.0 }), glm::vec3({ 0.0, 0.0, 180.0 }),
		glm::vec3({ 10.0, 10.0, 10.0 }), glm::vec3({ 1.0f, 1.0f, 1.0f }));

	// 땅
	mgr->AddObject(base, glm::vec3({ 0.0, -40.0, -70.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 40.0, 1.0, 40.0 }), glm::vec3({ 0.6f, 0.933f, 0.565f }));

	// 관람차
	mgr->AddObject(wheel_body, glm::vec3({ 0.0, -40.0, -65.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 1.0, 1.0, 1.0 }), glm::vec3({ 1.0f, 0.713f, 0.756f }));
	mgr->AddObject(wheel_car, glm::vec3({ 0.0, -8.0, -65.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
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
	mgr->AddObject(roller_coaster_body1, glm::vec3({ 0.0, -36.0, -114.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 0.0005,  0.0005, 0.0005 }), glm::vec3({ 1.0f, 0.95f, 0.8f }));
	mgr->AddObject(roller_coaster_body2, glm::vec3({ 5.0, -36.0, -114.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 0.0005,  0.0005, 0.0005 }), glm::vec3({ 1.0f, 0.713f, 0.756f }));
	mgr->AddObject(roller_coaster_body3, glm::vec3({ 10.0, -36.0, -114.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 0.0005,  0.0005, 0.0005 }), glm::vec3({ 0.88f, 0.74f, 0.91f }));

	ship_pos = glm::vec3({ -50.0, -0.0, -10.0 }); // 바이킹 회전축

	mgr->AddObject(tree_leaf, glm::vec3({ -60.0, -40.0, -100.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 4.0, 4.0, 4.0 }), glm::vec3({ 0.3f, 0.7f, 0.3f }));
	mgr->AddObject(tree_wood, glm::vec3({ -60.0, -40.0, -100.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 4.0, 4.0, 4.0 }), glm::vec3({ 0.65f, 0.4f, 0.2f }));

	mgr->AddObject(tree_leaf, glm::vec3({ -50.0, -40.0, -90.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 3.0, 3.0, 3.0 }), glm::vec3({ 0.3f, 0.7f, 0.3f }));
	mgr->AddObject(tree_wood, glm::vec3({ -50.0, -40.0, -90.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 3.0, 3.0, 3.0 }), glm::vec3({ 0.65f, 0.4f, 0.2f }));

	mgr->AddObject(tree_leaf, glm::vec3({ 60.0, -40.0, -100.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 4.0, 4.0, 4.0 }), glm::vec3({ 0.3f, 0.7f, 0.3f }));
	mgr->AddObject(tree_wood, glm::vec3({ 60.0, -40.0, -100.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 4.0, 4.0, 4.0 }), glm::vec3({ 0.65f, 0.4f, 0.2f }));

	mgr->AddObject(tree_leaf, glm::vec3({ 50.0, -40.0, -90.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 3.0, 3.0, 3.0 }), glm::vec3({ 0.3f, 0.7f, 0.3f }));
	mgr->AddObject(tree_wood, glm::vec3({ 50.0, -40.0, -90.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 3.0, 3.0, 3.0 }), glm::vec3({ 0.65f, 0.4f, 0.2f }));


	mgr->AddObject(tree_leaf, glm::vec3({ -40.0, -40.0, -140.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 4.0, 4.0, 4.0 }), glm::vec3({ 0.3f, 0.7f, 0.3f }));
	mgr->AddObject(tree_wood, glm::vec3({ -40.0, -40.0, -140.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 4.0, 4.0, 4.0 }), glm::vec3({ 0.65f, 0.4f, 0.2f }));

	mgr->AddObject(tree_leaf, glm::vec3({ -50.0, -40.0, -130.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 3.0, 3.0, 3.0 }), glm::vec3({ 0.3f, 0.7f, 0.3f }));
	mgr->AddObject(tree_wood, glm::vec3({ -50.0, -40.0, -130.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 3.0, 3.0, 3.0 }), glm::vec3({ 0.65f, 0.4f, 0.2f }));

	mgr->AddObject(tree_leaf, glm::vec3({ 40.0, -40.0, -140.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 4.0, 4.0, 4.0 }), glm::vec3({ 0.3f, 0.7f, 0.3f }));
	mgr->AddObject(tree_wood, glm::vec3({ 40.0, -40.0, -140.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 4.0, 4.0, 4.0 }), glm::vec3({ 0.65f, 0.4f, 0.2f }));

	mgr->AddObject(tree_leaf, glm::vec3({ 50.0, -40.0, -130.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 3.0, 3.0, 3.0 }), glm::vec3({ 0.3f, 0.7f, 0.3f }));
	mgr->AddObject(tree_wood, glm::vec3({ 50.0, -40.0, -130.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 3.0, 3.0, 3.0 }), glm::vec3({ 0.65f, 0.4f, 0.2f }));

	mgr->AddObject(tree_leaf, glm::vec3({ -60.0, -40.0, 0.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 4.0, 4.0, 4.0 }), glm::vec3({ 0.3f, 0.7f, 0.3f }));
	mgr->AddObject(tree_wood, glm::vec3({ -60.0, -40.0, 0.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 4.0, 4.0, 4.0 }), glm::vec3({ 0.65f, 0.4f, 0.2f }));

	mgr->AddObject(tree_leaf, glm::vec3({ 60.0, -40.0, 0.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 4.0, 4.0, 4.0 }), glm::vec3({ 0.3f, 0.7f, 0.3f }));
	mgr->AddObject(tree_wood, glm::vec3({ 60.0, -40.0, 0.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 4.0, 4.0, 4.0 }), glm::vec3({ 0.65f, 0.4f, 0.2f }));

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

	mgr->AddObject(balloon, glm::vec3({ -50.0, 35.0, -120.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 0.05, 0.05, 0.05 }), glm::vec3({ 1.0f, 0.95f, 0.8f }));
	mgr->AddObject(basket, glm::vec3({ -50.0, 35.0, -120.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 0.05, 0.05, 0.05 }), glm::vec3({ 0.65f, 0.4f, 0.2f }));


	// 구름
// 기존 구름 수정
	mgr->AddObject(cloud, glm::vec3({ -50.0, 45.0, -120.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 1.0, 1.0, 1.0 }), glm::vec3({ 1.0f, 1.0, 1.0 }));
	mgr->AddObject(cloud, glm::vec3({ -30.0, 50.0, -30.0 }), glm::vec3({ 0.0, 10.0, 0.0 }),
		glm::vec3({ 1.5, 1.5, 1.5 }), glm::vec3({ 1.0f, 1.0, 1.0 }));
	mgr->AddObject(cloud, glm::vec3({ 30.0, 50.0, -40.0 }), glm::vec3({ 0.0, 30.0, 0.0 }),
		glm::vec3({ 1.2, 1.2, 1.2 }), glm::vec3({ 1.0f, 1.0, 1.0 }));
	mgr->AddObject(cloud, glm::vec3({ 50.0, 45.0, -10.0 }), glm::vec3({ 0.0, 30.0, 0.0 }),
		glm::vec3({ 1.2, 1.2, 1.2 }), glm::vec3({ 1.0f, 1.0, 1.0 }));
	mgr->AddObject(cloud, glm::vec3({ -40.0, 50.0, -50.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 1.3, 1.3, 1.3 }), glm::vec3({ 1.0f, 1.0, 1.0 }));
	mgr->AddObject(cloud, glm::vec3({ 20.0, 55.0, -60.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 2.0, 2.0, 2.0 }), glm::vec3({ 1.0f, 1.0, 1.0 }));
	mgr->AddObject(cloud, glm::vec3({ 0.0, 45.0, -70.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 2.0, 2.0, 2.0 }), glm::vec3({ 1.0f, 1.0, 1.0 }));
	mgr->AddObject(cloud, glm::vec3({ 40.0, 40.0, -20.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 1.0, 1.0, 1.0 }), glm::vec3({ 1.0f, 1.0, 1.0 }));
	mgr->AddObject(cloud, glm::vec3({ 10.0, 50.0, -40.0 }), glm::vec3({ 0.0, 5.0, 0.0 }),
		glm::vec3({ 1.4, 1.4, 1.4 }), glm::vec3({ 1.0f, 1.0, 1.0 }));
	mgr->AddObject(cloud, glm::vec3({ -20.0, 50.0, -30.0 }), glm::vec3({ 0.0, 15.0, 0.0 }),
		glm::vec3({ 1.6, 1.6, 1.6 }), glm::vec3({ 1.0f, 1.0, 1.0 }));
	mgr->AddObject(cloud, glm::vec3({ 30.0, 50.0, -20.0 }), glm::vec3({ 0.0, 20.0, 0.0 }),
		glm::vec3({ 1.3, 1.3, 1.3 }), glm::vec3({ 1.0f, 1.0, 1.0 }));
	mgr->AddObject(cloud, glm::vec3({ -30.0, 45.0, 0.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 1.2, 1.2, 1.2 }), glm::vec3({ 1.0f, 1.0, 1.0 }));
	mgr->AddObject(cloud, glm::vec3({ 0.0, 55.0, -50.0 }), glm::vec3({ 0.0, 25.0, 0.0 }),
		glm::vec3({ 1.8, 1.8, 1.8 }), glm::vec3({ 1.0f, 1.0, 1.0 }));
	mgr->AddObject(cloud, glm::vec3({ 20.0, 50.0, -50.0 }), glm::vec3({ 0.0, 35.0, 0.0 }),
		glm::vec3({ 1.7, 1.7, 1.7 }), glm::vec3({ 1.0f, 1.0, 1.0 }));
	mgr->AddObject(cloud, glm::vec3({ -50.0, 45.0, -10.0 }), glm::vec3({ 0.0, 40.0, 0.0 }),
		glm::vec3({ 1.5, 1.5, 1.5 }), glm::vec3({ 1.0f, 1.0, 1.0 }));

	mgr->AddObject(cloud, glm::vec3({ -30.0, 50.0, -20.0 }), glm::vec3({ 0.0, 20.0, 0.0 }),
		glm::vec3({ 1.3, 1.3, 1.3 }), glm::vec3({ 1.0f, 1.0, 1.0 }));
	mgr->AddObject(cloud, glm::vec3({ -30.0, 45.0, -80.0 }), glm::vec3({ 0.0, 0.0, 0.0 }),
		glm::vec3({ 1.2, 1.2, 1.2 }), glm::vec3({ 1.0f, 1.0, 1.0 }));
	mgr->AddObject(cloud, glm::vec3({ 50.0, 55.0, -50.0 }), glm::vec3({ 0.0, 25.0, 0.0 }),
		glm::vec3({ 1.8, 1.8, 1.8 }), glm::vec3({ 1.0f, 1.0, 1.0 }));
	mgr->AddObject(cloud, glm::vec3({ 20.0, 50.0, -100.0 }), glm::vec3({ 0.0, 35.0, 0.0 }),
		glm::vec3({ 1.7, 1.7, 1.7 }), glm::vec3({ 1.0f, 1.0, 1.0 }));
	mgr->AddObject(cloud, glm::vec3({ -50.0, 45.0, -90.0 }), glm::vec3({ 0.0, 40.0, 0.0 }),
		glm::vec3({ 1.5, 1.5, 1.5 }), glm::vec3({ 1.0f, 1.0, 1.0 }));

	PlaySound(TEXT(M_Run), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);

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
		if (!RollerCoaster && !MerryGoRound && !Viking) {
			OneMode = true;
			ThreeMode = false;
			camera->SetLocation({ 0.0, -30.0, 60.0 });
			camera->SetLookLocation({ 0.0, 0.0, 0.0 });
			camera->angle = 70.0f;
		}
	}
	break;
	case '3':
	{
		if (!RollerCoaster && !MerryGoRound && !Viking) {
			ThreeMode = true;
			OneMode = false;
			camera->SetLocation({ 0.0, 100.0, 70.0 });
			camera->SetLookLocation({ 0.0, 0.0, -30.0 });
			camera->angle = 70.0f;
		}
	}
	break;
	// 놀이기구
	case '7': {
		if (!RollerCoaster && !Viking && !ThreeMode) {
			MerryGoRound = !MerryGoRound;
			if (MerryGoRound) {

				camera->SetLocation({ 45.0, -25.0, -42.0 });
				camera->SetLookLocation({ 0.0, 0.0, 0.0 });
				camera->angle = 100.0f;
			}
			else {
				camera->SetLocation({ 50.0, -30.0, 0.0 });
				camera->SetLookLocation({ 50.0, 0.0, -60.0 });
				camera->angle = 70.0f;
			}
		}
	}
			break;
	case '8': {
		if (!RollerCoaster && !MerryGoRound && !ThreeMode) {
			Viking = !Viking;
			if (Viking) {
				glm::mat4 orbit = glm::mat4(1.0f);
				//orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
				//orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				camera->camMatrix = orbit * camera->camMatrix;
				camera->cam_Look_Matrix = orbit * camera->cam_Look_Matrix;

				camera->SetLocation({ -50.0, -20.0, -30.0 });
				camera->SetLookLocation({ 70.0, 0.0, -30.0 });
				camera->angle = 100.0f;
			}
			else {
				camera->SetLocation({ -50.0, -30.0, 0.0 });
				camera->SetLookLocation({ -50.0, 0.0, -120.0 });
				camera->angle = 70.0f;
			}
		}
	}
			break;
	case '9': {
		if (!MerryGoRound && !Viking && !ThreeMode) {
			RollerCoaster = !RollerCoaster;
			if (RollerCoaster) {
				for (auto& v : mgr->GetAllObjs()) {
					if (v->GetType() == roller_coaster_head) {
						camera->SetLocation({ v->modelMatrix[3].x + 5, v->modelMatrix[3].y + 10, v->modelMatrix[3].z });
						camera->SetLookLocation({ v->modelMatrix[3].x + 5 , 0.0, -v->modelMatrix[3].z });
						camera->angle = 100.0f;
					}
				}
				glm::mat4 orbit = glm::mat4(1.0f);
				orbit = glm::translate(orbit, glm::vec3(camera->camMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
				orbit = glm::translate(orbit, glm::vec3(-camera->camMatrix[3]));
				camera->camMatrix = orbit * camera->camMatrix;
				camera->cam_Look_Matrix = orbit * camera->cam_Look_Matrix;
			}
			else {
				camera->SetLocation({ -30.0, -30.0, -80.0 });
				camera->SetLookLocation({ -30.0, 0.0, -190.0 });
				camera->angle = 70.0f;
			}
		}
	}
			break;
			// 상하좌우 이동
	case 'w':
	case 'W':
	{
		if (OneMode && !ThreeMode && !RollerCoaster && !MerryGoRound && !Viking) {
			camera->ChangeLocation({ 0.0, 0.0, -1.0 });
			camera->ChangeLookLocation({ 0.0, 0.0, -1.0 });
		}
	}
	break;
	case 's':
	case 'S':
	{
		if (OneMode && !ThreeMode && !RollerCoaster && !MerryGoRound && !Viking) {
			camera->ChangeLocation({ 0.0, 0.0,1.0 });
			camera->ChangeLookLocation({ 0.0, 0.0,1.0 });
		}
	}
	break;
	case 'a':
	case 'A':
	{
		if (OneMode && !ThreeMode && !RollerCoaster && !MerryGoRound && !Viking) {
			camera->ChangeLocation({ -1.0, 0.0, 0.0 });
			camera->ChangeLookLocation({ -1.0, 0.0, 0.0 });
		}
	}
	break;
	case 'd':
	case 'D':
	{
		if (OneMode && !ThreeMode && !RollerCoaster && !MerryGoRound && !Viking) {
			camera->ChangeLocation({ 1.0, 0.0, 0.0 });
			camera->ChangeLookLocation({ 1.0, 0.0, 0.0 });
		}
	}
	break;
	case 'o':
	case 'O': {
		OpenDoor = !OpenDoor;
	}

	}
	glutPostRedisplay();
}

void SpecialKeyboard(int key, int x, int y) {
	// 카메라 방향 조절
	if (OneMode && !RollerCoaster && !MerryGoRound && !Viking) {
		switch (key) {
		case GLUT_KEY_LEFT: {

			glm::mat4 orbit = glm::mat4(1.0f);
			orbit = glm::translate(orbit, glm::vec3(camera->camMatrix[3]));
			orbit = glm::rotate(orbit, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
			orbit = glm::translate(orbit, glm::vec3(-camera->camMatrix[3]));
			camera->camMatrix = orbit * camera->camMatrix;
			camera->cam_Look_Matrix = orbit * camera->cam_Look_Matrix;
		}
						  break;
		case GLUT_KEY_RIGHT: {
			glm::mat4 orbit = glm::mat4(1.0f);
			orbit = glm::translate(orbit, glm::vec3(camera->camMatrix[3]));
			orbit = glm::rotate(orbit, glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
			orbit = glm::translate(orbit, glm::vec3(-camera->camMatrix[3]));
			camera->camMatrix = orbit * camera->camMatrix;
			camera->cam_Look_Matrix = orbit * camera->cam_Look_Matrix;
		}

						   break;
		case GLUT_KEY_DOWN: {
			glm::mat4 orbit = glm::mat4(1.0f);
			orbit = glm::translate(orbit, glm::vec3(camera->camMatrix[3]));
			orbit = glm::rotate(orbit, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
			orbit = glm::translate(orbit, glm::vec3(-camera->camMatrix[3]));
			camera->camMatrix = orbit * camera->camMatrix;
			camera->cam_Look_Matrix = orbit * camera->cam_Look_Matrix;
		}

						  break;
		case GLUT_KEY_UP: {
			glm::mat4 orbit = glm::mat4(1.0f);
			orbit = glm::translate(orbit, glm::vec3(camera->camMatrix[3]));
			orbit = glm::rotate(orbit, glm::radians(-180.0f), glm::vec3(0.0, 1.0, 0.0));
			orbit = glm::translate(orbit, glm::vec3(-camera->camMatrix[3]));
			camera->camMatrix = orbit * camera->camMatrix;
			camera->cam_Look_Matrix = orbit * camera->cam_Look_Matrix;
		}

						break;
		}
	}
	glutPostRedisplay();
}


void MoveRollerCoaster()
{
	for (auto& v : mgr->GetAllObjs()) {
		if (v->GetType() == roller_coaster_head) {
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
			else if (v->GetMatrix()[3].x <= -65.4 && v->GetMatrix()[3].y <= 18.0 && v->GetMatrix()[3].z >= -122) {   //위로 직진
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
			//롤러코스터 낙하 
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
			//평지 이동 속도
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
				glm::vec3 loc = glm::vec3(70.0, -36.0, -114.0);  // 위치
				glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0);        // 회전 (라디안)
				glm::vec3 _size = glm::vec3(0.0005, 0.0005, 0.0005); // 크기

				// 초기화
				v->modelMatrix = glm::mat4(1.0f);

				// 크기 조정
				glm::mat4 scale = glm::scale(glm::mat4(1.0f), _size);

				// 회전 설정 (XYZ 순서로 회전)
				glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), rot.x, glm::vec3(1.0, 0.0, 0.0)); // X축 회전
				rotation = glm::rotate(rotation, rot.y, glm::vec3(0.0, 1.0, 0.0));                 // Y축 회전
				rotation = glm::rotate(rotation, rot.z, glm::vec3(0.0, 0.0, 1.0));                 // Z축 회전

				// 위치 이동
				glm::mat4 translation = glm::translate(glm::mat4(1.0f), loc);

				// 변환 순서: Translation * Rotation * Scale
				v->modelMatrix = translation * rotation * scale;

				// 디버그 출력
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
			else if (v->GetMatrix()[3].x <= -65.4 && v->GetMatrix()[3].y <= 14.0 && v->GetMatrix()[3].z >= -122) {   //위로 직진
				orbit = glm::translate(orbit, glm::vec3(0.0, 0.1, 0.0));
			}
			else if (v->GetMatrix()[3].x <= -65.4 && v->GetMatrix()[3].y <= 18.0 && v->GetMatrix()[3].z >= -122) {   //위로 직진
				orbit = glm::translate(orbit, glm::vec3(0.0, 0.05, 0.0));
			}
			else if (v->GetMatrix()[3].x <= 60.0 && v->GetMatrix()[3].x > -30.0 && v->GetMatrix()[3].z >= -122) {	//왼쪽으로 이동
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
			////롤러코스터 낙하 a
			else if (v->GetMatrix()[3].x <= 60.0 && v->GetMatrix()[3].y >= 17.6) {
				orbit = glm::translate(orbit, glm::vec3(0.0, -0.08, 0.0));
			/*	std::cout << "x      " << v->GetMatrix()[3].x << std::endl;
				std::cout << "y      " << v->GetMatrix()[3].y << std::endl;
				std::cout << "z      " << v->GetMatrix()[3].z << std::endl;*/
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
			//평지 이동 속도
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
				//printMatrix(v->modelMatrix, "v->modelMatrix");
			}
			else if (v->GetMatrix()[3].x >= 70.5 && v->GetMatrix()[3].z >= -114.5) {
				glm::vec3 loc = glm::vec3(70.0, -36.0, -114.0);  // 위치
				glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0);        // 회전 (라디안)
				glm::vec3 _size = glm::vec3(0.0005, 0.0005, 0.0005); // 크기

				// 초기화
				v->modelMatrix = glm::mat4(1.0f);

				// 크기 조정
				glm::mat4 scale = glm::scale(glm::mat4(1.0f), _size);

				// 회전 설정 (XYZ 순서로 회전)
				glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), rot.x, glm::vec3(1.0, 0.0, 0.0)); // X축 회전
				rotation = glm::rotate(rotation, rot.y, glm::vec3(0.0, 1.0, 0.0));                 // Y축 회전
				rotation = glm::rotate(rotation, rot.z, glm::vec3(0.0, 0.0, 1.0));                 // Z축 회전
				//glm::mat4 rotation = glm::mat4(1.0f); // 회전 제거
				// 위치 이동
				glm::mat4 translation = glm::translate(glm::mat4(1.0f), loc);

				// 변환 순서: Translation * Rotation * Scale
				v->modelMatrix = translation * rotation * scale;

				// 디버그 출력
				//printMatrix(v->modelMatrix, "v->modelMatrix");

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
			prev_m = v->modelMatrix;

		}
		else if (v->GetType() == roller_coaster_body2) {
			glm::mat4 orbit = glm::mat4(1.0f);
			if (prev_m[3].x > -45.0 && v->GetMatrix()[3].z >= -114.0) {	// 왼쪽이동
				glm::mat4 orbit = glm::mat4(1.0f);
				glm::vec3 loc = glm::vec3(prev_m[3].x + 5.8, -36.0, -114.0);  // 위치
				glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0);        // 회전 (라디안)
				glm::vec3 _size = glm::vec3(0.0005, 0.0005, 0.0005); // 크기

				// 초기화
				v->modelMatrix = glm::mat4(1.0f);

				// 크기 조정
				glm::mat4 scale = glm::scale(glm::mat4(1.0f), _size);

				// 회전 설정 (XYZ 순서로 회전)
				glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), rot.x, glm::vec3(1.0, 0.0, 0.0)); // X축 회전
				rotation = glm::rotate(rotation, rot.y, glm::vec3(0.0, 1.0, 0.0));                 // Y축 회전
				rotation = glm::rotate(rotation, rot.z, glm::vec3(0.0, 0.0, 1.0));                 // Z축 회전

				// 위치 이동
				glm::mat4 translation = glm::translate(glm::mat4(1.0f), loc);

				// 변환 순서: Translation * Rotation * Scale
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
			else if (prev_m[3].x <= 64.5 && prev_m[3].z == -114.0) {	//올라가는 부분
				glm::mat4 orbit = glm::mat4(1.0f);
				glm::vec3 loc = glm::vec3(prev_m[3].x, prev_m[3].y - 5.8, -114.0);  // 위치
				glm::vec3 rot = glm::vec3(0.0, 0.0, -1.5708);        // 회전 (라디안)
				glm::vec3 _size = glm::vec3(0.0005, 0.0005, 0.0005); // 크기

				// 초기화
				v->modelMatrix = glm::mat4(1.0f);

				// 크기 조정
				glm::mat4 scale = glm::scale(glm::mat4(1.0f), _size);

				// 회전 설정 (XYZ 순서로 회전)
				glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), rot.x, glm::vec3(1.0, 0.0, 0.0)); // X축 회전
				rotation = glm::rotate(rotation, rot.y, glm::vec3(0.0, 1.0, 0.0));                 // Y축 회전
				rotation = glm::rotate(rotation, rot.z, glm::vec3(0.0, 0.0, 1.0));                 // Z축 회전

				// 위치 이동
				glm::mat4 translation = glm::translate(glm::mat4(1.0f), loc);

				// 변환 순서: Translation * Rotation * Scale
				v->modelMatrix = translation * rotation * scale;
			}
			else if (v->GetMatrix()[3].x <= -65.4 && v->GetMatrix()[3].y <= 18.0 && v->GetMatrix()[3].z >= -122) {   //위로 직진
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
			else if (v->GetMatrix()[3].x <= 60.0 && v->GetMatrix()[3].y >= -8.5 && prev_m[3].z <= -124.0) {	//내려가는 부분
				glm::mat4 orbit = glm::mat4(1.0f);
				glm::vec3 loc = glm::vec3(prev_m[3].x, prev_m[3].y + 5.8, prev_m[3].z);  // 위치
				glm::vec3 rot = glm::vec3(0.0, 3.1416, 1.5708);        // 회전 (라디안)
				glm::vec3 _size = glm::vec3(0.0005, 0.0005, 0.0005); // 크기

				// 초기화
				v->modelMatrix = glm::mat4(1.0f);

				// 크기 조정
				glm::mat4 scale = glm::scale(glm::mat4(1.0f), _size);

				// 회전 설정 (XYZ 순서로 회전)
				glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), rot.x, glm::vec3(1.0, 0.0, 0.0)); // X축 회전
				rotation = glm::rotate(rotation, rot.y, glm::vec3(0.0, 1.0, 0.0));                 // Y축 회전
				rotation = glm::rotate(rotation, rot.z, glm::vec3(0.0, 0.0, 1.0));                 // Z축 회전

				// 위치 이동
				glm::mat4 translation = glm::translate(glm::mat4(1.0f), loc);

				// 변환 순서: Translation * Rotation * Scale
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
			else if (prev_m[3].x <= 68.0 && prev_m[3].z <= -124.0) {	//내려간 후 평지
				glm::mat4 orbit = glm::mat4(1.0f);
				glm::vec3 loc = glm::vec3(prev_m[3].x - 5.8, prev_m[3].y, prev_m[3].z);  // 위치
				glm::vec3 rot = glm::vec3(0.0, 3.1416, 0.0);        // 회전 (라디안)
				glm::vec3 _size = glm::vec3(0.0005, 0.0005, 0.0005); // 크기

				// 초기화
				v->modelMatrix = glm::mat4(1.0f);

				// 크기 조정
				glm::mat4 scale = glm::scale(glm::mat4(1.0f), _size);

				// 회전 설정 (XYZ 순서로 회전)
				glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), rot.x, glm::vec3(1.0, 0.0, 0.0)); // X축 회전
				rotation = glm::rotate(rotation, rot.y, glm::vec3(0.0, 1.0, 0.0));                 // Y축 회전
				rotation = glm::rotate(rotation, rot.z, glm::vec3(0.0, 0.0, 1.0));                 // Z축 회전

				// 위치 이동
				glm::mat4 translation = glm::translate(glm::mat4(1.0f), loc);

				// 변환 순서: Translation * Rotation * Scale
				v->modelMatrix = translation * rotation * scale;
			}
			else if (v->GetMatrix()[3].x <= 68.0 && v->GetMatrix()[3].z <= -120.0) {	//
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
				//printMatrix(v->modelMatrix, "v->modelMatrix");
			}
			else if (v->GetMatrix()[3].x >= 70.5 && v->GetMatrix()[3].z >= -114.5) {
				glm::vec3 loc = glm::vec3(70.0, -36.0, -114.0);  // 위치
				glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0);        // 회전 (라디안)
				glm::vec3 _size = glm::vec3(0.0005, 0.0005, 0.0005); // 크기

				// 초기화
				v->modelMatrix = glm::mat4(1.0f);

				// 크기 조정
				glm::mat4 scale = glm::scale(glm::mat4(1.0f), _size);

				// 회전 설정 (XYZ 순서로 회전)
				glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), rot.x, glm::vec3(1.0, 0.0, 0.0)); // X축 회전
				rotation = glm::rotate(rotation, rot.y, glm::vec3(0.0, 1.0, 0.0));                 // Y축 회전
				rotation = glm::rotate(rotation, rot.z, glm::vec3(0.0, 0.0, 1.0));                 // Z축 회전
				//glm::mat4 rotation = glm::mat4(1.0f); // 회전 제거
				// 위치 이동
				glm::mat4 translation = glm::translate(glm::mat4(1.0f), loc);

				// 변환 순서: Translation * Rotation * Scale
				v->modelMatrix = translation * rotation * scale;

				// 디버그 출력
				//printMatrix(v->modelMatrix, "v->modelMatrix");

			}
			v->modelMatrix = orbit * v->modelMatrix;
			prev_b_m = v->modelMatrix;
		}
		else if (v->GetType() == roller_coaster_body3) {
			glm::mat4 orbit = glm::mat4(1.0f);
			if (prev_b_m[3].x <= 65.0 && prev_b_m[3].x > -45.0 && v->GetMatrix()[3].z >= -114.0) {	// 왼쪽이동
				glm::mat4 orbit = glm::mat4(1.0f);
				glm::vec3 loc = glm::vec3(prev_b_m[3].x + 5.8, -36.0, -114.0);  // 위치
				glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0);        // 회전 (라디안)
				glm::vec3 _size = glm::vec3(0.0005, 0.0005, 0.0005); // 크기

				// 초기화
				v->modelMatrix = glm::mat4(1.0f);

				// 크기 조정
				glm::mat4 scale = glm::scale(glm::mat4(1.0f), _size);

				// 회전 설정 (XYZ 순서로 회전)
				glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), rot.x, glm::vec3(1.0, 0.0, 0.0)); // X축 회전
				rotation = glm::rotate(rotation, rot.y, glm::vec3(0.0, 1.0, 0.0));                 // Y축 회전
				rotation = glm::rotate(rotation, rot.z, glm::vec3(0.0, 0.0, 1.0));                 // Z축 회전

				// 위치 이동
				glm::mat4 translation = glm::translate(glm::mat4(1.0f), loc);

				// 변환 순서: Translation * Rotation * Scale
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
				orbit = glm::translate(orbit, glm::vec3(-0.031, 0.075, 0.0));
			}
			else if (v->GetMatrix()[3].x > -65.4 && v->GetMatrix()[3].x <= -63.5 && v->GetMatrix()[3].z >= -122) {
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.185f), glm::vec3(0.0, 0.0, -1.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(-0.0267, 0.11, 0.0));
			}
			else if (prev_b_m[3].x <= 64.5 && prev_b_m[3].z == -114.0) {	//올라가는 부분
				glm::mat4 orbit = glm::mat4(1.0f);
				glm::vec3 loc = glm::vec3(prev_b_m[3].x, prev_b_m[3].y - 5.8, -114.0);  // 위치
				glm::vec3 rot = glm::vec3(0.0, 0.0, -1.5708);        // 회전 (라디안)
				glm::vec3 _size = glm::vec3(0.0005, 0.0005, 0.0005); // 크기

				// 초기화
				v->modelMatrix = glm::mat4(1.0f);

				// 크기 조정
				glm::mat4 scale = glm::scale(glm::mat4(1.0f), _size);

				// 회전 설정 (XYZ 순서로 회전)
				glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), rot.x, glm::vec3(1.0, 0.0, 0.0)); // X축 회전
				rotation = glm::rotate(rotation, rot.y, glm::vec3(0.0, 1.0, 0.0));                 // Y축 회전
				rotation = glm::rotate(rotation, rot.z, glm::vec3(0.0, 0.0, 1.0));                 // Z축 회전

				// 위치 이동
				glm::mat4 translation = glm::translate(glm::mat4(1.0f), loc);

				// 변환 순서: Translation * Rotation * Scale
				v->modelMatrix = translation * rotation * scale;
			}
			else if (v->GetMatrix()[3].x <= -65.4 && v->GetMatrix()[3].y <= 18.0 && v->GetMatrix()[3].z >= -122) {   //위로 직진
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
				orbit = glm::rotate(orbit, glm::radians(0.81f), glm::vec3(-1.0, 0.0, 0.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(0.0, 0.012, -0.051));
			}
			else if (v->GetMatrix()[3].x <= 60.0 && v->GetMatrix()[3].y > 21.0 && v->GetMatrix()[3].z <= -123.0) {
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.45f), glm::vec3(-1.0, 0.0, 0.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(0.0, -0.08, -0.034));
			}
			else if (v->GetMatrix()[3].x <= 60.0 && v->GetMatrix()[3].y >= -8.5 && prev_b_m[3].z <= -124.0) {	//내려가는 부분
				glm::mat4 orbit = glm::mat4(1.0f);
				glm::vec3 loc = glm::vec3(prev_b_m[3].x, prev_b_m[3].y + 5.8, prev_b_m[3].z);  // 위치
				glm::vec3 rot = glm::vec3(0.0, 3.1416, 1.5708);        // 회전 (라디안)
				glm::vec3 _size = glm::vec3(0.0005, 0.0005, 0.0005); // 크기

				// 초기화
				v->modelMatrix = glm::mat4(1.0f);

				// 크기 조정
				glm::mat4 scale = glm::scale(glm::mat4(1.0f), _size);

				// 회전 설정 (XYZ 순서로 회전)
				glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), rot.x, glm::vec3(1.0, 0.0, 0.0)); // X축 회전
				rotation = glm::rotate(rotation, rot.y, glm::vec3(0.0, 1.0, 0.0));                 // Y축 회전
				rotation = glm::rotate(rotation, rot.z, glm::vec3(0.0, 0.0, 1.0));                 // Z축 회전

				// 위치 이동
				glm::mat4 translation = glm::translate(glm::mat4(1.0f), loc);

				// 변환 순서: Translation * Rotation * Scale
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
			else if (v->GetMatrix()[3].x >= -43.0 && v->GetMatrix()[3].x <= -36.0 && v->GetMatrix()[3].z <= -120.0) {	//
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.85f), glm::vec3(0.0, 0.0, 1.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(0.56, -0.053, 0.0));
			}
			else if (prev_b_m[3].x <= 68.0 && prev_b_m[3].z <= -124.0) {	//내려간 후 평지
				glm::mat4 orbit = glm::mat4(1.0f);
				glm::vec3 loc = glm::vec3(prev_b_m[3].x - 5.8, prev_b_m[3].y, prev_b_m[3].z);  // 위치
				glm::vec3 rot = glm::vec3(0.0, 3.1416, 0.0);        // 회전 (라디안)
				glm::vec3 _size = glm::vec3(0.0005, 0.0005, 0.0005); // 크기

				// 초기화
				v->modelMatrix = glm::mat4(1.0f);

				// 크기 조정
				glm::mat4 scale = glm::scale(glm::mat4(1.0f), _size);

				// 회전 설정 (XYZ 순서로 회전)
				glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), rot.x, glm::vec3(1.0, 0.0, 0.0)); // X축 회전
				rotation = glm::rotate(rotation, rot.y, glm::vec3(0.0, 1.0, 0.0));                 // Y축 회전
				rotation = glm::rotate(rotation, rot.z, glm::vec3(0.0, 0.0, 1.0));                 // Z축 회전

				// 위치 이동
				glm::mat4 translation = glm::translate(glm::mat4(1.0f), loc);

				// 변환 순서: Translation * Rotation * Scale
				v->modelMatrix = translation * rotation * scale;
			}
			else if (v->GetMatrix()[3].x <= 68.0 && v->GetMatrix()[3].z <= -120.0) {	//
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
				//printMatrix(v->modelMatrix, "v->modelMatrix");
			}
			else if (v->GetMatrix()[3].x >= 70.5 && v->GetMatrix()[3].z >= -114.5) {
				glm::vec3 loc = glm::vec3(70.0, -36.0, -114.0);  // 위치
				glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0);        // 회전 (라디안)
				glm::vec3 _size = glm::vec3(0.0005, 0.0005, 0.0005); // 크기

				// 초기화
				v->modelMatrix = glm::mat4(1.0f);

				// 크기 조정
				glm::mat4 scale = glm::scale(glm::mat4(1.0f), _size);

				// 회전 설정 (XYZ 순서로 회전)
				glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), rot.x, glm::vec3(1.0, 0.0, 0.0)); // X축 회전
				rotation = glm::rotate(rotation, rot.y, glm::vec3(0.0, 1.0, 0.0));                 // Y축 회전
				rotation = glm::rotate(rotation, rot.z, glm::vec3(0.0, 0.0, 1.0));                 // Z축 회전
				//glm::mat4 rotation = glm::mat4(1.0f); // 회전 제거
				// 위치 이동
				glm::mat4 translation = glm::translate(glm::mat4(1.0f), loc);

				// 변환 순서: Translation * Rotation * Scale
				v->modelMatrix = translation * rotation * scale;

				// 디버그 출력
				//printMatrix(v->modelMatrix, "v->modelMatrix");

			}
			v->modelMatrix = orbit * v->modelMatrix;
		}
	}
}


void TimerFunction(int value)
{
	if (MerryGoRound) {
		camera->RotateCamY(1.0, glm::vec3{ 50.0, -40.0, -30.0 });
	}


	for (auto& v : mgr->GetAllObjs()) {
		if (v->GetType() == roller_coaster_head) {
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
			else if (v->GetMatrix()[3].x <= -65.4 && v->GetMatrix()[3].y <= 18.0 && v->GetMatrix()[3].z >= -122) {   //위로 직진
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
			//롤러코스터 낙하 
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
			//평지 이동 속도
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
				glm::vec3 loc = glm::vec3(70.0, -36.0, -114.0);  // 위치
				glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0);        // 회전 (라디안)
				glm::vec3 _size = glm::vec3(0.0005, 0.0005, 0.0005); // 크기

				// 초기화
				v->modelMatrix = glm::mat4(1.0f);
				if (RollerCoaster) {
					camera->camMatrix = glm::mat4(1.0f);
					camera->cam_Look_Matrix = glm::mat4(1.0f);
				}

				// 크기 조정
				glm::mat4 scale = glm::scale(glm::mat4(1.0f), _size);

				// 회전 설정 (XYZ 순서로 회전)
				glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), rot.x, glm::vec3(1.0, 0.0, 0.0)); // X축 회전
				rotation = glm::rotate(rotation, rot.y, glm::vec3(0.0, 1.0, 0.0));                 // Y축 회전
				rotation = glm::rotate(rotation, rot.z, glm::vec3(0.0, 0.0, 1.0));                 // Z축 회전

				// 위치 이동
				glm::mat4 translation = glm::translate(glm::mat4(1.0f), loc);
		
				// 변환 순서: Translation * Rotation * Scale
				v->modelMatrix = translation * rotation * scale;
				if (RollerCoaster) {
					for (auto& v : mgr->GetAllObjs()) {
						if (v->GetType() == roller_coaster_head) {
							camera->SetLocation({ v->modelMatrix[3].x + 5, v->modelMatrix[3].y + 10, v->modelMatrix[3].z });
							camera->SetLookLocation({ v->modelMatrix[3].x + 5 , 0.0, -v->modelMatrix[3].z });
							camera->angle = 100.0f;
						}
					}
					glm::mat4 orbit = glm::mat4(1.0f);
					orbit = glm::translate(orbit, glm::vec3(camera->camMatrix[3]));
					orbit = glm::rotate(orbit, glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
					orbit = glm::translate(orbit, glm::vec3(-camera->camMatrix[3]));
					camera->camMatrix = orbit * camera->camMatrix;
					camera->cam_Look_Matrix = orbit * camera->cam_Look_Matrix;
				}

				// 디버그 출력
				//printMatrix(v->modelMatrix, "v->modelMatrix");

			}
			else if (v->GetMatrix()[3].x >= 65.0) {
				orbit = glm::translate(orbit, glm::vec3(-0.09, 0.0, 0.0));
			}
			else if (v->GetMatrix()[3].x >= 20.0) {
				orbit = glm::translate(orbit, glm::vec3(-0.3, 0.0, 0.0));
			}
			v->modelMatrix = orbit * v->modelMatrix;
			if (RollerCoaster) {
				camera->camMatrix = orbit * camera->camMatrix;
				camera->cam_Look_Matrix = orbit * camera->cam_Look_Matrix;
			}
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
			else if (v->GetMatrix()[3].x <= -65.4 && v->GetMatrix()[3].y <= 14.0 && v->GetMatrix()[3].z >= -122) {   //위로 직진
				orbit = glm::translate(orbit, glm::vec3(0.0, 0.1, 0.0));
			}
			else if (v->GetMatrix()[3].x <= -65.4 && v->GetMatrix()[3].y <= 18.0 && v->GetMatrix()[3].z >= -122) {   //위로 직진
				orbit = glm::translate(orbit, glm::vec3(0.0, 0.05, 0.0));
			}
			else if (v->GetMatrix()[3].x <= 60.0 && v->GetMatrix()[3].x > -30.0 && v->GetMatrix()[3].z >= -122) {	//왼쪽으로 이동
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
			////롤러코스터 낙하 a
			else if (v->GetMatrix()[3].x <= 60.0 && v->GetMatrix()[3].y >= 17.6) {
				orbit = glm::translate(orbit, glm::vec3(0.0, -0.08, 0.0));
				/*	std::cout << "x      " << v->GetMatrix()[3].x << std::endl;
					std::cout << "y      " << v->GetMatrix()[3].y << std::endl;
					std::cout << "z      " << v->GetMatrix()[3].z << std::endl;*/
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
			//평지 이동 속도
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
				//printMatrix(v->modelMatrix, "v->modelMatrix");
			}
			else if (v->GetMatrix()[3].x >= 70.5 && v->GetMatrix()[3].z >= -114.5) {
				glm::vec3 loc = glm::vec3(70.0, -36.0, -114.0);  // 위치
				glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0);        // 회전 (라디안)
				glm::vec3 _size = glm::vec3(0.0005, 0.0005, 0.0005); // 크기

				// 초기화
				v->modelMatrix = glm::mat4(1.0f);

				// 크기 조정
				glm::mat4 scale = glm::scale(glm::mat4(1.0f), _size);

				// 회전 설정 (XYZ 순서로 회전)
				glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), rot.x, glm::vec3(1.0, 0.0, 0.0)); // X축 회전
				rotation = glm::rotate(rotation, rot.y, glm::vec3(0.0, 1.0, 0.0));                 // Y축 회전
				rotation = glm::rotate(rotation, rot.z, glm::vec3(0.0, 0.0, 1.0));                 // Z축 회전
				//glm::mat4 rotation = glm::mat4(1.0f); // 회전 제거
				// 위치 이동
				glm::mat4 translation = glm::translate(glm::mat4(1.0f), loc);

				// 변환 순서: Translation * Rotation * Scale
				v->modelMatrix = translation * rotation * scale;

				// 디버그 출력
				//printMatrix(v->modelMatrix, "v->modelMatrix");

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
			prev_m = v->modelMatrix;

		}
		else if (v->GetType() == roller_coaster_body2) {
			glm::mat4 orbit = glm::mat4(1.0f);
			if (prev_m[3].x > -45.0 && v->GetMatrix()[3].z >= -114.0) {	// 왼쪽이동
				glm::mat4 orbit = glm::mat4(1.0f);
				glm::vec3 loc = glm::vec3(prev_m[3].x + 5.8, -36.0, -114.0);  // 위치
				glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0);        // 회전 (라디안)
				glm::vec3 _size = glm::vec3(0.0005, 0.0005, 0.0005); // 크기

				// 초기화
				v->modelMatrix = glm::mat4(1.0f);

				// 크기 조정
				glm::mat4 scale = glm::scale(glm::mat4(1.0f), _size);

				// 회전 설정 (XYZ 순서로 회전)
				glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), rot.x, glm::vec3(1.0, 0.0, 0.0)); // X축 회전
				rotation = glm::rotate(rotation, rot.y, glm::vec3(0.0, 1.0, 0.0));                 // Y축 회전
				rotation = glm::rotate(rotation, rot.z, glm::vec3(0.0, 0.0, 1.0));                 // Z축 회전

				// 위치 이동
				glm::mat4 translation = glm::translate(glm::mat4(1.0f), loc);

				// 변환 순서: Translation * Rotation * Scale
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
			else if (prev_m[3].x <= 64.5 && prev_m[3].z == -114.0) {	//올라가는 부분
				glm::mat4 orbit = glm::mat4(1.0f);
				glm::vec3 loc = glm::vec3(prev_m[3].x, prev_m[3].y - 5.8, -114.0);  // 위치
				glm::vec3 rot = glm::vec3(0.0, 0.0, -1.5708);        // 회전 (라디안)
				glm::vec3 _size = glm::vec3(0.0005, 0.0005, 0.0005); // 크기

				// 초기화
				v->modelMatrix = glm::mat4(1.0f);

				// 크기 조정
				glm::mat4 scale = glm::scale(glm::mat4(1.0f), _size);

				// 회전 설정 (XYZ 순서로 회전)
				glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), rot.x, glm::vec3(1.0, 0.0, 0.0)); // X축 회전
				rotation = glm::rotate(rotation, rot.y, glm::vec3(0.0, 1.0, 0.0));                 // Y축 회전
				rotation = glm::rotate(rotation, rot.z, glm::vec3(0.0, 0.0, 1.0));                 // Z축 회전

				// 위치 이동
				glm::mat4 translation = glm::translate(glm::mat4(1.0f), loc);

				// 변환 순서: Translation * Rotation * Scale
				v->modelMatrix = translation * rotation * scale;
			}
			else if (v->GetMatrix()[3].x <= -65.4 && v->GetMatrix()[3].y <= 18.0 && v->GetMatrix()[3].z >= -122) {   //위로 직진
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
			else if (v->GetMatrix()[3].x <= 60.0 && v->GetMatrix()[3].y >= -8.5 && prev_m[3].z <= -124.0) {	//내려가는 부분
				glm::mat4 orbit = glm::mat4(1.0f);
				glm::vec3 loc = glm::vec3(prev_m[3].x, prev_m[3].y + 5.8, prev_m[3].z);  // 위치
				glm::vec3 rot = glm::vec3(0.0, 3.1416, 1.5708);        // 회전 (라디안)
				glm::vec3 _size = glm::vec3(0.0005, 0.0005, 0.0005); // 크기

				// 초기화
				v->modelMatrix = glm::mat4(1.0f);

				// 크기 조정
				glm::mat4 scale = glm::scale(glm::mat4(1.0f), _size);

				// 회전 설정 (XYZ 순서로 회전)
				glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), rot.x, glm::vec3(1.0, 0.0, 0.0)); // X축 회전
				rotation = glm::rotate(rotation, rot.y, glm::vec3(0.0, 1.0, 0.0));                 // Y축 회전
				rotation = glm::rotate(rotation, rot.z, glm::vec3(0.0, 0.0, 1.0));                 // Z축 회전

				// 위치 이동
				glm::mat4 translation = glm::translate(glm::mat4(1.0f), loc);

				// 변환 순서: Translation * Rotation * Scale
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
			else if (prev_m[3].x <= 68.0 && prev_m[3].z <= -124.0) {	//내려간 후 평지
				glm::mat4 orbit = glm::mat4(1.0f);
				glm::vec3 loc = glm::vec3(prev_m[3].x - 5.8, prev_m[3].y, prev_m[3].z);  // 위치
				glm::vec3 rot = glm::vec3(0.0, 3.1416, 0.0);        // 회전 (라디안)
				glm::vec3 _size = glm::vec3(0.0005, 0.0005, 0.0005); // 크기

				// 초기화
				v->modelMatrix = glm::mat4(1.0f);

				// 크기 조정
				glm::mat4 scale = glm::scale(glm::mat4(1.0f), _size);

				// 회전 설정 (XYZ 순서로 회전)
				glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), rot.x, glm::vec3(1.0, 0.0, 0.0)); // X축 회전
				rotation = glm::rotate(rotation, rot.y, glm::vec3(0.0, 1.0, 0.0));                 // Y축 회전
				rotation = glm::rotate(rotation, rot.z, glm::vec3(0.0, 0.0, 1.0));                 // Z축 회전

				// 위치 이동
				glm::mat4 translation = glm::translate(glm::mat4(1.0f), loc);

				// 변환 순서: Translation * Rotation * Scale
				v->modelMatrix = translation * rotation * scale;
			}
			else if (v->GetMatrix()[3].x <= 68.0 && v->GetMatrix()[3].z <= -120.0) {	//
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
				//printMatrix(v->modelMatrix, "v->modelMatrix");
			}
			else if (v->GetMatrix()[3].x >= 70.5 && v->GetMatrix()[3].z >= -114.5) {
				glm::vec3 loc = glm::vec3(70.0, -36.0, -114.0);  // 위치
				glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0);        // 회전 (라디안)
				glm::vec3 _size = glm::vec3(0.0005, 0.0005, 0.0005); // 크기

				// 초기화
				v->modelMatrix = glm::mat4(1.0f);

				// 크기 조정
				glm::mat4 scale = glm::scale(glm::mat4(1.0f), _size);

				// 회전 설정 (XYZ 순서로 회전)
				glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), rot.x, glm::vec3(1.0, 0.0, 0.0)); // X축 회전
				rotation = glm::rotate(rotation, rot.y, glm::vec3(0.0, 1.0, 0.0));                 // Y축 회전
				rotation = glm::rotate(rotation, rot.z, glm::vec3(0.0, 0.0, 1.0));                 // Z축 회전
				//glm::mat4 rotation = glm::mat4(1.0f); // 회전 제거
				// 위치 이동
				glm::mat4 translation = glm::translate(glm::mat4(1.0f), loc);

				// 변환 순서: Translation * Rotation * Scale
				v->modelMatrix = translation * rotation * scale;

				// 디버그 출력
				//printMatrix(v->modelMatrix, "v->modelMatrix");

			}
			v->modelMatrix = orbit * v->modelMatrix;
			prev_b_m = v->modelMatrix;
		}
		else if (v->GetType() == roller_coaster_body3) {
			glm::mat4 orbit = glm::mat4(1.0f);
			if (prev_b_m[3].x <= 65.0 && prev_b_m[3].x > -45.0 && v->GetMatrix()[3].z >= -114.0) {	// 왼쪽이동
				glm::mat4 orbit = glm::mat4(1.0f);
				glm::vec3 loc = glm::vec3(prev_b_m[3].x + 5.8, -36.0, -114.0);  // 위치
				glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0);        // 회전 (라디안)
				glm::vec3 _size = glm::vec3(0.0005, 0.0005, 0.0005); // 크기

				// 초기화
				v->modelMatrix = glm::mat4(1.0f);

				// 크기 조정
				glm::mat4 scale = glm::scale(glm::mat4(1.0f), _size);

				// 회전 설정 (XYZ 순서로 회전)
				glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), rot.x, glm::vec3(1.0, 0.0, 0.0)); // X축 회전
				rotation = glm::rotate(rotation, rot.y, glm::vec3(0.0, 1.0, 0.0));                 // Y축 회전
				rotation = glm::rotate(rotation, rot.z, glm::vec3(0.0, 0.0, 1.0));                 // Z축 회전

				// 위치 이동
				glm::mat4 translation = glm::translate(glm::mat4(1.0f), loc);

				// 변환 순서: Translation * Rotation * Scale
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
				orbit = glm::translate(orbit, glm::vec3(-0.031, 0.075, 0.0));
			}
			else if (v->GetMatrix()[3].x > -65.4 && v->GetMatrix()[3].x <= -63.5 && v->GetMatrix()[3].z >= -122) {
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.185f), glm::vec3(0.0, 0.0, -1.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(-0.0267, 0.11, 0.0));
			}
			else if (prev_b_m[3].x <= 64.5 && prev_b_m[3].z == -114.0) {	//올라가는 부분
				glm::mat4 orbit = glm::mat4(1.0f);
				glm::vec3 loc = glm::vec3(prev_b_m[3].x, prev_b_m[3].y - 5.8, -114.0);  // 위치
				glm::vec3 rot = glm::vec3(0.0, 0.0, -1.5708);        // 회전 (라디안)
				glm::vec3 _size = glm::vec3(0.0005, 0.0005, 0.0005); // 크기

				// 초기화
				v->modelMatrix = glm::mat4(1.0f);

				// 크기 조정
				glm::mat4 scale = glm::scale(glm::mat4(1.0f), _size);

				// 회전 설정 (XYZ 순서로 회전)
				glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), rot.x, glm::vec3(1.0, 0.0, 0.0)); // X축 회전
				rotation = glm::rotate(rotation, rot.y, glm::vec3(0.0, 1.0, 0.0));                 // Y축 회전
				rotation = glm::rotate(rotation, rot.z, glm::vec3(0.0, 0.0, 1.0));                 // Z축 회전

				// 위치 이동
				glm::mat4 translation = glm::translate(glm::mat4(1.0f), loc);

				// 변환 순서: Translation * Rotation * Scale
				v->modelMatrix = translation * rotation * scale;
			}
			else if (v->GetMatrix()[3].x <= -65.4 && v->GetMatrix()[3].y <= 18.0 && v->GetMatrix()[3].z >= -122) {   //위로 직진
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
				orbit = glm::rotate(orbit, glm::radians(0.81f), glm::vec3(-1.0, 0.0, 0.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(0.0, 0.012, -0.051));
			}
			else if (v->GetMatrix()[3].x <= 60.0 && v->GetMatrix()[3].y > 21.0 && v->GetMatrix()[3].z <= -123.0) {
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.45f), glm::vec3(-1.0, 0.0, 0.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(0.0, -0.08, -0.034));
			}
			else if (v->GetMatrix()[3].x <= 60.0 && v->GetMatrix()[3].y >= -8.5 && prev_b_m[3].z <= -124.0) {	//내려가는 부분
				glm::mat4 orbit = glm::mat4(1.0f);
				glm::vec3 loc = glm::vec3(prev_b_m[3].x, prev_b_m[3].y + 5.8, prev_b_m[3].z);  // 위치
				glm::vec3 rot = glm::vec3(0.0, 3.1416, 1.5708);        // 회전 (라디안)
				glm::vec3 _size = glm::vec3(0.0005, 0.0005, 0.0005); // 크기

				// 초기화
				v->modelMatrix = glm::mat4(1.0f);

				// 크기 조정
				glm::mat4 scale = glm::scale(glm::mat4(1.0f), _size);

				// 회전 설정 (XYZ 순서로 회전)
				glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), rot.x, glm::vec3(1.0, 0.0, 0.0)); // X축 회전
				rotation = glm::rotate(rotation, rot.y, glm::vec3(0.0, 1.0, 0.0));                 // Y축 회전
				rotation = glm::rotate(rotation, rot.z, glm::vec3(0.0, 0.0, 1.0));                 // Z축 회전

				// 위치 이동
				glm::mat4 translation = glm::translate(glm::mat4(1.0f), loc);

				// 변환 순서: Translation * Rotation * Scale
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
			else if (v->GetMatrix()[3].x >= -43.0 && v->GetMatrix()[3].x <= -36.0 && v->GetMatrix()[3].z <= -120.0) {	//
				orbit = glm::translate(orbit, glm::vec3(v->modelMatrix[3]));
				orbit = glm::rotate(orbit, glm::radians(0.85f), glm::vec3(0.0, 0.0, 1.0));
				orbit = glm::translate(orbit, glm::vec3(-v->modelMatrix[3]));
				orbit = glm::translate(orbit, glm::vec3(0.56, -0.053, 0.0));
			}
			else if (prev_b_m[3].x <= 68.0 && prev_b_m[3].z <= -124.0) {	//내려간 후 평지
				glm::mat4 orbit = glm::mat4(1.0f);
				glm::vec3 loc = glm::vec3(prev_b_m[3].x - 5.8, prev_b_m[3].y, prev_b_m[3].z);  // 위치
				glm::vec3 rot = glm::vec3(0.0, 3.1416, 0.0);        // 회전 (라디안)
				glm::vec3 _size = glm::vec3(0.0005, 0.0005, 0.0005); // 크기

				// 초기화
				v->modelMatrix = glm::mat4(1.0f);

				// 크기 조정
				glm::mat4 scale = glm::scale(glm::mat4(1.0f), _size);

				// 회전 설정 (XYZ 순서로 회전)
				glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), rot.x, glm::vec3(1.0, 0.0, 0.0)); // X축 회전
				rotation = glm::rotate(rotation, rot.y, glm::vec3(0.0, 1.0, 0.0));                 // Y축 회전
				rotation = glm::rotate(rotation, rot.z, glm::vec3(0.0, 0.0, 1.0));                 // Z축 회전

				// 위치 이동
				glm::mat4 translation = glm::translate(glm::mat4(1.0f), loc);

				// 변환 순서: Translation * Rotation * Scale
				v->modelMatrix = translation * rotation * scale;
			}
			else if (v->GetMatrix()[3].x <= 68.0 && v->GetMatrix()[3].z <= -120.0) {	//
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
				//printMatrix(v->modelMatrix, "v->modelMatrix");
			}
			else if (v->GetMatrix()[3].x >= 70.5 && v->GetMatrix()[3].z >= -114.5) {
				glm::vec3 loc = glm::vec3(70.0, -36.0, -114.0);  // 위치
				glm::vec3 rot = glm::vec3(0.0, 0.0, 0.0);        // 회전 (라디안)
				glm::vec3 _size = glm::vec3(0.0005, 0.0005, 0.0005); // 크기

				// 초기화
				v->modelMatrix = glm::mat4(1.0f);

				// 크기 조정
				glm::mat4 scale = glm::scale(glm::mat4(1.0f), _size);

				// 회전 설정 (XYZ 순서로 회전)
				glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), rot.x, glm::vec3(1.0, 0.0, 0.0)); // X축 회전
				rotation = glm::rotate(rotation, rot.y, glm::vec3(0.0, 1.0, 0.0));                 // Y축 회전
				rotation = glm::rotate(rotation, rot.z, glm::vec3(0.0, 0.0, 1.0));                 // Z축 회전
				//glm::mat4 rotation = glm::mat4(1.0f); // 회전 제거
				// 위치 이동
				glm::mat4 translation = glm::translate(glm::mat4(1.0f), loc);

				// 변환 순서: Translation * Rotation * Scale
				v->modelMatrix = translation * rotation * scale;

				// 디버그 출력
				//printMatrix(v->modelMatrix, "v->modelMatrix");

			}
			v->modelMatrix = orbit * v->modelMatrix;
		}

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

		else if (v->GetType() == balloon || v->GetType() == basket) {
			glm::mat4 orbit = glm::mat4(1.0f);
			orbit = glm::translate(orbit, glm::vec3(-30.0, 35.0, -120.0));
			orbit = glm::rotate(orbit, glm::radians(0.1f), glm::vec3(0.0, 1.0, 0.0));
			orbit = glm::translate(orbit, -glm::vec3(-30.0, 35.0, -120.0));
			v->modelMatrix = orbit * v->modelMatrix;
		}

		if (v->GetType() == viking_ship) {
			// 회전 각도 업데이트
			if (increasing) {
				currentAngle += angleStep;
				if (currentAngle >= maxAngle) {
					currentAngle = maxAngle;  // 최대 각도를 넘어가지 않도록 제한
					increasing = false;  // 방향 전환
				}
			}
			else {
				currentAngle -= angleStep;
				if (currentAngle <= minAngle) {
					currentAngle = minAngle;  // 최소 각도를 넘어가지 않도록 제한
					increasing = true;  // 방향 전환
				}
			}

			// 회전 중심으로 이동 후 회전하고 원위치로 되돌리기
			glm::mat4 orbit = glm::mat4(1.0f);
			orbit = glm::translate(orbit, glm::vec3(ship_pos));  // 회전할 중심으로 이동
			orbit = glm::rotate(orbit, glm::radians(currentAngle), glm::vec3(0.0, 0.0, 1.0));  // 회전
			orbit = glm::translate(orbit, glm::vec3(-ship_pos)); // 원래 위치로 되돌리기

			// 초기 모델 행렬을 기준으로 회전 적용
			v->modelMatrix = orbit * v->initialModelMatrix;
			if (Viking) {
				//camera->cam_Look_Matrix = orbit * camera->Initial_cam_Look_Matrix;
				camera->camMatrix = orbit * camera->Initial_camMatrix;
			}
		}


		if (v->GetType() == cloud) {
			glm::mat4 orbit = glm::mat4(1.0f);
			orbit = glm::translate(orbit, glm::vec3(0.0, 0.0, -60.0));
			orbit = glm::rotate(orbit, glm::radians(v->speed), glm::vec3(0.0, 1.0, 0.0));
			orbit = glm::translate(orbit, -glm::vec3(0.0, 0.0, -60.0));
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
	if (ThreeMode && !OneMode)
	{
		glm::mat4 orbit = glm::mat4(1.0f);
		orbit = glm::translate(orbit, glm::vec3(0.0, 0.0, -60.0));
		orbit = glm::rotate(orbit, glm::radians(1.0f), glm::vec3(0.0, 1.0, 0.0));
		orbit = glm::translate(orbit, -glm::vec3(0.0, 0.0, -60.0));
		camera->camMatrix = orbit * camera->camMatrix;
		camera->cam_Look_Matrix = orbit * camera->cam_Look_Matrix;
	}

}