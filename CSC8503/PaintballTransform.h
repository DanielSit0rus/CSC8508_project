#pragma once

#include <reactphysics3d/reactphysics3d.h>
#include "Util.h"

using std::vector;

using namespace NCL::Maths;

namespace NCL {
	namespace CSC8503 {
		class PaintballTransform
		{
		public:
			PaintballTransform();
			~PaintballTransform();

			PaintballTransform& SetPosition(const rp3d::Vector3& worldPos);
			PaintballTransform& SetScale(const rp3d::Vector3& worldScale);
			PaintballTransform& SetRatioR(const rp3d::Vector3& r);
			PaintballTransform& SetOrientation(const rp3d::Quaternion& newOr);

			rp3d::Vector3 GetPosition() const { return transform.getPosition(); }

			rp3d::Vector3 GetScale() const { return scale; }
			rp3d::Vector3 GetRatioR() const { return ratioR; }

			rp3d::Quaternion GetOrientation() const { return transform.getOrientation(); }

			Matrix4 GetMatrix();
			void UpdateMatrix();

			void SetRpTransform(rp3d::Transform t) { transform = t; }
			rp3d::Transform GetRpTransform() const { return transform; }

		protected:
			Matrix4				matrix;
			rp3d::Transform		transform;
			rp3d::Vector3		scale;
			rp3d::Vector3		ratioR;

		};
	}
}

