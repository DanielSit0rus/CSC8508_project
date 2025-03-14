#include "Util.h"
#include "ResourceManager.h"

using namespace NCL::CSC8503;

int Util::GetResourceIdByString(const std::string& type, const std::string& target)
{
	return ResourceManager::GetInstance().GetResourceIdByString(type, target);
}

std::string Util::GetResourceByID(const std::string& type, const int& id)
{
	return ResourceManager::GetInstance().GetResourceByID(type, id);
}
