#pragma once
#include "PaintballTransform.h"
#include <fmod_studio.hpp>

using namespace FMOD;
using namespace FMOD::Studio;

namespace NCL::CSC8503 {
	class PaintballAudioObject
	{
	public:
		PaintballAudioObject(PaintballTransform* parentTransform);
		~PaintballAudioObject();

		void Update();

		PaintballTransform* GetTransform() const {
			return transform;
		}

		void AddEvent(const std::string& eventName);
		
		void PlayEvent(const std::string& eventName, float p = NULL);

	private:
		PaintballTransform* transform;
		FMOD_3D_ATTRIBUTES* sourceAttributes;

		std::unordered_map<std::string, EventInstance*> events;
	};
}

