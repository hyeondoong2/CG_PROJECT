#pragma once
#include <iostream>
#include <vector>
#include "Object.h"

class ObjectMgr
{
private:
	std::vector<Object*> objs;
	std::vector<Object*> xyz;
	Importer_obj* m_importer;
	int ObjectNum;

public:
	ObjectMgr(Importer_obj* importer);
	~ObjectMgr();
	void AddObject(int type, glm::vec3 loc, glm::vec3 rot, glm::vec3 _size, glm::vec3 _color);

	std::vector<Object*>GetAllObjs();
	std::vector<Object*>GetXYZ();
};

