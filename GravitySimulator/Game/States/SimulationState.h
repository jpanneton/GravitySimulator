#pragma once

#include "State.h"
#include "Engine/Physics/System.h"
#include "Engine/Display/Entity.h"
#include "Engine/Display/MeshGeneration.h"
#include "Engine/Display/Shader.h"
#include "Engine/Display/Camera.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

enum SimulationControlID
{
    Timescale = 0,
    Velocity,
    Mass,
    BodyMaterial,
    NB_SIMULATION_CONTROLS
};

class SimulationState : public State
{
public:
    SimulationState(StateStack& stack, Context context);

    bool update(sf::Time dt) final;

    bool handleEvent(const sf::Event& event) final;

    void draw() final;

private: 
    std::vector<Texture> m_materialTextures;
    std::vector<Texture> m_skyBoxTextures;
    Camera m_camera;

    bool m_mouseDrag = false;

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
    sf::Sprite m_crosshairSprite;
    bool m_showCrosshair = false;

    bool m_slowMode;
    bool m_fastMode;
    void setSlowMode();
    void setNormalMode();
    void setFastMode();

    void moveCamera();
    void controlSimulation(sf::Keyboard::Key);
    void setMode(sf::Keyboard::Key, sf::Event::EventType);
    void resetSystem();
    void drawBodies();
    void drawSkyBox();
    void drawSimulationControls();

    void loadSimulationControls();
    void setSimulationControlPositions();
    void setSimulationControlValue(SimulationControlID id, scalar value);
    void setSimulationControlValue(SimulationControlID id, Material value);
    void selectNextControl();
    void selectPrevControl();
    void increaseControlValue();
    void decreaseControlValue();
    void shootBody();
};
