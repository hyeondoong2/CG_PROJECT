#include "ObjectMgr.h"

ObjectMgr::ObjectMgr(Importer_obj* importer)
{
	m_importer = importer;
}

ObjectMgr::~ObjectMgr()
{

}

void ObjectMgr::AddObject(int type, glm::vec3 loc, glm::vec3 rot, glm::vec3 _size, glm::vec3 _color)
{
	++ObjectNum;
	Object* newObj = new Object(type, loc, rot, _size, _color, ObjectNum, m_importer);
	objs.push_back(newObj);
}

std::vector<Object*> ObjectMgr::GetAllObjs()
{
	return objs;
}

std::vector<Object*> ObjectMgr::GetXYZ()
{
	return xyz;
}
