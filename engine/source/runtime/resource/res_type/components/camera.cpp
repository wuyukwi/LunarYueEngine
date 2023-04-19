#include "runtime/resource/res_type/components/camera.h"

#include "runtime/core/base/macro.h"

namespace LunarYue
{
    CameraComponentRes::CameraComponentRes(const CameraComponentRes& res)
    {
        const std::string& camera_type_name = res.m_parameter.getTypeName();
        if (camera_type_name == "FirstPersonCameraParameter")
        {
            m_parameter = LunarYue_REFLECTION_NEW(FirstPersonCameraParameter);
            LunarYue_REFLECTION_DEEP_COPY(FirstPersonCameraParameter, m_parameter, res.m_parameter);
        }
        else if (camera_type_name == "ThirdPersonCameraParameter")
        {
            m_parameter = LunarYue_REFLECTION_NEW(ThirdPersonCameraParameter);
            LunarYue_REFLECTION_DEEP_COPY(ThirdPersonCameraParameter, m_parameter, res.m_parameter);
        }
        else if (camera_type_name == "FreeCameraParameter")
        {
            m_parameter = LunarYue_REFLECTION_NEW(FreeCameraParameter);
            LunarYue_REFLECTION_DEEP_COPY(FreeCameraParameter, m_parameter, res.m_parameter);
        }
        else
        {
            LOG_ERROR("invalid camera type");
        }
    }

    CameraComponentRes::~CameraComponentRes() { LunarYue_REFLECTION_DELETE(m_parameter); }
} // namespace LunarYue