#pragma once
#include "PaintballTransform.h"
#include <fmod_studio.hpp>

using namespace FMOD;
using namespace FMOD::Studio;

namespace NCL::CSC8503 {
	class PaintballAudioObject
	{
	public:
		PaintballAudioObject(PaintballTransform* parentTransform, EventInstance* ins);
		~PaintballAudioObject();

		void Update();

		PaintballTransform* GetTransform() const {
			return transform;
		}

		void Play(bool isPlay);

	private:
		PaintballTransform* transform;

		EventInstance* event;
		FMOD_3D_ATTRIBUTES* sourceAttributes;
	};
}

