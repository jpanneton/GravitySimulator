#pragma once

#include "SimulationState.h"

class TestSimulationState : public State
{
public:
    TestSimulationState(StateStack& stack, Context context);

    bool update(sf::Time dt) final;

    bool handleEvent(const sf::Event& event) final;

    void draw() final;

private:
    std::vector<Texture> m_materialTextures;
    std::vector<Texture> m_skyBoxTextures;
    Camera m_camera;
    Shader m_shaderObject;
    Shader m_shaderSky;
    Entity m_entity;
    Entity m_entitySkyBox;
    GLfloat m_lastX;
    GLfloat m_lastY;
    sf::Vector2<unsigned> m_windowSize;

    SimulationControlID m_selectedControl;
    std::vector<sf::Text> m_controlLabels;
    std::vector<sf::Text> m_controlValues;
    std::vector<scalar> m_controlValuesStep;

    static constexpr scalar MaxVelocity = 5'000.0f;
    static constexpr scalar SlowVelocityMassStep = 1.0f;
    static constexpr scalar DefaultVelocityMassStep = 10.0f;
    static constexpr scalar FastVelocityMassStep = 100.f;

    static constexpr scalar MaxBodyMass = 1'000'000.0f;
    static constexpr scalar SlowBodyMassStep = 1.0f;
    static constexpr scalar DefaultBodyMassStep = 10.0f;
    static constexpr scalar FastBodyMassStep = 100.0f;

    static constexpr scalar NearClip = 0.1f;
    static constexpr scalar FarClip = 1'000'000.f;

    static constexpr scalar DefaultBodyMass = 100.f;

    scalar m_bodyMass;
    scalar m_bodyVelocity;
    Material m_bodyMaterial;

    void drawBodies();
    void drawSkyBox();
    void drawSimulationControls();

    void loadSimulationControls();
    void setSimulationControlPositions();
    void setSimulationControlValue(SimulationControlID id, scalar value);
    void setSimulationControlValue(SimulationControlID id, Material value);
};
