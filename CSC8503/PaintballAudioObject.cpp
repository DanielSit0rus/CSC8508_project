#include "PaintballAudioObject.h"
using namespace NCL::CSC8503;

PaintballAudioObject::PaintballAudioObject(PaintballTransform* parentTransform, EventInstance* instance)
	: event(instance)
{
	transform = parentTransform;
	sourceAttributes = new FMOD_3D_ATTRIBUTES;
}

PaintballAudioObject::~PaintballAudioObject()
{
	event->release();
	delete sourceAttributes;
}

void PaintballAudioObject::Update()
{
	event->get3DAttributes(sourceAttributes);
	rp3d::Vector3 pos = transform->GetPosition();
	sourceAttributes->position = { pos.x, pos.y, pos.z };
	event->set3DAttributes(sourceAttributes);
}

