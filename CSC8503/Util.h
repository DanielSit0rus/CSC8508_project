#pragma once
#include <reactphysics3d/reactphysics3d.h>
#include <Vector.h>
#include "../NCLCoreClasses/Quaternion.h"

using namespace NCL::Maths;

namespace NCL {
    namespace CSC8503 {

        class Util
        {
        public:
            static rp3d::Vector3 NCLToRP3d(const Vector3& v) {
                return rp3d::Vector3(v.x, v.y, v.z);
            }

            static Vector3 RP3dToNCL(const rp3d::Vector3& v) {
                return Vector3(v.x, v.y, v.z);
            }

            static rp3d::Quaternion NCLToRP3d(const Quaternion& v) {
                return rp3d::Quaternion(v.x, v.y, v.z, v.w);
            }

            static Quaternion RP3dToNCL(const rp3d::Quaternion& v) {
                return Quaternion(v.x, v.y, v.z, v.w);
            }

            template <typename T>
            static int getValueInMap(const std::map<int, T*>& objectMap) {
                auto it = std::find_if(objectMap.begin(), objectMap.end(),
                    [](const std::pair<int, T*>& pair) {
                        return pair.second == nullptr;
                    });

                if (it != objectMap.end()) {
                    return it->first;
                }
                return static_cast<int>(objectMap.size());
            }

        };

    }
}
