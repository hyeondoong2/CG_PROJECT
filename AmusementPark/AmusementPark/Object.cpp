#include "Object.h"
#include <random>

std::random_device _rd;
std::default_random_engine _dre(_rd());
std::uniform_real_distribution<float> face{0.01, 0.05 };
std::uniform_int_distribution<int> newType{ 0,4 };

Object::Object(int _type, glm::vec3 loc, glm::vec3 rot, glm::vec3 _size, glm::vec3 _color, int index, Importer_obj* importer)
{
	Importer_mesh = importer;
	type = _type;
	location = loc;
	Originlocation = loc;
	Originrotation = rot;
	colors = _color;
	rotation = rot;
	size = _size;

	modelMatrix = glm::translate(modelMatrix, loc);
	modelMatrix = glm::scale(modelMatrix, _size);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(rot.x), glm::vec3(1.0, 0.0, 0.0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(rot.y), glm::vec3(0.0, 1.0, 0.0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(rot.z), glm::vec3(0.0, 0.0, 1.0));

	initialModelMatrix = glm::translate(initialModelMatrix, loc);
	initialModelMatrix = glm::scale(initialModelMatrix, _size);
	initialModelMatrix = glm::rotate(initialModelMatrix, glm::radians(rot.x), glm::vec3(1.0, 0.0, 0.0));
	initialModelMatrix = glm::rotate(initialModelMatrix, glm::radians(rot.y), glm::vec3(0.0, 1.0, 0.0));
	initialModelMatrix = glm::rotate(initialModelMatrix, glm::radians(rot.z), glm::vec3(0.0, 0.0, 1.0));

	object_index = index;

	std::cout << "new obj " << type << '\n';

	if (type == viking_body) {
		SetMesh("viking_body.obj");
		SetMaterial("viking_body.mtl");
	}
	else if (type == viking_ship) {
		SetMesh("viking_ship.obj");
		SetMaterial("viking_ship.mtl");
	}
	else if (type == wheel_car) {
		SetMesh("wheel_car.obj");
		SetMaterial("wheel_car.mtl");
	}
	else if (type == wheel_body) {
		SetMesh("wheel_body.obj");
		SetMaterial("wheel_body.mtl");
	}	
	else if (type == merry_go_round_body) {
		SetMesh("merry_go_round_body.obj");
		SetMaterial("merry_go_round_body.mtl");
	}
	else if (type == merry_go_round_horse) {
		SetMesh("merry_go_round_horse.obj");
		SetMaterial("viking_body.mtl");
	}
	else if (type == base) {
		SetMesh("Base.obj");
		SetText("textures/texture5.png");
	}
	else if (type == chair) {
		SetMesh("chair.obj");
	}
	else if (type == roller_coaster_rail) {
		SetMesh("roller_coaster_rail.obj");
	}
	else if (type == roller_coaster_body1 || type == roller_coaster_body2 || type == roller_coaster_body3) {
		SetMesh("roller_coaster_body.obj");
	}
	else if (type == roller_coaster_head) {
		SetMesh("roller_coaster_head.obj");
	}
	else if (type == tree) {
		SetMesh("tree.obj");
		SetMaterial("tree.mtl");
	}
	else if (type == fence) {
		SetMesh("fence.obj");
	}
	
	else if (type == tree_leaf) {
		SetMesh("tree_leaf.obj");
	}
	else if (type == tree_wood) {
		SetMesh("tree_wood.obj");
	}
	else if (type == door1 || type == door2) {
		SetMesh("door.obj");
		SetMaterial("door.mtl");
	}
	else if (type == cloud) {
		SetMesh("tree_leaf.obj");
		speed = face(_dre);
	}
	else if (type == entrance) {
		SetMesh("entrance.obj");
	}
	else if (type == cube) {
		SetMesh("cube.obj");
	}
	else if (type == kitty) {
		SetMesh("Kitty.obj");
		SetText("textures/HelloKitty.png");
	}

}

Object::~Object()
{
}

void Object::SetMesh(std::string filename)
{
	mesh = Importer_mesh->FindMesh(filename);
	std::cout << "find!" << '\n';
}


VertexData* Object::GetMesh()
{
	return mesh;
}

Material* Object::GetMaterial()
{
	return m_material;
}

void Object::SetMaterial(std::string filename)
{
	m_material = Importer_mesh->FindMaterial(filename);
	std::cout << "find!" << '\n';
}

void Object::SetText(std::string filename)
{
	text = Importer_mesh->LoadTexture(filename);
}

glm::vec3 Object::GetLocation()
{
	return location;
}

glm::vec3 Object::GetRotation()
{
	return rotation;
}

glm::vec3 Object::GetColor()
{
	return colors;
}

glm::vec3 Object::GetSize()
{
	return size;
}

int Object::GetType()
{
	return type;
}

glm::mat4 Object::GetMatrix()
{
	return modelMatrix;
}

void Object::AddMovementInput(glm::vec3 velocity)
{
	location.x += velocity.x;
	location.y += velocity.y;
	location.z += velocity.z;
}

void Object::AddRotationInput(glm::vec3 velocity)
{
	rotation.x += velocity.x;
	rotation.y += velocity.y;
	rotation.z += velocity.z;
}

void Object::SetNewPosition(glm::vec3 newLocation)
{
	location = newLocation;
}


void Object::SetNewRotation(glm::vec3 velocity)
{
	rotation = velocity;
}


void Object::ChangeSize(float _size)
{
	size.x += _size;
	size.y += _size;
	size.z += _size;
}
