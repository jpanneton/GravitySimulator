#include "TitleState.h"
#include "Engine/Core/ResourceHolder.h"
#include "Engine/Physics/Body.h"
#include "Engine/Physics/Serializer.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <fstream>
#include <random>

TitleState::TitleState(StateStack& stack, Context context)
    : State{ stack, context }
    , m_selectSystemText{}
    , m_selectedSystemNameText{}
    , m_allSystems{}
    , m_selectedSystemIndex{}
{
    getContext().window->setMouseCursorGrabbed(false);
    getContext().window->setMouseCursorVisible(true);

    loadSystemsNames();

    m_backgroundSprite.setTexture(context.textures->get(TexturesID::TitleScreen));
    
    auto& font = context.fonts->get(FontsID::Main);
    m_selectSystemText.setFont(font);
    m_selectedSystemNameText.setFont(font);

    m_selectSystemText.setOutlineColor(sf::Color());
    m_selectedSystemNameText.setOutlineColor(sf::Color());
    m_selectSystemText.setOutlineThickness(1.f);
    m_selectedSystemNameText.setOutlineThickness(1.f);

    m_selectSystemText.setString("Syst�me s�lectionn�: ");
    m_selectedSystemNameText.setString(m_allSystems[m_selectedSystemIndex].stem().native());

    auto& windowSize = context.window->getView().getSize();
    m_selectSystemText.setPosition((3.f / 10.f) * windowSize.x, (4.f / 10.f) * windowSize.y);
    m_selectedSystemNameText.setPosition((3.f / 10.f) * windowSize.x, (5.f / 10.f) * windowSize.y);
}

void TitleState::draw()
{
    sf::RenderWindow& window = *getContext().window;
    window.resetGLStates();
    window.draw(m_backgroundSprite);

    window.draw(m_selectSystemText);
    window.draw(m_selectedSystemNameText);
}

bool TitleState::update(sf::Time dt)
{
    return true;
}

bool TitleState::handleEvent(const sf::Event& event)
{
    // If enter is pressed, trigger the next screen
    if (event.type == sf::Event::KeyPressed)
    {
        switch (event.key.code)
        {
        case sf::Keyboard::Enter:
            if (m_selectedSystemIndex == 0)
            {
                createRandomSystem();
            }

            *getContext().selectedSystem = "Systems/" + m_allSystems[m_selectedSystemIndex].string();

            requestStackPop();
            requestStackPush(StatesID::Simulation);
            break;
        case sf::Keyboard::Right:
            selectNextSystem();
            break;
        case sf::Keyboard::Left:
            selectPreviousSystem();
            break;
        case sf::Keyboard::Escape:
            getContext().window->close();
        }

    }

    return true;
}

void TitleState::loadSystemsNames()
{
    m_allSystems.push_back("Random.txt");
    for (auto& p : fs::directory_iterator("Systems"))
    {
        if (p.path().filename() != "Random.txt")
        {
            m_allSystems.push_back(p.path().filename());
        }
    }
}

void TitleState::selectNextSystem()
{
    m_selectedSystemIndex = (m_selectedSystemIndex + 1) % m_allSystems.size();
    m_selectedSystemNameText.setString(m_allSystems[m_selectedSystemIndex].stem().native());
}

void TitleState::selectPreviousSystem()
{
    m_selectedSystemIndex = m_selectedSystemIndex == 0 ? m_allSystems.size() - 1 : m_selectedSystemIndex - 1;
    m_selectedSystemNameText.setString(m_allSystems[m_selectedSystemIndex].stem().native());
}

void TitleState::createRandomSystem()
{
    const float minMass = 10.f;
    const float maxMass = 50'000.f;

    const int minMat = 0;
    const int maxMat = 13;

    const int minRadius = 1000;
    const int maxRadius = 10000;

    const int minVel = 0;
    const int maxVel = 50;

    const int bodyCount = 3000;
    const float z = 0.0f;
    const float sunMass = 200'000'000.f;

    std::mt19937 random{ std::random_device{}() };
    std::uniform_real_distribution<float> randomMass{ minMass, maxMass };
    std::uniform_int_distribution<int> randomMaterial{ minMat, maxMat };
    std::uniform_real_distribution<float> randomAngle{};
    std::uniform_int_distribution<int> randomRadius{ minRadius, maxRadius };
    std::uniform_int_distribution<int> randomVelocity{ minVel, maxVel };

    std::ofstream file("Systems/" + m_allSystems[m_selectedSystemIndex].string());

    file << Body{ { 0.f, 0.f, z }, { 0.f, 0.f, 0.f }, sunMass, Material::sun } << std::endl;

    for (int i = 0; i < bodyCount; ++i)
    {
        // Choose random mass and texture
        float mass = randomMass(random);
        Material material;
        do { material = static_cast<Material>(randomMaterial(random)); } while (material == 11);

        // Choose a random angle
        float angle = randomAngle(random) * (2.f * glm::pi<float>());

        // Choose a random radius
        int radius = randomRadius(random);

        // Convert polar to cartesian
        float x = radius * cos(angle);
        float y = radius * sin(angle);

        // Make a velocity vector at 90* of sun vector
        float vx = cos(angle + (glm::pi<float>() / 2.f));
        float vy = sin(angle + (glm::pi<float>() / 2.f));

        // Calculate velocity magnitude
        int base_velocity = randomVelocity(random);
        float ratio = static_cast<float>(maxRadius) / radius;
        ratio *= ratio;
        vx *= ratio * base_velocity;
        vy *= ratio * base_velocity;

        // Create and return the body
        file << Body{ { x, y, z }, { vx, vy, 0.f }, mass, material } << std::endl;
    }
}
