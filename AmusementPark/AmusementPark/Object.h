#pragma once
#include "OpenGL.h"
#include "Importer.h"

enum Shape {
	wheel_body, wheel_car, viking_body, viking_ship, 
	merry_go_round_body, merry_go_round_horse, 
	base, chair
};


class Object
{
private:
	VertexData* mesh;
	Material* m_material;

	glm::vec3  Originrotation;

	glm::vec3 location;
	glm::vec3 rotation;
	glm::vec3 revolution;
	glm::vec3 colors;
	glm::vec3 size;
	GLuint vao;

	Importer_obj* Importer_mesh;
	int type;


public:
	Object(int _type, glm::vec3 loc, glm::vec3 rot, glm::vec3 _size, glm::vec3 _color, int index, Importer_obj* importer);
	~Object();

	glm::vec3 Originlocation;
	int object_index;

	float angle = 0.0f;

	glm::mat4 modelMatrix = glm::mat4(1.0f);
	glm::mat4 translationMatrix = glm::mat4(1.0f);
	glm::mat4 scaleMatrix = glm::mat4(1.0f);
	glm::mat4 rotationMatrix = glm::mat4(1.0f);

	VertexData* GetMesh();
	void SetMesh(std::string filename);
	Material* GetMaterial();
	void SetMaterial(std::string filename);

	glm::vec3 GetLocation();
	glm::vec3 GetRotation();
	glm::vec3 GetColor();
	glm::vec3 GetSize();
	glm::mat4 GetMatrix();
	int GetType();

	void AddMovementInput(glm::vec3 velocity);
	void AddRotationInput(glm::vec3 velocity);
	void SetNewPosition(glm::vec3 velocity);
	void SetNewRotation(glm::vec3 velocity);

	void ChangeSize(float _size);
};

