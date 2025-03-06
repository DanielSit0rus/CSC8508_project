#pragma once
#include "PaintballGameObject.h"
#include "PaintballGameWorld.h"


namespace NCL::CSC8503 {
	class BulletContactListener : public rp3d::EventListener {
	public:
		void onContact(const rp3d::CollisionCallback::CallbackData& callbackData) override;
		void onTrigger(const rp3d::OverlapCallback::CallbackData& callbackData) override;
		
	};


}