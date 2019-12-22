#pragma once

#include <glm/glm.hpp>

// An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // Default camera values
    static constexpr float DefaultYaw = -90.0f;
    static constexpr float DefaultPitch = 0.0f;
    static constexpr float SlowSpeed = 0.5f;
    static constexpr float DefaultSpeed = 3.0f;
    static constexpr float FastSpeed = 50.0f;
    static constexpr float DefaultSensitivity = 0.1f;
    static constexpr float DefaultFov = 45.0f;
    static inline const glm::vec3 DefaultPosition = glm::vec3(0.f, 0.f, 100.f);

    enum class Movement
    {
        Forward,
        Backward,
        Left,
        Right,
        Up,
        Down
    };

public:
    // Constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f);

    // Returns the view matrix calculated using Eular Angles and the LookAt Matrix
    glm::mat4 getViewMatrix() const;

    float getFov() const;

    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void processKeyboard(Movement direction);

    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void processMouseMovement(float deltaX, float deltaY, bool constrainPitch = true);

    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void processMouseScroll(float deltaY);

    glm::vec3 position() const;
    glm::vec3 front() const;

    void setSpeed(float speed);

private:
    // Calculates the front vector from the Camera's (updated) Eular Angles
    void updateCameraVectors();

    // Camera Attributes
    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_worldUp;
    // Euler Angles
    float m_yaw; // Left & Right : rotation around Y-axis
    float m_pitch; // Up & Down : rotation around X-axis
    // Camera options
    float m_movementSpeed;
    float m_mouseSensitivity;
    float m_fov;
};