#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include "OpenGL.h"

#include "LoadPng.h"

struct Material {
	std::string filename;

	glm::vec3 BaseColor;         // Diffuse 색상 (Kd)
	glm::vec3 Normal;            // Ambient 색상 (Ka)
	glm::vec3 Emissive;          // Specular 색상 (Ks)
	std::string texturePath;     // Diffuse 텍스처 경로 (map_Kd)

	GLuint BaseColorID;         // Diffuse 텍스처 ID
	GLuint NormalMapID;         // Normal 맵 텍스처 ID
	GLuint EmissiveID;          // Emissive 텍스처 ID
};

struct VertexData {
	std::string filename;

	unsigned int polygon_count{ 0 };

	GLuint VAO;
	GLuint VBO;
	GLuint texCoordVBO;
	GLuint colorVBO;
	GLuint normalVBO;
	GLuint tangentVBO;
	GLuint bitangentVBO;

	std::vector<glm::vec3> vertexs;
	std::vector<glm::vec2> texCoords;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> colors;

	std::vector<glm::vec3> faceIndices;
	std::vector<glm::vec3> texCoordIndices;
	std::vector<glm::vec3> normalIndices;
	std::vector<glm::vec3> colorIndices;
};

class Importer_obj
{
	std::vector<Material*> Materials;
	std::vector<VertexData*> VertexBuffers;

	void Initialize();

	void CalculateTangentBitangent(
		const glm::vec3& pos1, const glm::vec3& pos2, const glm::vec3& pos3,
		const glm::vec2& uv1, const glm::vec2& uv2, const glm::vec2& uv3,
		glm::vec3& tangent, glm::vec3& bitangent);
	void DeBugVertexData(VertexData* VD);
	std::string removeSubstring(const std::string& str, const std::string& toRemove);

public:
	static GLuint LoadTexture(const std::string filePath);
	void LoadMTL(const std::string filePath);
	Material* SetMaterial(std::string filename);
	void ReadObj(const std::string filePath);
	VertexData* FindMesh(std::string filename);
	void setupMesh(VertexData* VD);

	Importer_obj();
	~Importer_obj();
};

