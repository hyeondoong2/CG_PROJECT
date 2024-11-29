#include "Importer.h"
#include <random>

Importer_obj::Importer_obj()
{
	Initialize();
}

Importer_obj::~Importer_obj()
{
}

void Importer_obj::Initialize()
{
	ReadObj("objs/wheel_body.obj");
	LoadMTL("objs/wheel_body.mtl");

	ReadObj("objs/wheel_car.obj");
	LoadMTL("objs/wheel_car.mtl");

	ReadObj("objs/viking_ship.obj");
	LoadMTL("objs/viking_ship.mtl");

	ReadObj("objs/viking_body.obj");
	LoadMTL("objs/viking_body.mtl");

	ReadObj("objs/merry_go_round_body.obj");
	LoadMTL("objs/merry_go_round_body.mtl");

	ReadObj("objs/merry_go_round_horse.obj");
	LoadMTL("objs/merry_go_round_horse.mtl");

	ReadObj("objs/chair.obj");
	LoadMTL("objs/chair.mtl");

	ReadObj("objs/Base.obj");


}

void Importer_obj::CalculateTangentBitangent(const glm::vec3& pos1, const glm::vec3& pos2, const glm::vec3& pos3, const glm::vec2& uv1, const glm::vec2& uv2, const glm::vec2& uv3, glm::vec3& tangent, glm::vec3& bitangent)
{
	glm::vec3 edge1 = pos2 - pos1;
	glm::vec3 edge2 = pos3 - pos1;
	glm::vec2 deltaUV1 = uv2 - uv1;
	glm::vec2 deltaUV2 = uv3 - uv1;

	float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	tangent = glm::normalize(tangent);

	bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
	bitangent = glm::normalize(bitangent);
}

void Importer_obj::DeBugVertexData(VertexData* VD)
{
	int count{ 0 };
	for (auto& v : VD->vertexs) {
		std::cout << v.x << " " << v.y << " " << v.z << std::endl;

	}

	std::cout << std::endl;

	for (auto& v : VD->normals) {
		std::cout << v.x << " " << v.y << " " << v.z << std::endl;
		count++;
	}
	std::cout << std::endl;

	for (auto& v : VD->texCoords) {
		std::cout << v.x << " " << v.y << std::endl;
	}

	for (auto& v : VD->normalIndices) {
		std::cout << v.x << " " << v.y << " " << v.z << std::endl;
	}

	std::cout << count << std::endl;
}

std::string Importer_obj::removeSubstring(const std::string& str, const std::string& toRemove)
{
	std::size_t pos = str.find(toRemove);

	if (pos != std::string::npos) {
		return str.substr(pos + toRemove.length());
	}

	return str;
}

void Importer_obj::LoadMTL(const std::string filePath)
{
	std::ifstream mtlFile(filePath);

	if (!mtlFile.is_open()) {
		std::cerr << "Error: Unable to open MTL file " << filePath << std::endl;
		return;
	}
	else {
		std::cout << "File Load Success: " << filePath << std::endl;
	}

	Material* newMaterial = new Material;
	newMaterial->filename = removeSubstring(filePath, "objs/");

	std::cout << newMaterial->filename << '\n';

	std::string line;

	// 1. MTL 파일 읽기 시작
	while (std::getline(mtlFile, line)) {
		if (line.empty()) continue; // 빈 줄 무시

		std::stringstream ss(line);
		std::string type;
		ss >> type;

		if (type == "newmtl") {
			// 새로운 머티리얼 시작
			if (newMaterial != nullptr) {
				Materials.push_back(newMaterial); // 이전 머티리얼 저장
			}

			newMaterial = new Material;
			ss >> newMaterial->filename;
			//std::cout << "Material Created: " << newMaterial->filename << std::endl;
		}
		else if (type == "Ka") {
			// Ambient Color (주변광)
			if (newMaterial) {
				glm::vec3 ambient;
				ss >> ambient.x >> ambient.y >> ambient.z;
				newMaterial->AmbientColor = ambient;
				std::cout << "Ka: " << ambient.x << ", " << ambient.y << ", " << ambient.z << std::endl;
			}
		}
		else if (type == "Kd") {
			// Diffuse Color (난반사광)
			if (newMaterial) {
				glm::vec3 diffuse;
				ss >> diffuse.x >> diffuse.y >> diffuse.z;
				newMaterial->BaseColor = diffuse;
				std::cout << "Kd: " << diffuse.x << ", " << diffuse.y << ", " << diffuse.z << std::endl;
			}
		}
		else if (type == "Ks") {
			// Specular Color (반사광)
			if (newMaterial) {
				glm::vec3 specular;
				ss >> specular.x >> specular.y >> specular.z;
				newMaterial->Emissive = specular;
				std::cout << "Ks: " << specular.x << ", " << specular.y << ", " << specular.z << std::endl;
			}
		}
	}
	Materials.push_back(newMaterial);

	mtlFile.close();

	std::cout << "MTL File Load Complete." << std::endl;
}

GLuint Importer_obj::LoadTexture(const std::string filePath)
{
	//Load Png
	GLuint texture;

	std::vector<unsigned char> image;

	unsigned width, height;

	unsigned error = lodepng::decode(image, width, height, filePath);

	if (error != 0)
	{
		std::cout << "PNG image loading failed:" << filePath << std::endl;
	}

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, &image[0]);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	std::cout << "PNG image loading Success:" << filePath << std::endl;

	return texture;
}


Material* Importer_obj::FindMaterial(std::string filename)
{
	std::cout << "find " << filename << '\n';

	for (std::vector<Material*>::iterator itr = Materials.begin(); itr != Materials.end(); ++itr)
	{
		//std::cout << (*itr)->filename << '\n';
		//DeBugVertexData((*itr));
		if ((*itr)->filename == filename) {
			return (*itr);
		}
	}

	std::cout << "Can not find" << filename << " Material Asset." << std::endl;
	return nullptr;
}

void Importer_obj::ReadObj(const std::string filePath) {
	std::ifstream objFile(filePath);

	if (!objFile.is_open()) {
		std::cerr << "Error: Unable to open file " << filePath << std::endl;
		return;
	}
	else {
		std::cout << "File Load Success: " << filePath << std::endl;
	}


	VertexData* newVertexData = new VertexData;
	newVertexData->filename = removeSubstring(filePath, "objs/");

	std::cout << newVertexData->filename << '\n';

	std::string line;


	// 1. 파일에서 버텍스, 텍스처 좌표, 노멀 및 인덱스 데이터 읽기
	while (std::getline(objFile, line)) {
		if (line.empty()) continue;  // 빈 줄 무시

		std::stringstream ss(line);
		std::string type;
		ss >> type;

		if (type == "v") {
			// 버텍스 좌표 읽기
			glm::vec3 vertex;
			ss >> vertex.x >> vertex.y >> vertex.z;
			newVertexData->vertexs.push_back(vertex);

		}
		else if (type == "vt") {
			// 텍스처 좌표 읽기
			glm::vec2 texCoord;
			ss >> texCoord.x >> texCoord.y;
			newVertexData->texCoords.push_back(texCoord);

		}
		else if (type == "vn") {
			// 노멀 좌표 읽기
			glm::vec3 normal;
			ss >> normal.x >> normal.y >> normal.z;
			newVertexData->normals.push_back(normal);

		}
		else if (type == "f") {
			int vertexIndex[3], texIndex[3], normalIndex[3];
			char slash;

			// 각 삼각형의 정점과 노멀 인덱스 읽기
			ss >> vertexIndex[0] >> slash >> texIndex[0] >> slash >> normalIndex[0] >>
				vertexIndex[1] >> slash >> texIndex[1] >> slash >> normalIndex[1] >>
				vertexIndex[2] >> slash >> texIndex[2] >> slash >> normalIndex[2];

			// 각 인덱스를 VertexData에 저장
			newVertexData->faceIndices.push_back(glm::vec3(vertexIndex[0], vertexIndex[1], vertexIndex[2]));
			newVertexData->texCoordIndices.push_back(glm::vec3(texIndex[0], texIndex[1], texIndex[2]));
			newVertexData->normalIndices.push_back(glm::vec3(normalIndex[0], normalIndex[1], normalIndex[2]));

			newVertexData->polygon_count++;
		}
	}

	//DeBugVertexData(newVertexData);

	// 메쉬 설정 (버텍스, 텍스처, 노멀 좌표 및 인덱스 데이터로)
	setupMesh(newVertexData);

	// VertexBuffer 리스트에 추가
	VertexBuffers.push_back(newVertexData);

	std::cout << "debug" << '\n';
}

VertexData* Importer_obj::FindMesh(std::string filename)
{

	std::cout << "find " << filename << '\n';

	for (std::vector<VertexData*>::iterator itr = VertexBuffers.begin(); itr != VertexBuffers.end(); ++itr)
	{
		//std::cout << (*itr)->filename << '\n';
		//DeBugVertexData((*itr));
		if ((*itr)->filename == filename) {
			return (*itr);
		}
	}

	std::cout << "Can not find " << filename << " Mesh Asset." << std::endl;
	return nullptr;
}

void Importer_obj::setupMesh(VertexData* VD) {
	glGenVertexArrays(1, &VD->VAO);
	glGenBuffers(1, &VD->VBO);
	glGenBuffers(1, &VD->normalVBO);
	glGenBuffers(1, &VD->colorVBO);

	glBindVertexArray(VD->VAO);

	std::vector<glm::vec3> re_vertexs;
	std::vector<glm::vec2> re_texcoords;
	std::vector<glm::vec3> re_normals;

	for (size_t i = 0; i < VD->faceIndices.size(); ++i) {
		// 각 삼각형의 정점 좌표와 텍스처 좌표 가져오기
		glm::vec3 pos1 = VD->vertexs[VD->faceIndices[i].x - 1];
		glm::vec3 pos2 = VD->vertexs[VD->faceIndices[i].y - 1];
		glm::vec3 pos3 = VD->vertexs[VD->faceIndices[i].z - 1];

		glm::vec2 uv1 = VD->texCoords[VD->texCoordIndices[i].x - 1];
		glm::vec2 uv2 = VD->texCoords[VD->texCoordIndices[i].y - 1];
		glm::vec2 uv3 = VD->texCoords[VD->texCoordIndices[i].z - 1];

		glm::vec3 normal1 = VD->normals[VD->normalIndices[i].x - 1];
		glm::vec3 normal2 = VD->normals[VD->normalIndices[i].y - 1];
		glm::vec3 normal3 = VD->normals[VD->normalIndices[i].z - 1];

		re_vertexs.push_back(pos1);
		re_vertexs.push_back(pos2);
		re_vertexs.push_back(pos3);

		re_texcoords.push_back(uv1);
		re_texcoords.push_back(uv2);
		re_texcoords.push_back(uv3);

		re_normals.push_back(normal1);
		re_normals.push_back(normal2);
		re_normals.push_back(normal3);
	}

	// VBO에 버텍스 데이터 바인딩
	glBindBuffer(GL_ARRAY_BUFFER, VD->VBO);
	glBufferData(GL_ARRAY_BUFFER, re_vertexs.size() * sizeof(glm::vec3), &re_vertexs[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

	// VBO에 텍스처 좌표 데이터 바인딩
	glBindBuffer(GL_ARRAY_BUFFER, VD->texCoordVBO);
	glBufferData(GL_ARRAY_BUFFER, re_texcoords.size() * sizeof(glm::vec2), &re_texcoords[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);

	// VBO에 노멀 데이터 바인딩
	glBindBuffer(GL_ARRAY_BUFFER, VD->normalVBO);
	glBufferData(GL_ARRAY_BUFFER, re_normals.size() * sizeof(glm::vec3), &re_normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

	glBindVertexArray(0);
}
