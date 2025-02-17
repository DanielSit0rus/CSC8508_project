#pragma once

namespace NCL {
	namespace CSC8503 {
		class Constraint	{
		public:
			Constraint() {}
			virtual ~Constraint() {}
			//Constraint test
			virtual void UpdateConstraint(float dt) = 0;
		};
	}
}