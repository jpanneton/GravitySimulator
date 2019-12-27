#include "SimulationState.h"
#include "PausedSimulationState.h"
#include "SaveSimulationState.h"
#include "Engine/Core/ResourceHolder.h"
#include "Engine/Physics/Serializer.h"
#include <string>

SimulationState::SimulationState(StateStack & stack, Context context)
    : State(stack, context)
    , m_materialTextures{ *getContext().materialTextures }
    , m_skyBoxTextures{ *getContext().skyBoxTextures }
    , m_windowSize{ getContext().window->getSize()  }
{
    getContext().window->setMouseCursorGrabbed(true);
    getContext().window->setMouseCursorVisible(false);

    sf::Mouse::setPosition({ static_cast<int>(m_windowSize.x) / 2, static_cast<int>(m_windowSize.y) / 2 }, *getContext().window);

    m_crosshairSprite.setTexture(context.textures->get(TexturesID::Crosshair));
    m_crosshairSprite.setPosition(m_windowSize.x / 2 - m_crosshairSprite.getGlobalBounds().width / 2, m_windowSize.y / 2 - m_crosshairSprite.getGlobalBounds().height / 2);

    // Bodies shader and mesh
    m_shaderObject.loadShader("Resources/Shaders/Planet.vert", "Resources/Shaders/Planet.frag");
    m_entity = { MeshGeneration::generateSphere(m_materialTextures) };
    
    // SkyBox shader and mesh
    m_shaderSky.loadShader("Resources/Shaders/Skybox.vert", "Resources/Shaders/Skybox.frag");
    m_entitySkyBox = { MeshGeneration::generateSkyBox(m_skyBoxTextures) };

    BodiesArray bodies;
    deserializeBodies(std::ifstream{ getContext().selectedSystem->c_str() }, bodies);
    *getContext().system = System{ bodies, PhysicsEngine{} };

    initCamera(bodies);
    loadSimulationControls();
}

bool SimulationState::update(sf::Time dt)
{
    m_camera.update();
    getContext().system->update(dt);
    return true;
}

bool SimulationState::handleEvent(const sf::Event& event)
{
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
    {
        m_mouseDrag = true;
    }
    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
    {
        m_mouseDrag = false;
    }
    if (event.type == sf::Event::MouseMoved)
    {
        if (m_mouseDrag)
        {
            float dx = m_windowSize.x / 2.0f - event.mouseMove.x;
            float dy = m_windowSize.y / 2.0f - event.mouseMove.y;
            m_camera.mouseDrag({ dx, -dy });
        }

        // Reset mouse position
        sf::Mouse::setPosition({ static_cast<int>(m_windowSize.x) / 2, static_cast<int>(m_windowSize.y) / 2 }, *getContext().window);
    }
    if (event.type == sf::Event::MouseWheelMoved)
    {
        m_camera.zoom(1000.0f * event.mouseWheel.delta);
    }
    if (event.type == sf::Event::KeyPressed)
    {	
        if (event.key.code == sf::Keyboard::S)
        {
            m_showCrosshair = true;
        }
        else
        {
            controlSimulation(event.key.code);
            setMode(event.key.code, event.type);
        }
    }
    if (event.type == sf::Event::KeyReleased)
    {
        if (event.key.code == sf::Keyboard::S)
        {
            m_showCrosshair = false;
            shootBody();
        }
        else
        {
            setMode(event.key.code, event.type);
        }
    }
    
    return true;
}

void SimulationState::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    drawSkyBox();
    drawBodies();
    
    // Allow SFML to draw
    getContext().window->resetGLStates();

    drawSimulationControls();

    if (m_showCrosshair)
        getContext().window->draw(m_crosshairSprite);
}

void SimulationState::drawSkyBox()
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

void SimulationState::drawSimulationControls()
{
    for (auto& label : m_controlLabels)
        getContext().window->draw(label);
    for (auto& value : m_controlValues)
        getContext().window->draw(value);
}

void SimulationState::drawBodies()
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
    std::for_each(getContext().system->begin(), getContext().system->end(), [&entity, &shaderObject](Body& body) {
        entity.setPosition(body.position());
        entity.scale(body.radius());
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        entity.draw(shaderObject.id(), body.material());
    });
    glBindVertexArray(0);
    m_shaderObject.unbind();
}

void SimulationState::setSlowMode()
{
    m_slowMode = true;
    m_controlValuesStep[SimulationControlID::Velocity] = SlowVelocityMassStep;
    m_controlValuesStep[SimulationControlID::Mass] = SlowBodyMassStep;
}

void SimulationState::setNormalMode()
{
    m_slowMode = m_fastMode = false;
    m_controlValuesStep[SimulationControlID::Velocity] = DefaultVelocityMassStep;
    m_controlValuesStep[SimulationControlID::Mass] = DefaultBodyMassStep;
}

void SimulationState::setFastMode()
{
    m_fastMode = true;
    m_controlValuesStep[SimulationControlID::Velocity] = FastVelocityMassStep;
    m_controlValuesStep[SimulationControlID::Mass] = FastBodyMassStep;
}

void SimulationState::controlSimulation(sf::Keyboard::Key key)
{
    switch (key)
    {
    case sf::Keyboard::P:
        requestStackPush(StatesID::Paused);
        break;
    case sf::Keyboard::Enter:
        requestStackPush(StatesID::Save);
        break;
    case sf::Keyboard::R:
        resetSystem();
        break;
    case sf::Keyboard::Left:
        selectPrevControl();
        break;
    case sf::Keyboard::Right:
        selectNextControl();
        break;
    case sf::Keyboard::Up:
        increaseControlValue();
        break;
    case sf::Keyboard::Down:
        decreaseControlValue();
        break;
    case sf::Keyboard::Escape:
        requestStackPop();
        requestStackPush(StatesID::Title);
        break;
    }
}

void SimulationState::setMode(sf::Keyboard::Key key, sf::Event::EventType type)
{
    if (key != sf::Keyboard::LShift && key != sf::Keyboard::LAlt)
        return;

    if (type == sf::Event::EventType::KeyPressed)
    {
        switch (key)
        {
        case sf::Keyboard::LShift:
            if (!m_slowMode)
                setFastMode();
            break;
        case sf::Keyboard::LAlt:
            if (!m_fastMode)
                setSlowMode();
            break;
        }
    }
    else
    {
        setNormalMode();
    }
}

void SimulationState::resetSystem()
{
    BodiesArray bodies;
    deserializeBodies(std::ifstream{ *getContext().selectedSystem }, bodies);
    *getContext().system = System{ bodies, PhysicsEngine{} };

    setNormalMode();

    setSimulationControlValue(SimulationControlID::Timescale, getContext().system->timescale());
    m_bodyMass = DefaultBodyMass;
    setSimulationControlValue(SimulationControlID::Mass, m_bodyMass);
    m_bodyVelocity = 10.f;
    setSimulationControlValue(SimulationControlID::Velocity, m_bodyVelocity);
}

void SimulationState::initCamera(const BodiesArray& bodies)
{
    scalar totalMass = {};
    for (const Body& body : bodies)
    {
        totalMass += body.mass();
    }

    glm::vec3 averagePosition;
    for (const Body& body : bodies)
    {
        averagePosition += (body.mass() / totalMass) * body.position();
    }

    scalar farthestBodyDistance = std::numeric_limits<scalar>::lowest();
    for (const Body& body : bodies)
    {
        const scalar distance = glm::distance(body.position(), averagePosition);
        if (distance > farthestBodyDistance)
        {
            farthestBodyDistance = distance;
        }
    }

    m_camera.setOrbitalRadius(3 * farthestBodyDistance);
    m_camera.setCenter(averagePosition);
    m_camera.setViewport({ m_windowSize.x, m_windowSize.y });
}

void SimulationState::loadSimulationControls()
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

void SimulationState::setSimulationControlPositions()
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

void SimulationState::setSimulationControlValue(SimulationControlID id, scalar value)
{
    auto stringValue = std::to_string(value);
    m_controlValues[id].setString(stringValue.substr(0, stringValue.find('.') + 2));
    setSimulationControlPositions();
}

void SimulationState::setSimulationControlValue(SimulationControlID id, Material value)
{
    m_controlValues[id].setString(MaterialNames[value]);
    setSimulationControlPositions();
}

void SimulationState::selectNextControl()
{
    int next = (m_selectedControl + 1) % SimulationControlID::NB_SIMULATION_CONTROLS;
    m_controlLabels[m_selectedControl].setFillColor(sf::Color::White);
    m_controlValues[m_selectedControl].setFillColor(sf::Color::White);
    m_controlLabels[next].setFillColor(sf::Color::Yellow);
    m_controlValues[next].setFillColor(sf::Color::Yellow);
    m_selectedControl = static_cast<SimulationControlID>(next);
}

void SimulationState::selectPrevControl()
{
    int prev = m_selectedControl == 0 ? SimulationControlID::NB_SIMULATION_CONTROLS - 1 : m_selectedControl - 1;
    m_controlLabels[m_selectedControl].setFillColor(sf::Color::White);
    m_controlValues[m_selectedControl].setFillColor(sf::Color::White);
    m_controlLabels[prev].setFillColor(sf::Color::Yellow);
    m_controlValues[prev].setFillColor(sf::Color::Yellow);
    m_selectedControl = static_cast<SimulationControlID>(prev);
}

void SimulationState::increaseControlValue()
{
    switch (m_selectedControl)
    {
    case SimulationControlID::Timescale:
        getContext().system->increaseTimescale();
        setSimulationControlValue(m_selectedControl, getContext().system->timescale());
        break;
    case SimulationControlID::Mass:
        m_bodyMass = std::min(m_bodyMass + m_controlValuesStep[m_selectedControl], MaxBodyMass);
        setSimulationControlValue(m_selectedControl, m_bodyMass);
        break;
    case SimulationControlID::Velocity:
        m_bodyVelocity = std::min(m_bodyVelocity + m_controlValuesStep[m_selectedControl], MaxVelocity);
        setSimulationControlValue(m_selectedControl, m_bodyVelocity);
        break;
    case SimulationControlID::BodyMaterial:
        m_bodyMaterial = static_cast<Material>((m_bodyMaterial + 1) % NB_MATERIALS);
        setSimulationControlValue(m_selectedControl, m_bodyMaterial);
        break;
    }
}

void SimulationState::decreaseControlValue()
{
    switch (m_selectedControl)
    {
    case SimulationControlID::Timescale:
        getContext().system->decreaseTimescale();
        setSimulationControlValue(m_selectedControl, getContext().system->timescale());
        break;
    case SimulationControlID::Mass:
        m_bodyMass = std::max(m_bodyMass - m_controlValuesStep[m_selectedControl], Body::MassMin); 
        setSimulationControlValue(m_selectedControl, m_bodyMass);
        break;
    case SimulationControlID::Velocity:
        m_bodyVelocity = std::max(m_bodyVelocity - m_controlValuesStep[m_selectedControl], Body::VelocityMin); 
        setSimulationControlValue(m_selectedControl, m_bodyVelocity);
        break;
    case SimulationControlID::BodyMaterial:
        m_bodyMaterial = m_bodyMaterial > 0
            ? static_cast<Material>(m_bodyMaterial - 1)
            : static_cast<Material>(NB_MATERIALS - 1);
        setSimulationControlValue(m_selectedControl, m_bodyMaterial);
        break;
    }
}

void SimulationState::shootBody()
{
    const glm::vec3 cameraDirection = m_camera.getDirection();
    Body body{ m_camera.getPosition() + (cameraDirection * Body::radiusFromMass(m_bodyMass)), cameraDirection * m_bodyVelocity, m_bodyMass, m_bodyMaterial };
    getContext().system->addBody(body);
}