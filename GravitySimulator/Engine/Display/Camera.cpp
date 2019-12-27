#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

float Camera::getFov() const
{
    return m_fov;
}

//------------------------------------------------------------------------

DraggableOrbitCamera::DraggableOrbitCamera(float orbitalRadius, const VectorType& center) noexcept
    : m_baseQuaternion(glm::angleAxis(0.0f, VectorType(0.0f, 1.0f, 0.0f))) // Y-axis
    , m_quaternion(m_baseQuaternion)
{
    setOrbitalRadius(orbitalRadius);
    setCenter(center);
}

void DraggableOrbitCamera::setViewport(const glm::ivec2& frame) noexcept
{
    m_frame = frame;
}

void DraggableOrbitCamera::setOrbitalRadius(float radius) noexcept
{
    m_radius = glm::max(MinRadius, glm::min(radius, MaxRadius));
}

void DraggableOrbitCamera::zoom(float delta, bool scaled) noexcept
{
    if (scaled)
    {
        // The higher the value, the faster it takes to zoom out when completely zoomed in
        constexpr float minZoomRadius = 10.0f;
        // Otherwise, produces a null ratio when completely zoomed in (i.e. can't zoom out)
        static_assert(minZoomRadius > 0.0f);
        delta *= ((m_radius - MinRadius + minZoomRadius) / (MaxRadius - MinRadius + minZoomRadius));
    }
    m_lastZoom += delta;
}

float DraggableOrbitCamera::getOrbitalRadius() const noexcept
{
    return m_radius;
}

void DraggableOrbitCamera::setCenter(const VectorType& center) noexcept
{
    m_center = center;
}

void DraggableOrbitCamera::moveCenter(const VectorType& delta) noexcept
{
    VectorType cameraVector = getPosition();
    cameraVector.y = 0.0f;
    cameraVector = glm::normalize(cameraVector);
    m_center += delta.x * VectorType(cameraVector.z, 0.0f, -cameraVector.x);
    m_center += delta.z * cameraVector;
    m_center.y = m_center.y + delta.y;
}

DraggableOrbitCamera::VectorType DraggableOrbitCamera::getCenter() const noexcept
{
    return m_center;
}

DraggableOrbitCamera::VectorType DraggableOrbitCamera::getPosition() const noexcept
{
    return m_center + m_quaternion * VectorType(0.0f, 0.0f, m_radius);
}

DraggableOrbitCamera::MatrixType DraggableOrbitCamera::getViewMatrix() const
{
    // https://www.3dgep.com/understanding-the-view-matrix/#Arcball_Orbit_Camera
    // The view matrix is the inverse of the camera's transformation matrix
    // In fact, a camera is an abstract concept and the whole world is being transformed

    // Instead of creating a transformation matrix and inverting it, we can optimize this
    // by pre-inverting the operations and swapping the order of multiplication

    // Orbital radius translation (pre-inverted)
    MatrixType T0 = glm::translate(MatrixType(), -VectorType(0.0f, 0.0f, m_radius));
    // Orbital rotation (pre-inverted)
    MatrixType R = glm::toMat4(glm::inverse(m_quaternion));
    // Orbital center rotation (pre-inverted)
    MatrixType T1 = glm::translate(MatrixType(), -m_center);
    // View matrix (order of multiplication swapped)
    return T0 * R * T1;
}

DraggableOrbitCamera::VectorType DraggableOrbitCamera::getDirection() const
{
    return -glm::normalize(m_quaternion * VectorType(0.0f, 0.0f, m_radius));
}

void DraggableOrbitCamera::update() noexcept
{
    // Move camera
    const glm::vec2 newMouseDelta = mouseDeltaToProportion(m_lastMouseDrag);
    m_pitch += newMouseDelta.y;
    m_yaw = glm::mod(m_yaw + newMouseDelta.x, glm::two_pi<float>());

    constexpr float epsilon = 0.001f;

    if (m_pitch > glm::pi<float>() / 2.0f - epsilon)
        m_pitch = glm::pi<float>() / 2.0f - epsilon;
    if (m_pitch < -(glm::pi<float>() / 2.0f - epsilon))
        m_pitch = -(glm::pi<float>() / 2.0f - epsilon);

    QuaternionType pitchQuat = glm::angleAxis(m_pitch, VectorType(1.0f, 0.0f, 0.0f));
    QuaternionType yawQuat = glm::angleAxis(m_yaw, VectorType(0.0f, 1.0f, 0.0f));
    m_quaternion = m_baseQuaternion * glm::normalize(yawQuat * pitchQuat);

    // Zoom camera
    setOrbitalRadius(m_radius - m_lastZoom);

    // Smooth transformations for next update
    m_lastMouseDrag *= (1.0f - SmoothFactor);
    m_lastZoom *= (1.0f - SmoothFactor);
}

void DraggableOrbitCamera::mouseDrag(const glm::vec2& mouseDelta) noexcept
{
    m_lastMouseDrag = mouseDelta;
}

glm::vec2 DraggableOrbitCamera::mouseDeltaToProportion(const glm::vec2& mouseDelta) const noexcept
{
    // setViewport() must be called before any mouse input callbacks!
    assert(m_frame.x > 0 && m_frame.y > 0);

    const float scale = glm::min(m_frame.x, m_frame.y) / 2.0f;
    return { mouseDelta.x / scale, -mouseDelta.y / scale };
}