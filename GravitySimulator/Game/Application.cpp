#include "Application.h"
#include "States/TitleState.h"
#include "States/SimulationState.h"
#include "States/PausedSimulationState.h"
#include "States/SaveSimulationState.h"
#include "States/TestSimulationState.h"
#include "Engine/Physics/Serializer.h"
#include <SFML/OpenGL.hpp>
#include <SFML/Window/Event.hpp>
#include <filesystem>

namespace fs = std::filesystem;

const sf::Time Application::TimePerFrame = sf::seconds(1.f / 60.f);

Application::Application()
    : m_window{ sf::VideoMode{ 1920, 1080 }, "GravitySimulator", sf::Style::Default, sf::ContextSettings{ 32 } }
    , m_system{}
    , m_updateGame{ true }
    , m_stateStack{ State::Context{ m_window, m_textures, m_fonts, m_selectedSystem, m_materialTextures, m_skyBoxTextures, m_system } }
    , m_statisticsText{}
    , m_statisticsUpdateTime{}
    , m_statisticsNumFrames{ 0 }
    , m_selectedSystem{ "" }
{
    m_window.setKeyRepeatEnabled(true);
    m_window.setFramerateLimit(60);
    m_window.setActive(true);

    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        exit(1);
    }

    m_fonts.load(FontsID::Main, "Resources/Fonts/OpenSans-Light.ttf");
    m_textures.load(TexturesID::TitleScreen, "Resources/Textures/TitleScreenBig.png");
    m_textures.load(TexturesID::Crosshair, "Resources/Textures/Crosshair.png");

    // Load material textures for bodies
    for (auto& p : fs::directory_iterator("Resources/Textures/Materials"))
    {	
        Texture tempTexture;
        if (tempTexture.loadFromFile(p.path().string().c_str()))
            m_materialTextures.push_back(tempTexture);
    }

    // Load skybox textures for the cube map
    std::vector<std::string> textureName{
        "Resources/Textures/Materials/Skybox/Right.jpg",
        "Resources/Textures/Materials/Skybox/Left.jpg",
        "Resources/Textures/Materials/Skybox/Top.jpg",
        "Resources/Textures/Materials/Skybox/Bottom.jpg",
        "Resources/Textures/Materials/Skybox/Front.jpg",
        "Resources/Textures/Materials/Skybox/Back.jpg"
    };

    Texture skyBoxTexture;
    skyBoxTexture.loadSkyboxFromFile(textureName);
    m_skyBoxTextures.push_back(skyBoxTexture);

    m_statisticsText.setFont(m_fonts.get(FontsID::Main));
    m_statisticsText.setPosition(5.f, 5.f);
    m_statisticsText.setCharacterSize(10);

    registerStates();
    m_stateStack.pushState(StatesID::Title);

    glEnable(GL_DEPTH_TEST);
}

void Application::run()
{
    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
        
    while (m_window.isOpen())
    {
        sf::Time elapsedTime = clock.restart();
        timeSinceLastUpdate += elapsedTime;

        while (timeSinceLastUpdate > TimePerFrame)
        {
            timeSinceLastUpdate -= TimePerFrame;
            processInput();

            if (m_updateGame)
            {
                update(TimePerFrame);
            }		
        }

        updateStatistics(elapsedTime);
        render();
    }
}

void Application::runTest()
{
    m_stateStack.popState();
    m_stateStack.pushState(StatesID::Test);

    std::vector<size_t> fps;
    fps.reserve(5 * 60 * 60);

    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    sf::Time testDuration = sf::seconds(5.f * 60.f);

    while (testDuration > sf::Time::Zero)
    {
        sf::Time elapsedTime = clock.restart();
        timeSinceLastUpdate += elapsedTime;

        while (timeSinceLastUpdate > TimePerFrame)
        {
            timeSinceLastUpdate -= TimePerFrame;
            processInput();

            if (m_updateGame)
            {
                update(TimePerFrame);
            }
        }

        testDuration -= elapsedTime;
        m_statisticsUpdateTime += elapsedTime;
        m_statisticsNumFrames += 1;

        if (m_statisticsUpdateTime >= sf::seconds(1.0f))
        {
            fps.push_back(m_statisticsNumFrames);
            m_statisticsText.setString("FPS: " + std::to_string(m_statisticsNumFrames));
            m_statisticsUpdateTime -= sf::seconds(1.0f);
            m_statisticsNumFrames = 0;
        }

        render();
    }

    float avg = 0;
    for (auto& f : fps)
        avg += f;
    avg /= fps.size();

    size_t min = *std::min_element(begin(fps), end(fps));
    size_t max = *std::max_element(begin(fps), end(fps));

    std::ofstream results("Stats.txt");
    results << "Average: " << avg << std::endl << "Min: " << min << std::endl << "Max: " << max;
}

void Application::processInput()
{
    sf::Event event;

    while (m_window.pollEvent(event))
    {
        m_stateStack.handleEvent(event);

        switch (event.type)
        {
        case sf::Event::GainedFocus:
            m_updateGame = true;
            break;
        case sf::Event::LostFocus:
            m_updateGame = false;
            break;
        case sf::Event::Closed:
            m_window.close();
            break;
        case sf::Event::Resized:
            // adjust the viewport when the window is resized
            glViewport(0, 0, event.size.width, event.size.height);
            break;
        }
    }
}

void Application::update(sf::Time dt)
{
    m_stateStack.update(dt);
}

void Application::render()
{
    m_window.clear();
    glPushAttrib(GL_DEPTH_BUFFER_BIT);
    m_stateStack.draw();	
    m_window.setView(m_window.getDefaultView());
    m_window.draw(m_statisticsText);
    glPopAttrib();
    m_window.display();

}

void Application::updateStatistics(sf::Time dt)
{
    m_statisticsUpdateTime += dt;
    m_statisticsNumFrames += 1;

    if (m_statisticsUpdateTime >= sf::seconds(1.0f))
    {
        m_statisticsText.setString("FPS: " + std::to_string(m_statisticsNumFrames));
        m_statisticsUpdateTime -= sf::seconds(1.0f);
        m_statisticsNumFrames = 0;
    }
}

void Application::registerStates()
{
    m_stateStack.registerState<TitleState>(StatesID::Title);
    m_stateStack.registerState<SimulationState>(StatesID::Simulation);
    m_stateStack.registerState<PausedSimulationState>(StatesID::Paused);
    m_stateStack.registerState<SaveSimulationState>(StatesID::Save);
    m_stateStack.registerState<TestSimulationState>(StatesID::Test);
}