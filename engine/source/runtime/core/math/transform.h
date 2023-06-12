#pragma once
#include "runtime/core/math/matrix4.h"
#include "runtime/core/math/quaternion.h"
#include "runtime/core/math/vector3.h"
#include "runtime/core/meta/reflection/reflection.h"

namespace LunarYue
{
    REFLECTION_TYPE(Transform)
    CLASS(Transform, Fields)
    {
        REFLECTION_BODY(Transform);

    public:
        Vector3    m_position {Vector3::ZERO};
        Vector3    m_scale {Vector3::UNIT_SCALE};
        Quaternion m_rotation {Quaternion::IDENTITY};
        META(Disable)
        Matrix4x4 m_mat {Matrix4x4::IDENTITY};

        Transform() = default;
        Transform(const Vector3& position, const Quaternion& rotation, const Vector3& scale) :
            m_position {position}, m_scale {scale}, m_rotation {rotation}
        {}

        Matrix4x4 getMatrix() const { return m_mat; }

        void degreesToQuaternion(const Vector3& degrees)
        {
            float pitch = Math::degreesToRadians(degrees.x / 2);
            float roll  = Math::degreesToRadians(degrees.y / 2);
            float yaw   = Math::degreesToRadians(degrees.z / 2);

            m_rotation.w = Math::cos(pitch) * Math::cos(roll) * Math::cos(yaw) + Math::sin(pitch) * Math::sin(roll) * Math::sin(yaw);
            m_rotation.x = Math::sin(pitch) * Math::cos(roll) * Math::cos(yaw) - Math::cos(pitch) * Math::sin(roll) * Math::sin(yaw);
            m_rotation.y = Math::cos(pitch) * Math::sin(roll) * Math::cos(yaw) + Math::sin(pitch) * Math::cos(roll) * Math::sin(yaw);
            m_rotation.z = Math::cos(pitch) * Math::cos(roll) * Math::sin(yaw) - Math::sin(pitch) * Math::sin(roll) * Math::cos(yaw);

            m_rotation.normalise();
        }

        Vector3 getRotationDegrees()
        {
            Vector3 degrees_val;

            degrees_val.x = m_rotation.getPitch(false).valueDegrees();
            degrees_val.y = m_rotation.getRoll(false).valueDegrees();
            degrees_val.z = m_rotation.getYaw(false).valueDegrees();

            return degrees_val;
        }
    };
} // namespace LunarYue
