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

	// 카메라 및 조명 설정
	camera->DoWorking(shaderProgramID);
	m_light->LightWorks(shaderProgramID);

	// 1. 불투명 객체 렌더링
	for (auto& v : m_ObjectMgr->GetAllObjs()) {
		if (v->GetType() == cloud) continue; // 구름은 건너뜀

		glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "transform"), 1, GL_FALSE, glm::value_ptr(v->modelMatrix));

		glm::vec3 color = v->GetColor();
		// 알파값은 불투명 객체라서 1.0
		glUniform3f(glGetUniformLocation(shaderProgramID, "incolor"), color.r, color.g, color.b);
		glUniform1f(glGetUniformLocation(shaderProgramID, "Alpha"), 1.0f); // 알파값 전달

		glBindVertexArray(v->GetMesh()->VAO);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawArrays(GL_TRIANGLES, 0, v->GetMesh()->polygon_count * 3);
		glBindVertexArray(0);
	}

	// 2. 투명 객체(구름) 렌더링
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE); // 깊이 버퍼 쓰기 비활성화

	for (auto& v : m_ObjectMgr->GetAllObjs()) {
		if (v->GetType() != cloud) continue; // 구름만 처리

		glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "transform"), 1, GL_FALSE, glm::value_ptr(v->modelMatrix));

		glm::vec3 color = v->GetColor();
		// 구름의 알파값 설정
		glUniform3f(glGetUniformLocation(shaderProgramID, "incolor"), color.r, color.g, color.b);
		glUniform1f(glGetUniformLocation(shaderProgramID, "Alpha"), 0.3f); // 투명도 설정

		glBindVertexArray(v->GetMesh()->VAO);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawArrays(GL_TRIANGLES, 0, v->GetMesh()->polygon_count * 3);
		glBindVertexArray(0);
	}

	glDepthMask(GL_TRUE); // 깊이 버퍼 쓰기 다시 활성화
	glDisable(GL_BLEND); // 블렌딩 비활성화
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
	vertexSource = filetobuf("vertex.glsl");	// 버텍스 세이더 읽어오기

	vertexShader = glCreateShader(GL_VERTEX_SHADER);	// 버텍스 세이더 객체 만들기

	glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);	// 세이더 코드를 세이더 객체에 넣기
	glCompileShader(vertexShader);	// 버텍스 세이더 컴파일

	// 컴파일이 제대로 되지 않은 경우 : 에러체크
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}


void Renderer::make_fragmentShaders()
{
	fragmentSource = filetobuf("fragment.glsl");	// 프래그먼트 세이더 읽어오기

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);	// 프래그먼트 세이더 객체 만들기

	glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);	// 세이더 코드를 세이더 객체에 넣기
	glCompileShader(fragmentShader);	// 프래그먼트 세이더 컴파일

	// 컴파일이 제대로 되지 않은 경우 : 컴파일 에러 체크
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: fragment shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}

void Renderer::make_shaderProgram()
{
	// 1. 일반 객체용 셰이더 프로그램
	make_vertexShaders();       // 일반 객체용 버텍스 셰이더 생성
	make_fragmentShaders();     // 프래그먼트 셰이더 생성

	shaderProgramID = glCreateProgram();  // 일반 객체용 셰이더 프로그램 생성

	glAttachShader(shaderProgramID, vertexShader);    // 객체용 버텍스 셰이더 붙이기
	glAttachShader(shaderProgramID, fragmentShader);  // 프래그먼트 셰이더 붙이기

	glLinkProgram(shaderProgramID);  // 객체용 셰이더 프로그램 링크

	// 셰이더 객체 삭제 (셰이더 프로그램에 링크되었으므로 필요 없음)
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glUseProgram(shaderProgramID);
}

void Renderer::Initialize()
{
	make_shaderProgram();
}
