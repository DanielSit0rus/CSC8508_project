#include "PaintballAudioObject.h"
#include "EventManager.h"
#include "AudioSystem.h"

using namespace NCL::CSC8503;

PaintballAudioObject::PaintballAudioObject(PaintballTransform* parentTransform)
{
	transform = parentTransform;
	sourceAttributes = new FMOD_3D_ATTRIBUTES;
}

PaintballAudioObject::~PaintballAudioObject()
{
	for (auto& [_, event] : events) {
		event->release();
	}

	events.clear();

	delete sourceAttributes;
}

void PaintballAudioObject::Update()
{
	for (auto it = events.begin(); it != events.end(); ++it) {
		events[it->first]->set3DAttributes(sourceAttributes);
		rp3d::Vector3 pos = transform->GetPosition();
		sourceAttributes->position = { pos.x, pos.y, pos.z };
		events[it->first]->set3DAttributes(sourceAttributes);
	}
}

void PaintballAudioObject::AddEvent(const std::string& eventName)
{
	EventInstance* eventInstance = AudioSystem::GetInstance().GetEvent(eventName);
	if (eventInstance) {
		events[eventName] = eventInstance;
	}
}

void PaintballAudioObject::PlayEvent(const std::string& eventName, float p) {
	auto it = events.find(eventName);
	if (it != events.end()) {
		FMOD_STUDIO_PLAYBACK_STATE state;
		it->second->getPlaybackState(&state);
		if (state == FMOD_STUDIO_PLAYBACK_PLAYING) {
			return;
		}
		if (p != NULL) it->second->setParameterByName("Speed", p);
		it->second->start();
		return;
	}
	std::cerr << "[Audio] Cannot find the event £º" << eventName << std::endl;
}
