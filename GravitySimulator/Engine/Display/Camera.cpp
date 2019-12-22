#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : m_position(position)
    , m_front(glm::vec3(0.0f, 0.0f, -1.0f))
    , m_worldUp(up)
    , m_yaw(yaw)
    , m_pitch(pitch)
    , m_movementSpeed(DefaultSpeed)
    , m_mouseSensitivity(DefaultSensitivity)
    , m_fov(DefaultFov)
{
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

float Camera::getFov() const
{
    return m_fov;
}

void Camera::processKeyboard(Movement direction)
{
    const float velocity = m_movementSpeed;
    if (direction == Movement::Forward)
    {
        m_position += m_front * velocity;
    }
    else if (direction == Movement::Backward)
    {
        m_position -= m_front * velocity;
    }
    else if (direction == Movement::Left)
    {
        m_position -= m_right * velocity;
    }
    else if (direction == Movement::Right)
    {
        m_position += m_right * velocity;
    }
    else if (direction == Movement::Up)
    {
        m_position += m_up * velocity;
    }
    else if (direction == Movement::Down)
    {
        m_position += m_up * -velocity;
    }
}

void Camera::processMouseMovement(float deltaX, float deltaY, bool constrainPitch)
{
    deltaX *= m_mouseSensitivity;
    deltaY *= m_mouseSensitivity;

    m_yaw = glm::mod(m_yaw + deltaX, 360.0f);
    m_pitch += deltaY;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (m_pitch > 89.0f)
        {
            m_pitch = 89.0f;
        }
        else if (m_pitch < -89.0f)
        {
            m_pitch = -89.0f;
        }
    }

    // Update Front, Right and Up Vectors using the updated Eular angles
    updateCameraVectors();
}

void Camera::processMouseScroll(float deltaY)
{
    if (m_fov >= 1.0f && m_fov <= 45.0f)
    {
        m_fov -= deltaY;
    }
    if (m_fov <= 1.0f)
    {
        m_fov = 1.0f;
    }
    if (m_fov >= 45.0f)
    {
        m_fov = 45.0f;
    }
}

glm::vec3 Camera::position() const
{
    return m_position;
}

glm::vec3 Camera::front() const
{
    return m_front;
}

void Camera::setSpeed(float speed)
{
    m_movementSpeed = speed;
}

void Camera::updateCameraVectors()
{
    // Calculate the new Front vector
    glm::vec3 front;
    front.x = glm::cos(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch));
    front.y = glm::sin(glm::radians(m_pitch));
    front.z = glm::sin(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch));
    m_front = glm::normalize(front);
    // Also re-calculate the Right and Up vector
    m_right = glm::normalize(glm::cross(m_front, m_worldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    m_up = glm::normalize(glm::cross(m_right, m_front));
}