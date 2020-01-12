#include "TestSimulationState.h"
#include "Engine/Core/ResourceHolder.h"
#include "Engine/Physics/Serializer.h"
#include <string>

TestSimulationState::TestSimulationState(StateStack& stack, Context context)
    : State{ stack, context }
    , m_windowSize{ getContext().window->getSize() }
{
    sf::Mouse::setPosition({ static_cast<int>(m_windowSize.x) / 2, static_cast<int>(m_windowSize.y) / 2 }, *getContext().window);

    // Bodies shader and mesh
    m_shaderObject.loadShader("Resources/Shaders/Planet.vert", "Resources/Shaders/Planet.frag");
    m_entity = { MeshGeneration::generateSphere(1.0f, 20, 20, *getContext().materialTextures) };

    // Skybox shader and mesh
    m_shaderSky.loadShader("Resources/Shaders/Skybox.vert", "Resources/Shaders/Skybox.frag");
    m_entitySkyBox = { MeshGeneration::generateSkybox(*getContext().skyboxTextures) };

    BodiesArray bodies;
    deserializeBodies(std::ifstream{ "Systems/Random.txt" }, bodies);
    *getContext().system = System{ bodies };

    initCamera(bodies);
    loadSimulationControls();
}

bool TestSimulationState::update(sf::Time dt)
{
    getContext().system->update(dt);
    return true;
}

bool TestSimulationState::handleEvent(const sf::Event & event)
{
    return true;
}

void TestSimulationState::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    drawSkyBox();
    drawBodies();

    // Allow SFML to draw
    getContext().window->resetGLStates();

    drawSimulationControls();
}

void TestSimulationState::drawSkyBox()
{
    glDepthMask(GL_FALSE);
    m_shaderSky.bind();

    // Transformations
    glm::mat4 viewSky = glm::mat4(glm::mat3(m_camera.getViewMatrix()));
    glm::mat4 projectionSky = glm::perspective(glm::radians(m_camera.getFov()), static_cast<float>(m_windowSize.x) / m_windowSize.y, NearClip, FarClip);

    GLint viewLocationSky = glGetUniformLocation(m_shaderSky.id(), "view");
    GLint projectionLocationSky = glGetUniformLocation(m_shaderSky.id(), "projection");
    glUniformMatrix4fv(viewLocationSky, 1, GL_FALSE, glm::value_ptr(viewSky));
    glUniformMatrix4fv(projectionLocationSky, 1, GL_FALSE, glm::value_ptr(projectionSky));

    m_entitySkyBox.draw(m_shaderSky.id(), 0, true);
    glBindVertexArray(0);
    m_shaderSky.unbind();
    glDepthMask(GL_TRUE);
}

void TestSimulationState::drawSimulationControls()
{
    for (auto& label : m_controlLabels)
        getContext().window->draw(label);
    for (auto& value : m_controlValues)
        getContext().window->draw(value);
}

void TestSimulationState::drawBodies()
{
    m_shaderObject.bind();

    // Transformations
    glm::mat4 view = m_camera.getViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(m_camera.getFov()), static_cast<float>(m_windowSize.x) / m_windowSize.y, NearClip, FarClip);

    GLint viewLocation = glGetUniformLocation(m_shaderObject.id(), "view");
    GLint projectionLocation = glGetUniformLocation(m_shaderObject.id(), "projection");

    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

    auto& entity = m_entity;
    auto& shaderObject = m_shaderObject;
    for (const Body& body : *getContext().system)
    {
        entity.setPosition(body.getPosition());
        entity.scale(body.getRadius());
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        entity.draw(shaderObject.id(), body.getMaterial());
    }
    glBindVertexArray(0);
    m_shaderObject.unbind();
}

void TestSimulationState::initCamera(const BodiesArray& bodies)
{
    scalar totalMass = {};
    glm::vec3 averagePosition;
    for (const Body& body : bodies)
    {
        totalMass += body.getMass();
        averagePosition += body.getMass() * body.getPosition();
    }
    averagePosition /= totalMass;

    scalar farthestBodyDistance = std::numeric_limits<scalar>::lowest();
    for (const Body& body : bodies)
    {
        const scalar distance = glm::distance(body.getPosition(), averagePosition);
        if (distance > farthestBodyDistance)
        {
            farthestBodyDistance = distance;
        }
    }

    m_camera.setOrbitalRadius(3 * farthestBodyDistance);
    m_camera.setCenter(averagePosition);
    m_camera.setViewport({ m_windowSize.x, m_windowSize.y });
}

void TestSimulationState::loadSimulationControls()
{
    m_controlLabels.resize(SimulationControlID::NB_SIMULATION_CONTROLS);
    m_controlValues.resize(SimulationControlID::NB_SIMULATION_CONTROLS);
    m_controlValuesStep.resize(SimulationControlID::NB_SIMULATION_CONTROLS);

    m_controlValuesStep[SimulationControlID::Velocity] = DefaultVelocityMassStep;
    m_controlValuesStep[SimulationControlID::Mass] = DefaultBodyMassStep;

    auto& font = getContext().fonts->get(FontsID::Main);
    for (auto& label : m_controlLabels)
    {
        label.setFont(font);
        label.setCharacterSize(10);
    }
    for (auto& value : m_controlValues)
    {
        value.setFont(font);
        value.setCharacterSize(10);
    }

    auto& labelTimescale = m_controlLabels[SimulationControlID::Timescale];
    auto& valueTimescale = m_controlValues[SimulationControlID::Timescale];
    auto& labelVelocity = m_controlLabels[SimulationControlID::Velocity];
    auto& labelMass = m_controlLabels[SimulationControlID::Mass];
    auto& labelMaterial = m_controlLabels[SimulationControlID::BodyMaterial];

    labelTimescale.setString("Vitesse: ");
    labelVelocity.setString("Vélocité: ");
    labelMass.setString("Masse: ");
    labelMaterial.setString("Matériel: ");

    setSimulationControlValue(SimulationControlID::Timescale, getContext().system->timescale());
    setSimulationControlValue(SimulationControlID::Mass, m_bodyMass);
    setSimulationControlValue(SimulationControlID::Velocity, m_bodyVelocity);
    setSimulationControlValue(SimulationControlID::BodyMaterial, m_bodyMaterial);
    setSimulationControlPositions();

    labelTimescale.setFillColor(sf::Color::Yellow);
    valueTimescale.setFillColor(sf::Color::Yellow);
}

void TestSimulationState::setSimulationControlPositions()
{
    auto& labelTimescale = m_controlLabels[SimulationControlID::Timescale];
    auto& valueTimescale = m_controlValues[SimulationControlID::Timescale];
    auto bottom = getContext().window->getSize().y - labelTimescale.getGlobalBounds().height - 10.f;
    auto left = 5.f + labelTimescale.getGlobalBounds().width;

    labelTimescale.setPosition(5.f, bottom);
    valueTimescale.setPosition(left, bottom);
    for (int i = 1; i < SimulationControlID::NB_SIMULATION_CONTROLS; ++i)
    {
        auto& currentLabel = m_controlLabels[i];
        auto& currentValue = m_controlValues[i];
        auto& prevValue = m_controlValues[i - 1];

        left += prevValue.getGlobalBounds().width + 10.f;
        currentLabel.setPosition(left, bottom);
        left += currentLabel.getGlobalBounds().width + 5.f;
        currentValue.setPosition(left, bottom);
    }
}

void TestSimulationState::setSimulationControlValue(SimulationControlID id, scalar value)
{
    auto stringValue = std::to_string(value);
    m_controlValues[id].setString(stringValue.substr(0, stringValue.find('.') + 2));
    setSimulationControlPositions();
}

void TestSimulationState::setSimulationControlValue(SimulationControlID id, Material value)
{
    m_controlValues[id].setString(MaterialNames[value]);
    setSimulationControlPositions();
}