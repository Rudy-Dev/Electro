//                    ELECTRO ENGINE
// Copyright(c) 2021 - Electro Team - All rights reserved
#include "epch.hpp"
#include "EditorCamera.hpp"
#include "Core/Input.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#define M_PI 3.14159f

namespace Electro
{
    EditorCamera::EditorCamera(float fov, float aspectRatio, float nearClip, float farClip)
        : mFOV(fov), mAspectRatio(aspectRatio), mFarClip(farClip), mNearClip(nearClip), Camera(glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip))
    {
        const glm::vec3 position = { -5, 5, 5 };
        mDistance = glm::distance(position, mFocalPoint);
        mYaw = 3.0f * M_PI / 4.0f;
        mPitch = M_PI / 4.0f;
        UpdateView();
    }

    void EditorCamera::UpdateProjection()
    {
        mAspectRatio = mViewportWidth / mViewportHeight;
        mProjection = glm::perspective(glm::radians(mFOV), mAspectRatio, mNearClip, mFarClip);
    }

    void EditorCamera::UpdateView()
    {
        //m_Yaw = m_Pitch = 0.0f; // Lock the camera's rotation
        mPosition = CalculatePosition();

        const glm::quat orientation = GetOrientation();
        mViewMatrix = glm::translate(glm::mat4(1.0f), mPosition) * glm::toMat4(orientation);
        mViewMatrix = glm::inverse(mViewMatrix);
    }

    std::pair<float, float> EditorCamera::PanSpeed() const
    {
        float x = std::min(mViewportWidth / 1000.0f, 2.4f); // max = 2.4f
        float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

        float y = std::min(mViewportHeight / 1000.0f, 2.4f); // max = 2.4f
        float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

        return { xFactor, yFactor };
    }

    float EditorCamera::RotationSpeed() const
    {
        return 1.8f;
    }

    float EditorCamera::ZoomSpeed() const
    {
        float distance = mDistance * 0.2f;
        distance = std::max(distance, 0.0f);
        float speed = distance * distance;
        speed = std::min(speed, 200.0f); // max speed = 200
        return speed;
    }

    void EditorCamera::OnUpdate(Timestep ts)
    {
        if (Input::IsKeyPressed(Key::LeftAlt))
        {
            const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
            const glm::vec2 delta = (mouse - mInitialMousePosition) * 0.003f;
            mInitialMousePosition = mouse;

            if (Input::IsMouseButtonPressed(Mouse::ButtonMiddle))
                MousePan(delta);
            else if (Input::IsMouseButtonPressed(Mouse::ButtonLeft))
                MouseRotate(delta);
            else if (Input::IsMouseButtonPressed(Mouse::ButtonRight))
                MouseZoom(delta.y);
        }
        UpdateView();
    }

    void EditorCamera::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<MouseScrolledEvent>(ELECTRO_BIND_EVENT_FN(EditorCamera::OnMouseScroll));
    }

    bool EditorCamera::OnMouseScroll(MouseScrolledEvent& e)
    {
        const float delta = e.GetDelta() * 0.1f;
        MouseZoom(delta);
        UpdateView();
        return false;
    }

    void EditorCamera::MousePan(const glm::vec2& delta)
    {
        auto [xSpeed, ySpeed] = PanSpeed();
        mFocalPoint += -GetRightDirection() * delta.x * xSpeed * mDistance;
        mFocalPoint += GetUpDirection() * delta.y * ySpeed * mDistance;
    }

    void EditorCamera::MouseRotate(const glm::vec2& delta)
    {
        const float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
        mYaw += yawSign * delta.x * RotationSpeed();
        mPitch += delta.y * RotationSpeed();
    }

    void EditorCamera::MouseZoom(float delta)
    {
        mDistance -= delta * ZoomSpeed();
        if (mDistance < 1.0f)
        {
            mFocalPoint += GetForwardDirection();
            mDistance = 1.0f;
        }
    }

    void EditorCamera::Focus(const glm::vec3& focusPoint)
    {
        mFocalPoint = focusPoint;
        if (mDistance > mMinFocusDistance)
        {
            float distance = mDistance - mMinFocusDistance;
            MouseZoom(distance / ZoomSpeed());
            UpdateView();
        }
    }

    glm::vec3 EditorCamera::GetUpDirection() const
    {
        return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
    }

    glm::vec3 EditorCamera::GetRightDirection() const
    {
        return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
    }

    glm::vec3 EditorCamera::GetForwardDirection() const
    {
        return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
    }

    glm::vec3 EditorCamera::CalculatePosition() const
    {
        return mFocalPoint - GetForwardDirection() * mDistance;
    }

    glm::quat EditorCamera::GetOrientation() const
    {
        return glm::quat(glm::vec3(-mPitch, -mYaw, 0.0f));
    }
}
