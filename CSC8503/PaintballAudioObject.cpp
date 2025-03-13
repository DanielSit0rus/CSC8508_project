#include "PaintballAudioObject.h"
#include "EventManager.h"

using namespace NCL::CSC8503;

PaintballAudioObject::PaintballAudioObject(PaintballTransform* parentTransform, EventInstance* instance, bool isAuto)
	: event(instance)
{
	transform = parentTransform;
	sourceAttributes = new FMOD_3D_ATTRIBUTES;

	if (isAuto) {
		EventManager::Subscribe(EventType::Game_Start, [this]() {Play(true); });
		EventManager::Subscribe(EventType::Game_End, [this]() {Play(false); });
		EventManager::Subscribe(EventType::Game_Resume, [this]() {Play(true); });
		EventManager::Subscribe(EventType::Game_Pause, [this]() {Play(false); });
	}
}

PaintballAudioObject::~PaintballAudioObject()
{
	event->stop(FMOD_STUDIO_STOP_IMMEDIATE);
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

void PaintballAudioObject::Play(bool isPlay) {
	//std::cout << "PaintballAudioObject::Play = " << isPlay << std::endl;
	if (isPlay) {
		FMOD_STUDIO_PLAYBACK_STATE state;
		event->getPlaybackState(&state);

		if (state == FMOD_STUDIO_PLAYBACK_STOPPED) {
			event->start();
		}
		else {
			event->setPaused(false);
		}
	}
	else {
		event->setPaused(true);
	}
}
