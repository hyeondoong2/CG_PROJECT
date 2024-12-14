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
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(shaderProgramID, "textureSampler"), 0);

	// 카메라 및 조명 설정
	camera->DoWorking(shaderProgramID);
	m_light->LightWorks(shaderProgramID);

	for (auto& v : m_ObjectMgr->GetAllObjs()) {
		if (v->GetType() == cloud) continue; // 구름은 건너뜀
		// 모델 변환 행렬 전달
		glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "transform"), 1, GL_FALSE, glm::value_ptr(v->modelMatrix));

		// 색상 유니폼 전달
		glm::vec3 color = v->GetColor();
		glUniform3f(glGetUniformLocation(shaderProgramID, "incolor"), color.r, color.g, color.b);
		glBindTexture(GL_TEXTURE_2D, v->text);

		if (v->text != 0) {
			glUniform1i(glGetUniformLocation(shaderProgramID, "useTexture"), GL_TRUE);
			glBindVertexArray(v->GetMesh()->VAO);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDrawArrays(GL_TRIANGLES, 0, v->GetMesh()->polygon_count * 3);
		}

	/*	if (v->GetType() == kitty) {
			glUniform1i(glGetUniformLocation(shaderProgramID, "useTexture"), GL_TRUE);
			glBindVertexArray(v->GetMesh()->VAO);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDrawArrays(GL_TRIANGLES, 0, v->GetMesh()->polygon_count * 3);
		}*/
		else {
			// VAO 바인딩
			//glBindTexture(GL_TEXTURE_2D, v->text);
			glUniform1i(glGetUniformLocation(shaderProgramID, "useTexture"), GL_FALSE);
			glBindVertexArray(v->GetMesh()->VAO);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDrawArrays(GL_TRIANGLES, 0, v->GetMesh()->polygon_count * 3);
		}

		glBindVertexArray(0); // VAO 바인딩 해제
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
		glUniform1i(glGetUniformLocation(shaderProgramID, "useTexture"), GL_FALSE);
		glUniform3f(glGetUniformLocation(shaderProgramID, "incolor"), color.r, color.g, color.b);
		glUniform1f(glGetUniformLocation(shaderProgramID, "Alpha"), 0.5f); // 투명도 설정

		glBindVertexArray(v->GetMesh()->VAO);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // 사각형 매쉬의 경계도 제대로 처리되도록 설정
		// 사각형을 두 개의 삼각형으로 나누기 (예시: 두 삼각형을 그린다고 가정)
		glDrawArrays(GL_TRIANGLES, 0, v->GetMesh()->polygon_count * 3);
		glBindVertexArray(0);;
	}

	glDepthMask(GL_TRUE); // 깊이 버퍼 쓰기 다시 활성화
	glDisable(GL_BLEND); // 블렌딩 비활성화
}

void Renderer::Render_Enviroment(GLuint Shader)
{
	glDepthMask(GL_FALSE);

	glUseProgram(Shader);

	camera->DoWorking(Shader);
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_CUBE_MAP, importer->GetEnviromentMaterial());
	GLuint ul_enviroment = glGetUniformLocation(Shader, "u_enviroment");
	glUniform1i(ul_enviroment, 0);

	glBindVertexArray(EviromentVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0);

	glDepthMask(GL_TRUE);
}

void Renderer::Initialize_EviromentVAO()
{
	float skyboxSize = 100.f;
	float skyboxVertices[] = {
		// positions          
		-skyboxSize,  skyboxSize, -skyboxSize,
		-skyboxSize, -skyboxSize, -skyboxSize,
		 skyboxSize, -skyboxSize, -skyboxSize,
		 skyboxSize, -skyboxSize, -skyboxSize,
		 skyboxSize,  skyboxSize, -skyboxSize,
		-skyboxSize,  skyboxSize, -skyboxSize,

		-skyboxSize, -skyboxSize,  skyboxSize,
		-skyboxSize, -skyboxSize, -skyboxSize,
		-skyboxSize,  skyboxSize, -skyboxSize,
		-skyboxSize,  skyboxSize, -skyboxSize,
		-skyboxSize,  skyboxSize,  skyboxSize,
		-skyboxSize, -skyboxSize,  skyboxSize,

		 skyboxSize, -skyboxSize, -skyboxSize,
		 skyboxSize, -skyboxSize,  skyboxSize,
		 skyboxSize,  skyboxSize,  skyboxSize,
		 skyboxSize,  skyboxSize,  skyboxSize,
		 skyboxSize,  skyboxSize, -skyboxSize,
		 skyboxSize, -skyboxSize, -skyboxSize,

		-skyboxSize, -skyboxSize,  skyboxSize,
		-skyboxSize,  skyboxSize,  skyboxSize,
		 skyboxSize,  skyboxSize,  skyboxSize,
		 skyboxSize,  skyboxSize,  skyboxSize,
		 skyboxSize, -skyboxSize,  skyboxSize,
		-skyboxSize, -skyboxSize,  skyboxSize,

		-skyboxSize,  skyboxSize, -skyboxSize,
		 skyboxSize,  skyboxSize, -skyboxSize,
		 skyboxSize,  skyboxSize,  skyboxSize,
		 skyboxSize,  skyboxSize,  skyboxSize,
		-skyboxSize,  skyboxSize,  skyboxSize,
		-skyboxSize,  skyboxSize, -skyboxSize,

		-skyboxSize, -skyboxSize, -skyboxSize,
		-skyboxSize, -skyboxSize,  skyboxSize,
		 skyboxSize, -skyboxSize, -skyboxSize,
		 skyboxSize, -skyboxSize, -skyboxSize,
		-skyboxSize, -skyboxSize,  skyboxSize,
		 skyboxSize, -skyboxSize,  skyboxSize
	};

	glGenVertexArrays(1, &EviromentVAO);
	glGenBuffers(1, &EviromentVBO);

	glBindVertexArray(EviromentVAO);

	glBindBuffer(GL_ARRAY_BUFFER, EviromentVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

	glBindVertexArray(0);
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
	Initialize_EviromentVAO();
}
