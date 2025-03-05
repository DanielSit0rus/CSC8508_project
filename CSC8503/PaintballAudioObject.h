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

		void Play(bool isPlay) {
			if (isPlay) {
				// 如果音频已经停止，开始播放
				FMOD_STUDIO_PLAYBACK_STATE state;
				event->getPlaybackState(&state);

				if (state == FMOD_STUDIO_PLAYBACK_STOPPED) {
					event->start();  // 开始播放
				}
				else {
					event->setPaused(false);  // 继续播放
				}
			}
			else {
				event->setPaused(true);  // 暂停播放
			}
		}

	private:
		PaintballTransform* transform;

		EventInstance* event;
		FMOD_3D_ATTRIBUTES* sourceAttributes;
	};
}

