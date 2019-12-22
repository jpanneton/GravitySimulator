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

    m_selectSystemText.setString("Système sélectionné: ");
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
    const float pi = 3.14159265359f;

    const float min_mass = 10.f;
    const float max_mass = 50'000.f;

    const int min_mat = 0;
    const int max_mat = 13;

    const int min_radius = 1000;
    const int max_radius = 10000;

    const int min_vel = 0;
    const int max_vel = 50;

    const int nb_bodies = 1000;
    const float z = -15000.f;
    const float sun_mass = 200'000'000.f;

    std::mt19937 random{ std::random_device{}() };
    std::uniform_real_distribution<float> randomMass{ min_mass, max_mass };
    std::uniform_int_distribution<int> randomMaterial{ min_mat, max_mat };
    std::uniform_real_distribution<float> randomAngle{};
    std::uniform_int_distribution<int> randomRadius{ min_radius , max_radius };
    std::uniform_int_distribution<int> randomVelocity{ min_vel , max_vel };

    std::ofstream file("Systems/" + m_allSystems[m_selectedSystemIndex].string());

    file << Body{ { 0.f, 0.f, z }, { 0.f, 0.f, 0.f }, sun_mass , Material::sun } << std::endl;

    for (int i = 0; i < nb_bodies; ++i)
    {
        // Choose random mass and texture
        float mass = randomMass(random);
        Material material;
        do { material = static_cast<Material>(randomMaterial(random)); } while (material == 11);

        // Choose a random angle
        float angle = randomAngle(random) * (2.f * pi);

        // Choose a random radius
        int radius = randomRadius(random);

        // Convert polar to cartesian
        float x = radius * cos(angle);
        float y = radius * sin(angle);

        // Make a velocity vector at 90* of sun vector
        float vx = cos(angle + (pi / 2.f));
        float vy = sin(angle + (pi / 2.f));

        // Calculate velocity magnitude
        int base_velocity = randomVelocity(random);
        float ratio = static_cast<float>(max_radius) / radius;
        ratio *= ratio;
        vx *= ratio * base_velocity;
        vy *= ratio * base_velocity;

        // Create and return the body
        file << Body{ { x, y, z }, { vx, vy, 0.f }, mass , material } << std::endl;
    }
}
