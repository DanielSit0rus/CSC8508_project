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
				// �����Ƶ�Ѿ�ֹͣ����ʼ����
				FMOD_STUDIO_PLAYBACK_STATE state;
				event->getPlaybackState(&state);

				if (state == FMOD_STUDIO_PLAYBACK_STOPPED) {
					event->start();  // ��ʼ����
				}
				else {
					event->setPaused(false);  // ��������
				}
			}
			else {
				event->setPaused(true);  // ��ͣ����
			}
		}

	private:
		PaintballTransform* transform;

		EventInstance* event;
		FMOD_3D_ATTRIBUTES* sourceAttributes;
	};
}

