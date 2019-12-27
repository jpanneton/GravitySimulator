#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

class Camera
{
public:
    using MatrixType = glm::mat4;
    using VectorType = glm::vec3;
    using QuaternionType = glm::quat;
    
    static constexpr float DefaultPitch = {};
    static constexpr float DefaultYaw = {};
    static constexpr float DefaultFov = 45.0f;

    virtual MatrixType getViewMatrix() const = 0;

    float getFov() const;

protected:
    // Euler Angles
    float m_pitch = DefaultPitch; // Rotation around X-axis in radians (elevation: up and down)
    float m_yaw = DefaultYaw;     // Rotation around Y-axis in radians (azimuth: left and right)
    float m_fov = DefaultFov;
};

//--------------------------------------------------------------------------------------------
/// Generates orbital camera transformations from mouse input events and a set of parameters.
//--------------------------------------------------------------------------------------------
class DraggableOrbitCamera : public Camera
{
    static constexpr float MinRadius = 5.0f;
    static constexpr float MaxRadius = 100'000.0f;
    static constexpr float SmoothFactor = 0.1f; // 0 = no transformation smoothing

    static_assert(MinRadius > 0.0f && MaxRadius > 0.0f);
    static_assert(MaxRadius >= MinRadius);
    static_assert(SmoothFactor >= 0.0f && SmoothFactor < 1.0f);

public:
    //----------------------------------------------------------------------------------------
    /// Constructor.
    /// @param[in] orbitalRadius			Distance from the center point (point where the camera is pointing).
    /// @param[in] center                   Center point in world coordinates.
    //----------------------------------------------------------------------------------------
    DraggableOrbitCamera(float orbitalRadius = MinRadius, const VectorType& center = {}) noexcept;

    //----------------------------------------------------------------------------------------
    /// Sets the frame of the camera.
    /// @param[in] frame			        Viewport frame.
    //----------------------------------------------------------------------------------------
    void setViewport(const glm::ivec2& frame) noexcept;

    //----------------------------------------------------------------------------------------
    /// Sets the distance from the center point.
    /// @see DraggableOrbitCamera::zoom
    /// @param[in] radius					Distance from the center point (point where the camera is pointing).
    //----------------------------------------------------------------------------------------
    void setOrbitalRadius(float radius) noexcept;

    //----------------------------------------------------------------------------------------
    /// Moves the camera towards the center point.
    /// @see DraggableOrbitCamera::setOrbitalRadius
    /// @param[in] delta					Distance to move the camera towards the center point.
    /// @param[in] scaled					If true, linear scaling will be applied to the zoom.
    //----------------------------------------------------------------------------------------
    void zoom(float delta, bool scaled = true) noexcept;

    //----------------------------------------------------------------------------------------
    /// Returns the distance from the center point.
    /// @return								Orbital radius.
    //----------------------------------------------------------------------------------------
    float getOrbitalRadius() const noexcept;

    //----------------------------------------------------------------------------------------
    /// Sets the point where the camera is pointing (center of gravity).
    /// @see DraggableOrbitCamera::moveCenter
    /// @param[in] center					Center point in world coordinates.
    //----------------------------------------------------------------------------------------
    void setCenter(const VectorType& center) noexcept;

    //----------------------------------------------------------------------------------------
    /// Moves the point where the camera is pointing (center of gravity).
    /// @see DraggableOrbitCamera::setCenter
    /// @param[in] delta					Delta to move the center point.
    //----------------------------------------------------------------------------------------
    void moveCenter(const VectorType& delta) noexcept;

    //----------------------------------------------------------------------------------------
    /// Returns the point where the camera is pointing (center of gravity).
    /// @return								Orbital center.
    //----------------------------------------------------------------------------------------
    VectorType getCenter() const noexcept;

    //----------------------------------------------------------------------------------------
    /// Returns the position of the camera in world space.
    /// @return								Camera position.
    //----------------------------------------------------------------------------------------
    VectorType getPosition() const noexcept;

    //----------------------------------------------------------------------------------------
    /// Returns the transformation matrix of the current camera state.
    /// @return								View matrix.
    //----------------------------------------------------------------------------------------
    MatrixType getViewMatrix() const override;

    //----------------------------------------------------------------------------------------
    /// Returns the front vector of the camera.
    /// @return								Front vector.
    //----------------------------------------------------------------------------------------
    VectorType getDirection() const;

    //----------------------------------------------------------------------------------------
    /// Updates the camera transformations.
    //----------------------------------------------------------------------------------------
    void update() noexcept;

    //----------------------------------------------------------------------------------------
    /// Mouse drag event callback.
    /// @param[in] mouseDelta				Mouse delta.
    //----------------------------------------------------------------------------------------
    void mouseDrag(const glm::vec2& mouseDelta) noexcept;

private:
    //----------------------------------------------------------------------------------------
    /// Converts a mouse drag in pixels to a normalized delta (between -1 and 1).
    /// Allows dragging to be relative to the viewport size.
    /// @param[in] mouseDrag			    Mouse delta in pixels.
    /// @return								Normalized mouse position.
    //----------------------------------------------------------------------------------------
    glm::vec2 mouseDeltaToProportion(const glm::vec2& mouseDelta) const noexcept;

    float m_radius = {};				/// Distance of the camera from the center point.
    VectorType m_center;                /// Point where the camera is pointing (center of gravity).
    glm::ivec2 m_frame;				    /// Frame of the camera.
    QuaternionType m_baseQuaternion;	/// Rotation axis quaternion.
    QuaternionType m_quaternion;		/// Camera quaternion.

    glm::vec2 m_lastMouseDrag;          /// Last mouse drag delta.
    float m_lastZoom = {};              /// Last zoom delta.
};