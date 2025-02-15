#include "ISaveable.h"
#include "SLSystem.h"

using namespace NCL::CSC8503;
using namespace NCL;

void ISaveable::RegisterSL()
{
	SLSystem::GetInstance().RegisterISaveable(this);
}

void ISaveable::UnRegisterSL()
{
	SLSystem::GetInstance().UnRegisterISaveable(this);
}
